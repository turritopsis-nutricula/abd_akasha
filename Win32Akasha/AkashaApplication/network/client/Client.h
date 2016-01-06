#ifndef AKASHA_CLIENET__H
#define AKASHA_CLIENET__H

#include "../connection.hpp"

//boost
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/unique.hpp>

#include <boost/range/algorithm_ext/erase.hpp>


#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

#include <boost/thread.hpp>

#include <boost/date_time/posix_time/ptime.hpp>


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
//akasha
//#include "akashaCommon.h"



namespace akasha
{
	namespace network
	{

		namespace client
		{
			class ClientData
			{
				typedef std::string string_type;
			public:
				//std::vector<data::ChatData> chats_;
				struct Time{};
				struct UniqueID{};


				typedef boost::multi_index::multi_index_container<
						data::ChatData,
						boost::multi_index::indexed_by<
						boost::multi_index::ordered_non_unique<
						boost::multi_index::tag<Time>,
						boost::multi_index::member<data::ChatData, boost::posix_time::ptime, &data::ChatData::time_>>,
						boost::multi_index::hashed_unique<
						boost::multi_index::tag< UniqueID >,
						boost::multi_index::member<data::ChatData, boost::uuids::uuid, &data::ChatData::chatID_ >>
						>
						> chatBufferType;

				std::vector<boost::uuids::uuid> playerIDList_;
				data::Player myData_;
				std::vector<data::Player> otherPlayers_;
				chatBufferType chats_;

				boost::posix_time::ptime lastChatTime_;

				std::vector< OtherModelBase<std::string> > m_modelBaseBuffer; //TODO : ����string

				ClientData() = delete;
				ClientData(
						const network::StringType& name,
						boost::uuids::uuid id) :
							myData_({name, id, boost::uuids::uuid()}),
							lastChatTime_(boost::posix_time::second_clock::local_time()){ }

				std::vector<data::Player>
				getPlayerList() const
				{
					std::vector<data::Player> res;
					auto iter = otherPlayers_.cbegin();
					BOOST_FOREACH(const auto& id, playerIDList_)
					{
						//sort���Ă���̂��O��
						if (iter->HostID_!=id)
							res.push_back(myData_);
						else
						{
							res.push_back(*iter);
							iter++;
						}
					}
					return std::move(res);
					//return res;
				}


				///////////////////////////////////////////////////////////////
				//�l�b�g���[�N�o�R�̌Ăяo��

				//�`���b�g�f�[�^���d�����l�����ǉ�
				void
				addChatData(const data::ChatData& c)
				{
					chats_.get<UniqueID>().insert(c);

					lastChatTime_ = (*( chats_.get<Time>().rbegin()) ).time_;
				}

				//! �v���C���[ID���X�g���X�V�B�V�K�擾���K�v�ȃv���C���[��ID�݂̂��c���B
				void
				comperePlayerList(std::vector<boost::uuids::uuid>& uuidList)
				{
					playerIDList_ = uuidList;
					boost::range::remove_erase_if(uuidList, [&](const boost::uuids::uuid id){return id == myData_.HostID_; });

					//�������v���C���[������
					std::vector<data::Player> newBuf;
					boost::for_each(uuidList,[this,&newBuf](const boost::uuids::uuid& id){
						auto i = boost::range::find_if(otherPlayers_, [&id](data::Player& p){ return p.HostID_ == id; });
						if (i!= otherPlayers_.end())
							newBuf.push_back( *i );
					});
					otherPlayers_.swap(newBuf); //�V���ȃ��X�g�ɁB

					//�V�K�擾���K�v�ȃv���C���[���X�g�ɐ��`
					boost::for_each(otherPlayers_, [&uuidList](const data::Player& p){
						const auto& playerId = p.HostID_;
						boost::range::remove_erase_if(uuidList, [playerId](const boost::uuids::uuid& i){ return playerId == i; });
					});

					/*
					std::cout << "compere my" << myData_.sessionID_ << "cout:" << uuidList.size() << "\n";
					BOOST_FOREACH(const auto& i, uuidList)
					{
						std::cout << i << "\n";
					}
					std::cout << std::endl;*/
				}

