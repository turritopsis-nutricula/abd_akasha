#ifndef AKASHA_SERVER__H
#define AKASHA_SERVER__H

//gpp serverTest.cpp -lboost_date_time-mgw46-mt-d-1_47 -lboost_serialization-mgw46-mt-d-1_47 -lwsock32 -lws2_32

#include "../connection.hpp"
#include "../CommunicateData.hpp"

//#include "STUN_Observer.hpp"

//std
#include <set>

//boost
#include <boost/utility.hpp>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/thread.hpp>

//akasha

namespace akasha
{
	namespace network
	{
		namespace server
		{
			using boost::asio::ip::tcp;

			//! 部屋
			template<typename ParticipantType>
				class ServerDispatch : boost::noncopyable
				{
					//typedef PaketData DataType;
					//typedef V VisiterType;
					typedef boost::shared_ptr<ParticipantType> INetworkParticipant_ptr;

					std::set<INetworkParticipant_ptr> m_participants;
					data::ServerState m_state;

					boost::posix_time::ptime lastChat_;

				public:
					ServerDispatch() = delete;
					ServerDispatch(boost::asio::io_service& io) {}

					//クライアントメンバー仮接続
					void
					preJoin(INetworkParticipant_ptr participant)
					{
						m_participants.insert(participant);
					}

					//クライアントメンバー本登録
					void
					join(
							INetworkParticipant_ptr participant,
							const client::request::SessionEnter& d,
							const std::string& globalAddress
							)
					{
						const auto i = m_participants.find(participant);
						if (i!=m_participants.end())
						{

							boost::uuids::nil_generator gen;  //ModelID();
							m_state.players_.push_back(
									data::Player {d.name_, d.HostID_, gen(), globalAddress}
							);

							notifyAll();  //通知
						}
						else
						{
							//すでに登録済み
							//leave(participant);
						}
					}

					//クライアントメンバー離脱
					void
					leave(INetworkParticipant_ptr participant)
					{

						m_participants.erase(participant);

						if (const auto& id = participant->getUuid())
						{

							auto& players = m_state.players_;
							auto i = boost::range::find_if(
									players,
									[&id](const data::Player& p) {return p.HostID_ == *id;}
							);

							//該当したらプレイヤー情報を消去したのち通知発進
							if (i!=players.end())
							{

								players.erase(i);
								notifyAll();  //通知
							}
						}

						//ここでparticipantはデストラクタ実行(されるはず)

					}

					//////////////////
					//送信
					//! データ一括送信

					void
					notifyAll()
					{
						std::vector<boost::uuids::uuid> playerIDbuf;
						boost::range::for_each(
								m_state.players_,
								[&playerIDbuf](const data::Player& d) {playerIDbuf.push_back(d.HostID_);}
						);

						server::notify::ServerNotify data {playerIDbuf, lastChat_};


						boost::range::for_each(m_state.players_, [&](const data::Player& p){

							BOOST_FOREACH( INetworkParticipant_ptr con, m_participants) {

								if (auto id = con->getUuid())
									if (*id == p.HostID_)
										con->notify(data);

							}
						});


					}

					//! 接続数
					const int
					getParticipantsCount() const
					{
						return m_participants.size();
					}

					data::ServerState&
					getServerState()
					{
						return m_state;
					}

					void
					addChat(const data::ChatData& d)
					{
						m_state.chats_.push_back(d);
						lastChat_ = boost::posix_time::second_clock::local_time();
						m_state.chats_.back().time_ = lastChat_;

						notifyAll();
					}
					std::vector<data::ChatData>&
					getChatLog()
					{
						return m_state.chats_;
					}
					std::vector<data::ChatData>
					getChatLog(const boost::posix_time::ptime& border)
					{
						std::vector<data::ChatData> buf(m_state.chats_);
						boost::range::remove_erase_if(buf, [&border](const data::ChatData& c)->bool {return c.time_ < border;});

						return std::move(buf);
					}

					data::Player
					getPlayer(const boost::uuids::uuid id) const
					{
						auto hit = boost::range::find_if(m_state.players_, [id](const data::Player& p){ return id == p.HostID_;} );
						assert( hit != m_state.players_.end() );
						return *hit;
					}
					void
					sendChatLog(INetworkParticipant_ptr p)
					{
						//p->reply(id_, reply::ChatLog {s.chats_});
					}

				};

			template<typename ParticipantType>
				class DispatchHolder : boost::noncopyable
				{

