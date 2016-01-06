
#ifndef AKASHA_NETWORK_CONNECT__H
#define AKASHA_NETWORK_CONNECT__H

#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

namespace akasha {
namespace network{

namespace detail
{
template<int HeaderSize>
	struct ReadBuffer
{
	char Header_[ HeaderSize ];
	std::vector<char> Data_;
};
}
class TCP_connection
{
private:
	boost::asio::ip::tcp::socket m_socket;
	enum {header_length = 8};
	/*std::string m_outHeader;
	  std::string m_outData;*/
	/*char m_inHeader[ header_length ];
	  std::vector<char> m_inData;*/
	typedef detail::ReadBuffer<header_length> ReadBufferType;

public:
	TCP_connection(boost::asio::io_service& io_service) :
		m_socket(io_service)  {  }

	//! ソケット参照
	boost::asio::ip::tcp::socket& socket(){
		return m_socket;
	}
	const boost::asio::ip::tcp::socket& socket() const{
		return m_socket;
	}

	//! シリアライズ可能データを非同期送信
	template <typename T, typename Handler>
		void async_write(const T& t, Handler handler)
		{
			//データシリアライズ
			std::ostringstream archive_stream;
			boost::archive::text_oarchive archive(archive_stream);
			archive << t;
			//m_outData = archive_stream.str();
			std::string outData = archive_stream.str();

			//ヘッダー作成
			std::ostringstream header_stream;
			header_stream << std::setw(header_length) <<
				//std::hex << m_outData.size();
				std::hex << outData.size();
			if (!header_stream || header_stream.str().size() != header_length )
			{
				//ヘッダー作成失敗
				boost::system::error_code error(boost::asio::error::invalid_argument);
				// m_socket.io_service().post(boost::bind(handler,error));
				m_socket.get_io_service().post(boost::bind(handler,error));
			}
			//m_outHeader = header_stream.str();
			std::string outHeader = header_stream.str();

			//シリアライズデータをソケットへ書き込み
			//ヘッダとデータ本体を同時に描き込む
			std::vector<boost::asio::const_buffer> buffers;
			buffers.push_back(boost::asio::buffer(outHeader));
			buffers.push_back(boost::asio::buffer(outData));
			boost::asio::async_write(m_socket, buffers, handler);
		}

	//! シリアライズデータを非同期受信
	template <typename T, typename Handler>
		void async_read(T& t, Handler handler)
		{
			//ヘッダーサイズ分の読み取り操作
			void (TCP_connection::*f)(
					const boost::system::error_code&,
					T&,
					boost::shared_ptr<ReadBufferType>,
					boost::tuple<Handler>)
				= &TCP_connection::handler_read_header<T, Handler>;

			boost::shared_ptr<ReadBufferType > buf(new ReadBufferType() );
			boost::asio::async_read(
					m_socket,
					//boost::asio::buffer(m_inHeader),
					boost::asio::buffer(buf->Header_),
					boost::bind(
						f,
						this,
						boost::asio::placeholders::error,
						boost::ref(t),
						buf,
						boost::make_tuple(handler))
					);
		}

	//! ヘッダー読み取りハンドラ
	template <typename T, typename Handler>
		void handler_read_header(
				const boost::system::error_code& e,
				T& t,
				boost::shared_ptr< ReadBufferType > buffer,
				boost::tuple<Handler> handler)
		{
			if (e)
			{
				boost::get<0>(handler)(e);
			}
			else
			{
				//シリアライズデータ長にヘッダ整形
				//std::istringstream is(std::string(m_inHeader, header_length));
				std::istringstream is(std::string(buffer->Header_, header_length));
				std::size_t inDataSize = 0;
				if (!(is >> std::hex >> inDataSize))
				{
					// ヘッダー読み取り失敗
					boost::system::error_code error(boost::asio::error::invalid_argument);
					boost::get<0>(handler)(error);
					return;
				}

				//データ本体読み込みハンドラセット
				//m_inData.resize(inDataSize);
				buffer->Data_.resize(inDataSize);

				void (TCP_connection::*f)(
						const boost::system::error_code&,
						T&,
						boost::shared_ptr<ReadBufferType>,
						boost::tuple<Handler>) =
					&TCP_connection::handler_read_data<T, Handler>;
				boost::asio::async_read(
						m_socket,
						boost::asio::buffer(buffer->Data_),
						boost::bind(
							f,
							this,
							boost::asio::placeholders::error,
							boost::ref(t),
							buffer,
							handler));
			}
		}

	//! データ読み込みハンドラ
	template <typename T, typename Handler>
		void handler_read_data(
				const boost::system::error_code& e,
				T& t,
				boost::shared_ptr<ReadBufferType> buffer,
				boost::tuple<Handler> handler
				)
		{
			if (e)
			{
				boost::get<0>(handler)(e);
			}
			else
			{
				//データをデシリアライズ
				try
				{
					//std::string archive_data(&m_inData[0], m_inData.size());
					std::string archive_data(&(buffer->Data_[0]), buffer->Data_.size());
					std::istringstream archive_stream(archive_data);
					boost::archive::text_iarchive archive(archive_stream);
					archive >> t;
				}
				catch (std::exception&)
				{
					//データデシリアライズ失敗
					boost::system::error_code err(boost::asio::error::invalid_argument);
					boost::get<0>(handler)(err);
					return;
				}
				//データ読み取り完了
				boost::get<0>(handler)(e);
			}
		}


};
typedef boost::shared_ptr<TCP_connection> connection_ptr;

}
}

#endif //AKASHA_NETWORK_CONNECT__H
