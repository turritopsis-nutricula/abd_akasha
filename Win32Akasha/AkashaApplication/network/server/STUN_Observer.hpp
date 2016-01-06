/*
 * STUN_Observer.hpp
 *
 *  Created on: 2012/05/26
 *      Author: ely
 */

#ifndef STUN_OBSERVER_HPP_
#define STUN_OBSERVER_HPP_

#include <boost/asio.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at_c.hpp>

namespace akasha
{
namespace network
{
struct STUN_Packet
{
	boost::uuids::uuid HostID_;

};
namespace server
{



//! STUNÇäƒéãÇµÇƒèÓïÒÇèWêœÇ∑ÇÈ
class STUN_Observer
{

	boost::asio::ip::udp::socket m_socket;

	typedef boost::fusion::vector<boost::asio::ip::udp::endpoint, STUN_Packet> reciveDataType;
public:
	STUN_Observer() = delete;
	STUN_Observer(boost::asio::io_service& io, unsigned int portNumber) :
		m_socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), portNumber) )
	{

	}

private:

	void
		setRecive()
		{
			auto d = boost::make_shared< reciveDataType >();
			m_socket.async_receive_from(
					boost::asio::buffer(boost::fusion::at_c<1>(*d)),
					boost::fusion::at_c<0>(*d),
					boost::bind(&STUN_Observer::on_recive, this, boost::asio::placeholders::error, d)
					);
		}

	void
		on_recive(boost::system::error_code const& e, boost::shared_ptr<reciveDataType> data)
		{
			if (!e)
			{
				boost::asio::ip::udp::endpoint const& senderPoint = boost::fusion::at_c<1>(*data);
				STUN_Packet const& STUN_data = boost::fusion::at_c<0>(*data);

				//
				setRecive();
			}
			else
			{
				std::cout << "STUNObs ReciveError." <<  e.value() << e.message() << std::endl;
			}
		}


};
}
}
}

#endif /* STUN_OBSERVER_HPP_ */
