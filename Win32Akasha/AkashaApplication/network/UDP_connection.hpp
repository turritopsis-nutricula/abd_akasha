/*
 * UDP_connection.hpp
 *
 *  Created on: 2012/05/26
 *      Author: ely
 */

#ifndef UDP_CONNECTION_HPP_
#define UDP_CONNECTION_HPP_

#include "connection.hpp" //ReadBuffer

#include <boost/asio.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace akasha
{
namespace network
{
class UDP_connection
{

	boost::asio::ip::udp::socket m_socket;
	enum {header_length = 8};
	typedef detail::ReadBuffer< header_length> readBufferType;
public:
	UDP_connection(boost::asio::io_service& io) : m_socket(io){ }

	boost::asio::ip::udp::socket& socket(){
		return m_socket;
	}
	const boost::asio::ip::udp::socket& socket() const
	{
		return m_socket;
	}

	template<typename T, typename Handler>
		void
		async_read_from(T& t, boost::asio::ip::udp::endpoint& sendPoint, Handler handler)
		{
			void (UDP_connection::*f)(const boost::system::error_code&,boost::asio::ip::udp::endpoint& sendPoint,T&,boost::shared_ptr<readBufferType>,boost::tuple<Handler>) =
				&UDP_connection::on_header_read<T,Handler>;

			auto buf = boost::make_shared<readBufferType>();
			m_socket.async_receive_from(
					boost::asio::buffer(buf->Header_),
					sendPoint,
					boost::bind(f, this, boost::asio::placeholders::error,
						boost::ref(t),
						boost::ref(sendPoint),
						buf,
						boost::make_tuple(handler)
						));
		}

private:
	//ヘッダー読み取り
	template<typename T, typename HandlerType>
		void
		on_header_read(
				const boost::system::error_code& e,
				boost::asio::ip::udp::endpoint& sendPoint,
				T& t,
				boost::shared_ptr<readBufferType> buffer,
				boost::tuple<HandlerType> handler)
		{

			if (e)
			{
				boost::get<0>(handler)(e);
				return;
			}

			//ヘッダー取得
			std::istringstream is(std::string(buffer->Header_, header_length));
			std::size_t inDataSize = 0;
			if (!(is >> std::hex >> inDataSize))
			{
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			buffer->Data_.resize(inDataSize);

			void (UDP_connection::*f)(const boost::system::error_code&, boost::asio::ip::udp::endpoint& sendPoint,T&, boost::shared_ptr<readBufferType>, boost::tuple<HandlerType>) =
				&UDP_connection::on_data_read<T,HandlerType>;

			boost::asio::

		}
	template<typename T, typename HandlerType>
		void
		on_data_read(const boost::system::error_code& e, boost::asio::ip::udp::endpoint& sendPoint,T& t, boost::shared_ptr<readBufferType>, boost::tuple<HandlerType> h)
		{

		}
};
}
}

#endif /* UDP_CONNECTION_HPP_ */