				private:
					boost::asio::io_service::strand guard_;
					ServerDispatch<ParticipantType> dispatch_;
				public:
					typedef ServerDispatch<ParticipantType> dispatch_type;
				public:
					DispatchHolder() = delete;
					DispatchHolder(boost::asio::io_service& s) : guard_(s), dispatch_(s) {}

					template<typename F>
					void
					post(const F& f)
					{
						guard_.get_io_service().post(
								guard_.wrap([&dispatch_, f]() {f(dispatch_);}));
					}

					template<typename F>
					void
					dispatch(const F& f)
					{
						guard_.get_io_service().dispatch(
								guard_.wrap([&dispatch_, f]() {f(dispatch_);}));
					}

				};

				//////////////////////////////////////////////////////////////////
				//セッションビジター
			template<typename SessionType, typename DispatchType>
				struct ServerSessionVisiter : boost::static_visitor<void>
				{

					boost::uuids::uuid id_;
					boost::shared_ptr<SessionType> session_;
					DispatchType* dispatch_;

					ServerSessionVisiter(const boost::uuids::uuid& id,
							boost::shared_ptr<SessionType> s, DispatchType* d) :
							id_(id), session_(s), dispatch_(d)
					{
					}

					void
					sendOK() const
					{
						session_->reply(id_, reply::RequestOK());
					}
					void
					sendFaild() const
					{
						session_->reply(id_, reply::RequestFailed());
					}
					void
					nofityAll() const
					{

						dispatch_->dispatch(
								[](typename DispatchType::dispatch_type& d) {d.notifyAll();});

					}
					template<typename T>
						void
						operator()(const T& t) const
						{
#ifdef COUT_DEBUG
							std::cout << "\n##Server Recive Unkown: " << id_ << std::endl;
#endif
						}

					void
					operator()(const client::request::SessionLeave& d) const
					{
#ifdef COUT_DEBUG
						std::cout << "\n##Server Recive Bye:" << id_ << std::endl;
#endif

						session_->closeStanby();
						sendOK();



					}

					void
					operator()(const client::request::Ping& d) const
					{
#ifdef COUT_DEBUG
						std::cout << "\n##Server Reply Ping" << std::endl;
#endif
						session_->reply(id_, reply::PingReply { d.sendTime_ });
					}

					void
					operator()(const client::request::SendChat& chat) const
					{
#ifdef COUT_DEBUG
						std::cout << "\n##Server Reply SendChat" << std::endl;
#endif
						sendOK();

						dispatch_->post(
								[chat](typename DispatchType::dispatch_type& d ) {d.addChat(chat.chat_);});
						/*dispatch_->accessServerState(
						 [ this, chat]( data::ServerState& s) {
						 s.chats_.push_back(chat.chat_);
						 dispatch_->notifyAll(notify::ServerNotify());
						 });*/

						//nofityAll();
					}

					void
					operator()(const client::request::GetChatLog& get) const
					{

						typedef typename DispatchType::dispatch_type arg_type;
						boost::uuids::uuid id = id_;
						boost::shared_ptr<SessionType> s = session_;
						boost::posix_time::ptime time = get.borderTime_;
						dispatch_->dispatch([id, s, time](arg_type& d) {
							s->reply(id, reply::ChatLog {d.getChatLog(time)});
						});

					}