				void
				addPlayer(const data::Player& newPlayer)
				{
					assert(newPlayer.HostID_!=myData_.HostID_);
					otherPlayers_.push_back( newPlayer);
					sortOtherPlayerList();


				}

			private:
				void
				sortOtherPlayerList()
				{
					auto iter = otherPlayers_.begin();
					boost::for_each(playerIDList_, [this,&iter](const boost::uuids::uuid& id){

						//�G���h�ő����^�[��
						if (iter==otherPlayers_.end()) return;

						if (myData_.HostID_==id)
							iter++;
						else if (iter->HostID_==id)
							iter++; //�q�b�g�����Ȃ玟��
						else
						{
							//players���̊Y��������
							auto target = boost::range::find_if(otherPlayers_, [&id](data::Player& p){ return p.HostID_==id; });
							//�q�b�g�����炻��Ɠ���ւ��A����
							if (target != otherPlayers_.end())
							{
								std::iter_swap(iter, target);
								iter++;
							}
						}

					});
				}

			public:
				//! ���f���x�[�X�f�[�^�o�b�t�@�֒ǉ�
				void
				addModelBaseData(OtherModelBase<string_type> const& d)
				{
					m_modelBaseBuffer.push_back(d);
				}
				//! ���f���x�[�X�f�[�^�o�b�t�@����move
				void
				moveModelBaseData(std::vector<OtherModelBase<string_type> >& v)
				{
					v.swap(m_modelBaseBuffer);
					m_modelBaseBuffer.clear();
					m_modelBaseBuffer.resize(0);
				}
			};

			//�f�[�^�z���_�[
			class ClientDataHolder
			{
				ClientData data_;
				mutable boost::asio::io_service::strand gurad_;
			public:
				ClientDataHolder() = delete;
				ClientDataHolder(
						boost::asio::io_service& s,
						const network::StringType& name,
						boost::uuids::uuid id
						) : data_(name, id), gurad_(s) {}

				//TODO : ���[�u�ł�����?
				template<typename F>
				void
				access(F f)
				{

					gurad_.dispatch([&,f]() {f(this->data_);});
				}
				template<typename F>
				void
				constAccess(F&& f) const
				{
					gurad_.dispatch(  boost::bind(f,this->data_) );
				}

			};

		}
		//typedef akasha::network::SegmentData<boost::uuids::uuid> DataType;

		using boost::asio::ip::tcp;

		template<typename V>
			class NetworkClientBase
			{
				typedef V VisiterType;
				typedef NetworkClientBase<V> this_type;
				typedef std::vector<data::ChatData> chatData_type;

			private:

				//! reply�҂��Ǘ�
				class replyWaitBuffer
				{
				public:
					typedef boost::function<void
					(const server::reply::ReplyData&, client::ClientData&)> request_finish_handler_type;

				private:
					boost::unordered_map<boost::uuids::uuid,
							request_finish_handler_type> m_buffer;
					std::vector<
							std::tuple<boost::uuids::uuid,server::reply::ReplyData,
									int>> m_retrayBuffer;

					boost::asio::io_service::strand gurad_;

				public:
					boost::uuids::uuid clientID_;

					replyWaitBuffer() = delete;
					replyWaitBuffer(boost::asio::io_service& s) : gurad_(s) {}

