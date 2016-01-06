#ifndef NETWORK_HPP_INCLUDED
#define NETWORK_HPP_INCLUDED

#include "FrontEnd.hpp"
#include <foreplay/foreplay.hpp>

#include <boost/function.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/shared_ptr.hpp>


namespace akasha
{
namespace net
{
class Network
{
	struct PlayerDataKey
	{
		boost::uuids::uuid name_;
		boost::uuids::uuid chatList_;
	};

	boost::shared_ptr<foreplay::ForeplayService> m_service;
	boost::shared_ptr<foreplay::ForeplayClient> m_client;
	boost::shared_ptr<NetworkFrontEnd> m_frontend;


public:
	Network();

	void setFrontEnd( boost::shared_ptr<NetworkFrontEnd> f)
	{
		m_frontend = f;
	}
	void runServer(ConnectionInfo const& info, std::string const& port);
	void runClient(ConnectionInfo const& info, std::string const& ip,std::string const& port);
	/*
		bool
		isActive() const{ return m_service; }
		bool
		isServer() const{ if (m_service) return m_service->isServerActive(); else return false;}
		*/
	//          std::vector<PlayerData> const&
	//          getPlayerList() const;


	void update();

	void close();

};
}
}

#endif // NETWORK_HPP_INCLUDED