					void
					operator()(const client::request::GetPlayerData& r) const
					{
						typedef typename DispatchType::dispatch_type arg_type;
						boost::shared_ptr<SessionType> s = session_;
						boost::uuids::uuid requestId = id_;
						dispatch_->post([ r,s,requestId](arg_type& dis){
							s->reply(requestId, reply::ReplyPlayerData{ dis.getPlayer(r.targetID_) });

						});
					}

				};
			//////////////////////////////////////////////////////////////////
			//! TCPセッションソケット
			//TODO: strandで保護
			class TCP_Session : public boost::enable_shared_from_this<
					TCP_Session>
			{
			private:

				typedef DispatchHolder<TCP_Session> dispatchHolder_type;
				typedef ServerDispatch<TCP_Session> dispatch_type;

				TCP_connection m_connection;  //コネクション
				dispatchHolder_type& m_roomRef;  //サーバーデータ

				boost::optional<boost::uuids::uuid> m_connectionUuid;  //! 接続先のUuid

				client::ClientSendData m_inRequest;  //! 受信する一時記憶領域
				boost::asio::io_service::strand m_protocolGuard;
				boost::asio::io_service::strand m_writeGuard;

				bool m_lastWrite;
			public:
				TCP_Session(boost::asio::io_service& io_serveice,
						dispatchHolder_type& room) :
						m_connection(io_serveice), m_roomRef(room), m_protocolGuard(
								io_serveice), m_writeGuard(io_serveice), m_lastWrite(
								false)
				{
				}

				//! ソケット参照
				tcp::socket&
				socket()
				{
					return m_connection.socket();
				}

				const tcp::socket&
				socket() const
				{
					return m_connection.socket();
				}
				const boost::optional<boost::uuids::uuid>&
				getUuid() const
				{
					return m_connectionUuid;
				}

				//! セッション開始
				void
				start()
				{
#ifdef COUT_DEBUG
					std::cout << "#Server start:" <<
					boost::this_thread::get_id() << std::endl;
#endif
					//boost::shared_ptr<TCP_Session> this_ = shared_from_this();
					auto this_ = shared_from_this();  //startから抜けるとptrが0になるため、関数オブジェクトに持たせて延命する
					m_roomRef.post([&,this_](dispatch_type& d) {
						d.preJoin(this_->shared_from_this());

						//初回待ち受けをセット
							this->m_connection.async_read(
									this->m_inRequest,
									boost::bind(&TCP_Session::handle_read_first, this,
											boost::asio::placeholders::error));
						});

				}
				//! セッション停止
				void
				closeStanby()
				{
					m_lastWrite = true;
				}



				//! ソケット待ち受けをセット
				void
				setReaderHandle()
				{
					//待ち受けをセット
					m_connection.async_read(
							m_inRequest,
							boost::bind(&TCP_Session::handle_read_request, this,
									boost::asio::placeholders::error));
				}

				//! ソケット先に返信
				template<typename ReplyDataType>
					void
					reply(const boost::uuids::uuid& id,
							const ReplyDataType& data)
					{
						//m_protocolGuard.post([&, id, data](){
						m_writeGuard.post([&, id, data]() {
							//std::cout << "Server Reply id:" << id << std::endl;
								server::reply::ReplyData d = {id,data};
								server::ServerSendData send = d;
								m_connection.async_write(
										send,
										boost::bind(&TCP_Session::handle_write, this, boost::asio::placeholders::error)
								);

							});
					}

				//! ソケット先に送信
				void
				notify(const server::notify::ServerNotify& data)
				{

					//m_writeGuard.get_io_service().post(
					m_writeGuard.post([&, data]() {

						//TODO : dataが破損している?
						server::ServerSendData send(data);
						m_connection.async_write(
								send,
								boost::bind(&TCP_Session::handle_write, this,
										boost::asio::placeholders::error));

					}
					//)
					);
				}

			private:

				//初回通信用ハンドラ
				void
				handle_read_first(const boost::system::error_code& e)
				{
#ifdef COUT_DEBUG
					std::cout << boost::format("\n#Server First Read:%1%") % m_inRequest.data_.which() << std::endl;
#endif

					if (!e) {
						/*m_connection.socket().get_io_service().dispatch(
								m_protocolGuard.wrap(*/
						m_protocolGuard.dispatch(
										[&,e]() {

											try {
												const client::request::SessionEnter& d =
												boost::get<client::request::SessionEnter>(this->m_inRequest.data_);
												this->m_connectionUuid = d.HostID_;
												//std::cout << "#Server : recived.id:" << (*m_connectionUuid) << std::endl;

												auto remoteAddress = m_connection.socket().remote_endpoint().address();
												this->m_roomRef.post([&,d](dispatch_type& dis) {
															dis.join(this->shared_from_this(), d, remoteAddress.to_string());

															this->reply(this->m_inRequest.id_, server::reply::RequestOK());
															this->setReaderHandle();
														});
											}
											catch( const boost::bad_get& e)
											{
												//TODO : これでいいのやら

												std::cout << "Server::firstRead" << e.what() << std::endl;
											}

										}
						//)
										);
					}
				}
				//リクエストハンドラ
				void
				handle_read_request(const boost::system::error_code& e)
				{
#ifdef COUT_DEBUG
					std::cout << boost::format("\n#ServerRead:%1%") % m_inRequest.data_.which() << std::endl;
#endif

					if (!e) {

						m_connection.socket().get_io_service().post(
								m_protocolGuard.wrap(
										boost::bind(
												&TCP_Session::handle_do_request,
												this->shared_from_this(),
												this->m_inRequest)));

						this->setReaderHandle();
					}
					else
					{
#ifdef COUT_DEBUG
						std::cout << "\n#ServerRead Error:" << e.value() << ":" << e.message() << std::endl;
#endif
						m_roomRef.post(
									[&](dispatch_type& d) {d.leave(this->shared_from_this());});

						if (e!=boost::asio::error::misc_errors::eof) //ソケットがクローズされた以外
							std::cout << "ServerRead Error:" << e.value() << ":" << e.message() << std::endl;

						//TODO : 切断された以外のエラー処理
						//m_connection.socket().close();

					}
				}
				void
				handle_do_request(const client::ClientSendData inData)
				{
#ifdef COUT_DEBUG
					//std::cout << "\n#Server Do Request:" << inData.data_.which() << std::endl;
#endif
					assert(
							inData.data_.which() >= 0 &&
							inData.data_.which() < boost::mpl::size<typename client::ClientSendData::request_type::types>::value
							);
					boost::apply_visitor(
							ServerSessionVisiter<TCP_Session,
									dispatchHolder_type>(inData.id_,
									this->shared_from_this(),
									&(this->m_roomRef)),
							inData.data_);
				}

				void
				handle_write(const boost::system::error_code& e)
				{
					if (!e && m_lastWrite) {
						//m_connection.socket().close();

						/*m_roomRef.post(
								[&](dispatch_type& d) {d.leave(this->shared_from_this());});*/
					}
				}

			};