					template<typename F>
					void
					in(const boost::uuids::uuid i, F f)
					{
						gurad_.dispatch([&,i, f]() {
							this->m_buffer[i] = f;
						});

					}
					request_finish_handler_type
					out(const boost::uuids::uuid i)
					{

						assert(m_buffer.count(i)>0);

						request_finish_handler_type res(m_buffer.at(i));
						m_buffer.erase(i);

						return std::move(res);

					}
					void
					finish(const boost::uuids::uuid i, const server::reply::ReplyData& r, client::ClientData& d )
					{


						gurad_.dispatch([&,i,r]() {

							//���g���C�҂�
							typedef std::tuple<boost::uuids::uuid, server::reply::ReplyData, int> value_type;
							boost::range::remove_erase_if(m_retrayBuffer, [&](value_type& v){

								//std::cout << "retray : count" << v.second << std::endl;
								if (m_buffer.count(std::get<0>(v))>0){
									//�f�o�b�N�R�[�h
									std::cout << "retry hit. " << std::endl;

									m_buffer.at(std::get<0>(v))(std::get<1>(v), d);
									m_buffer.erase(std::get<0>(v));
									return true;
								}
								else
								{
									std::get<2>(v)++;
									return false;
								}
							});


									if (this->m_buffer.count(i)>0)
									{
										/*if (this->m_buffer.count(i)>1)
											std::cout << "multi hit." << std::endl;*/

										this->m_buffer.at(i)(r,d);
										this->m_buffer.erase(i);
									}
									else
									{
										this->m_retrayBuffer.emplace_back( i,r,0 );
										///�ȉ��Ńo�b�N�R�[�h
										std::cout <<
												boost::format("no hit. reply.\n   type:%1% id:%3%\n   clientID:%4%\n   retrayBufSize %2%") %
												r.data_.which() %
												m_retrayBuffer.size() %
												i %
												clientID_
												<< std::endl;
									}

								});

					}

					void
					print() const
					{

						std::cout << boost::format("id:%2%\n bufferCount:%1%") % m_buffer.size() % clientID_;
						BOOST_FOREACH(const auto& b, m_buffer)
						{
							std::cout << "\n" << b.first;
						}
						std::cout << boost::format("\n retry Count:%1%") % m_retrayBuffer.size();
						BOOST_FOREACH(const auto& b, m_retrayBuffer)
						{
							std::cout << "\n" << std::get<0>(b);
						}
						std::cout << std::endl;
					}

				};
			private:
				boost::uuids::random_generator m_requestIDGenerator;  //! ���N�G�X�gUuid������
				boost::asio::io_service& m_ioService;

				std::string m_name;
				const boost::uuids::uuid m_HostID;//! Uuid

				std::string m_host;
				std::string m_accessPort;

				TCP_connection m_connection;

				server::ServerSendData m_inReply;//! �o�b�t�@
				bool m_isLogin;
				client::ClientDataHolder m_data;

				replyWaitBuffer m_replyWaitBuffer;

				boost::asio::io_service::strand m_protocolGuard;
				boost::asio::io_service::strand m_readHandlerGuard;



			public:
				NetworkClientBase(
						boost::asio::io_service& io_service,
						const std::string name,
						const boost::uuids::uuid id,
						const std::string& host,
						const std::string& port) :
				m_ioService(io_service),
				m_name(name),
				m_HostID(id),
				m_host(host),
				m_accessPort(port),
				m_connection(m_ioService),
				m_isLogin(false),
				m_data(io_service, m_name, m_HostID),
				m_replyWaitBuffer(io_service),
				m_protocolGuard(m_ioService),
				m_readHandlerGuard(m_ioService)
				{
					m_replyWaitBuffer.clientID_ = id;

					tcp::resolver resolver(m_ioService);
					tcp::resolver::query query(m_host.c_str(), m_accessPort.c_str());
					tcp::resolver::iterator iterator = resolver.resolve(query);
					tcp::endpoint endpoint = *iterator;

					m_connection.socket().async_connect(
							endpoint,
							boost::bind(&this_type::handle_connect,this, boost::asio::placeholders::error,++iterator)
					);

				}

				virtual ~NetworkClientBase()
				{
					do_close();
				}

				void print() const { return m_replyWaitBuffer.print(); }

				//! �ڑ��Ώۃz�X�g����Ԃ�
				const std::string& getHostName() const {return m_host;};

				//! �ڑ��|�[�g��Ԃ�
				const std::string& getAccessPort() const {return m_accessPort;};

				//! �ڑ���Ԃ�Ԃ�
				bool hasConnect() const {return m_connection.socket().is_open();};

				//! �T�[�o�[�ւ̐ڑ���
				bool isLogin() const {return hasConnect() && m_isLogin;}

				//Ping����
				void
				ping()
				{

					request(client::request::Ping {boost::posix_time::microsec_clock::local_time()});
				}

				//�`���b�g����
				void
				sendChat(const network::StringType& chat)
				{

					boost::uuids::name_generator gen(m_HostID);
					request(
							client::request::SendChat { {m_HostID, gen(chat.c_str()),boost::posix_time::second_clock::local_time(),chat,}}

					);

				}
				//! �N���[�Y
				void close()
				{
					if (hasConnect())
					{
#ifdef COUT_DEBUG
						std::cout << "###Client close Start." << std::endl;
#endif


						request( client::request::SessionLeave(),
								[this](const server::reply::ReplyData&, client::ClientData&)
								{
									this->m_isLogin = false;
									this->do_close();
								});

					}
					else
					do_close();
				}

				//! �V�����擾�������f���f�[�^���擾
				template<typename StringType>
				void
				getNewModelBase(boost::promise<std::vector<OtherModelBase<StringType> >>& p)
				{
					accessData([&p](client::ClientData& client){
						std::vector<std::vector<OtherModelBase<StringType>> > r;
						client.moveModelBaseData(r);
						p.set_value(r);
					});
				}

				//! Uuid���擾
				boost::uuids::uuid getUuid() const {
					return m_HostID;
				}

				//TODO : ���[�u�ł�����?
				template<typename F>
				void
				accessData(F f)
				{
					m_data.access(f);
				}

				template<typename F>
				void
				constAccessData(F&& f) const
				{
					m_data.constAccess(std::move(f));
				}
			private:
				//���N�G�X�g��Send
				template<typename RequestType>
				void
				request(const RequestType& r)
				{
					request(r, [](const server::reply::ReplyData&, client::ClientData&) {});
				}
				template<typename RequestType,typename HandlerType>
				void
				request(const RequestType& r, HandlerType&& h)
				{

					//TODO : m_protoclGuard.post(�ł悭��?
					m_ioService.post(
							m_protocolGuard.wrap(
									[&, r, h]() {
										boost::uuids::uuid requiestID = m_requestIDGenerator();
										this->m_connection.async_write(
												client::ClientSendData {requiestID, r},
												//this->m_readHandlerGuard.wrap(
												boost::bind(
														&this_type::handler_request_finish<HandlerType>,
														this,
														boost::asio::placeholders::error,
														requiestID,
														h)
												//)
										);}
							));

				}
				///���N�G�X�g�������݊����n���h��
				template<typename HandlerType>
				void handler_request_finish(const boost::system::error_code& e,const boost::uuids::uuid requestID, HandlerType h)
				{
					//reply�҂��o�b�t�@�[�ɓo�^
					if (!e)
					{
						m_replyWaitBuffer.in(requestID, h);
					}
					else
					{
						std::cout << "Client Request Write Error:" << e.value() <<":"<< e.message() << std::endl;
					}
				}

			private:

				//! �R�l�N�g�n���h��
				void handle_connect(const boost::system::error_code& error,
						tcp::resolver::iterator endpoint_iterator)
				{

#ifdef COUT_DEBUG
					std::cout << "###Client ConnectHander:" << std::endl;
#endif

					if (!error)
					{

#ifdef COUT_DEBUG
						std::cout << "###Client FirstWrite:" << std::endl;
#endif

						std::string address = m_connection.socket().local_endpoint().address().to_string();
						request(
								client::request::SessionEnter {m_name, m_HostID, address},
								[&](const server::reply::ReplyData&, client::ClientData&) {
									this->m_isLogin = true;
								});

						this->setReaderHandle();

					}
					else if ( endpoint_iterator != tcp::resolver::iterator())
					{
						this->do_close();
						tcp::endpoint endpoint = *endpoint_iterator;
						m_connection.socket().async_connect(
								endpoint,
								boost::bind(&this_type::handle_connect,
										this,
										boost::asio::placeholders::error,
										++endpoint_iterator));
					}

				}