			typedef boost::shared_ptr<TCP_Session> TCP_Session_ptr;

			//! ネットワークサーバー
			class NetworkServer : boost::noncopyable
			{
				//typedef PaketData DataType;
				typedef boost::mutex::scoped_lock acceptLock;

			private:
				boost::asio::io_service& m_ioService;  //!< ioサーバー
				std::string m_listenPort;  //!< 待ち受けポート
				tcp::acceptor m_acceptor;  //!< アクセプタ
				boost::asio::io_service::strand m_acceptorStrand;  //!<アクセプタ排他処理


				boost::function<void
				(bool)> m_acceptCallback;  //!< アクセプトコールバック

				DispatchHolder<TCP_Session> m_dispatcher;  //!< 部屋
				//DispatchHandler m_dispatchHandle; //! ディスパッチハンドラ

			public:
				NetworkServer(boost::asio::io_service& io_service,
						const std::string& listenPort) :
						m_ioService(io_service), m_listenPort(listenPort), m_acceptor(
								io_service,
								tcp::endpoint(tcp::v4(),
										std::atoi(listenPort.c_str()))), m_acceptorStrand(
								io_service), m_dispatcher(io_service)
				{
					//ポート作成
					create_accept();
				}

				//! 待ち受けポートを新設
				void
				create_accept()
				{

					//新しいセッションを作成
					TCP_Session_ptr new_session(
							new TCP_Session(m_ioService, m_dispatcher));
#ifdef COUT_DEBUG
					std::cout << "accept create thread:" <<

					boost::this_thread::get_id() << std::endl;
#endif
					//登録
					m_acceptor.async_accept(
							new_session->socket(),
							//m_acceptorStrand.wrap(
							boost::bind(&NetworkServer::handle_accept, this,
									new_session,
									boost::asio::placeholders::error)
									//)
									);

				}

				//! アクセプトハンドラ
				void
				handle_accept(TCP_Session_ptr session,
						const boost::system::error_code& error)
				{
#ifdef COUT_DEBUG
					std::cout << "##Server Accept:" << boost::this_thread::get_id() << std::endl;
#endif
					if (!error) {
						session->start();
						create_accept();
					}
					else {
						//995はポートクローズ
						if (error.value()
								!= boost::asio::error::operation_aborted) {
							std::cout << error.message() << std::endl;
							assert(false);
						}
					}
				}

				//! 待ち受けポート
				const std::string&
				getListenPort()
				{
					return m_listenPort;
				}

				//! 繋がってるクライアント数
				const int
				getClientCount()
				{
					//TODO : 要実装
					return 0;  //m_dispatcher.getParticipantsCount();
				}

				//! サーバークローズ
				void
				close()
				{

					//TODO :ディスパッチャに閉じるコール
					m_acceptor.close();
				}

				//! 書き込み
				void
				notify()
				{
					//m_dispatcher.notifyAll(server::notify::ServerNotify());
				}

			};
		}
	}
}

#endif //AKASHA_SERVER__H