				//Connectoin��Read���d�|����
				void setReaderHandle()
				{
					m_connection.async_read(
							m_inReply,
							m_readHandlerGuard.wrap(
									boost::bind(
											&this_type::handle_read,
											this,
											boost::asio::placeholders::error)
							));
				}

				//Read�n���h��
				void handle_read(const boost::system::error_code& e)
				{
					if (!hasConnect())
						return;

					if (e)
					{
						std::cout << "Client Read: "<< e.message() << std::endl;
						return;
					}

#ifdef COUT_DEBUG
					std::cout << boost::format("###Client Read. %1%") % (m_inReply.which()==1 ? "reply" : "notify") << std::endl;
#endif
					m_ioService.post(
							[this]() {
						assert(m_inReply.which() >= 0 && m_inReply.which() < boost::mpl::size<typename server::ServerSendData::types>::value );
										if (m_inReply.which() == 1)  //ReplyData
										{
											const server::reply::ReplyData& data = boost::get<server::reply::ReplyData>(m_inReply);

											m_data.access([&](client::ClientData& c) {
														m_replyWaitBuffer.finish(data.replyID_, data, c);
													});


										}
										else  if(m_inReply.which() == 0)//notify
										{

											//�v���C���[�X�V
											const server::notify::ServerNotify& notify = boost::get<server::notify::ServerNotify>(m_inReply);
											m_data.access([&,notify](client::ClientData& c){
												std::vector<boost::uuids::uuid> l(notify.playerIDList_);
												c.comperePlayerList(l);
												//std::cout << "get player Data size:" << l.size() << std::endl;
												BOOST_FOREACH(const auto& id, l){


													this->request(client::request::GetPlayerData{id},
															[&](const server::reply::ReplyData& reply, client::ClientData& cData){

														//std::cout << boost::format("replyData:%1%") % reply.data_.which() << std::endl;
														assert(reply.data_.which() == 4); //ReplyPlayerData

														const data::Player& newPlayer =
																boost::get<server::reply::ReplyPlayerData>( reply.data_).player_;

														cData.addPlayer(newPlayer);

															});
												}

											});

											//�`���b�g�X�V
											boost::posix_time::ptime l;
											m_data.access([&l](client::ClientData& c)
													{
												l = c.lastChatTime_;
													});

											if (l<notify.lastChat_)
											{
												this->request(client::request::GetChatLog{l},
														[&](const server::reply::ReplyData& reply, client::ClientData& d) {

													const server::reply::ChatLog& log = boost::get<server::reply::ChatLog>(reply.data_);

													//�S����������
													boost::for_each(log.chat_, [&](const data::ChatData& c){d.addChatData(c);} );
												});
											}


										} else { assert(false); }

										this->setReaderHandle();
									}
					//)
					);

				}

				//! �\�P�b�g�����
				void do_close()
				{
					m_connection.socket().close();
				}

				//LockBuffer<DataType> m_inBuffer;
			};

			//���V�[�u�����p�r�W�^�[
		struct ClientVisiter : boost::static_visitor<>
		{
			typedef NetworkClientBase<ClientVisiter> client_type;
			boost::uuids::uuid id_;
			client_type& client_;
			//const DataType& data_;
			ClientVisiter() = delete;
			ClientVisiter(const boost::uuids::uuid& i, client_type& c) :
			id_(i),
			client_(c)
			{}

			template<typename T>
			void operator()(T& t) const
			{
#ifdef COUT_DEBUG
				std::cout << "###Client Reply Recive." << std::endl;
#endif
			}

			void operator()(server::reply::PingReply& d) const
			{
#ifdef COUT_DEBUG
				std::cout << d.clientSendTime_ << std::endl;
#endif
			}

			void
			operator()(server::reply::ChatLog& l) const
			{
#ifdef COUT_DEBUG
				std::cout << "\n###Client Reply Chatlog." << std::endl;
#endif
				//client_.AccessChatData([l](typename client_type::chatData_type& d){ d = l.chat_;});
			}

		};

		typedef NetworkClientBase<ClientVisiter> NetworkClient;
	}
}

#endif //AKASHA_CLIENET__H
