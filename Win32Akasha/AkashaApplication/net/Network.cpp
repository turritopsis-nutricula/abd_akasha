#include "Network.hpp"

#include <boost/make_shared.hpp>

struct ForeplayServiceDelete
{
	void operator()( foreplay::ForeplayService* s) const
	{
		s->close();
		delete s;
	}
};

namespace akasha
{
namespace net
{
Network::Network()
{
	m_service = boost::shared_ptr<foreplay::ForeplayService>(
			new foreplay::ForeplayService(),
			ForeplayServiceDelete());
}
void Network::runServer(ConnectionInfo const& info, std::string const& port)
{
	m_frontend->setInfo( info );
	auto frontend = m_frontend;
	m_client = m_service->openServer( port ,
			[ frontend ]( foreplay::ForeplayClient& client,foreplay::PlayerUpdate const& u){
			frontend->onNetworkConnect( client,u );
			frontend->setStatus(true);
			});
}

void Network::runClient(ConnectionInfo const& info,
		std::string const& address, std::string const& port)
{
	m_frontend->setInfo( info );
	auto frontend = m_frontend;
	m_client = m_service->connectClient( address, port,
			[ frontend ]( foreplay::ForeplayClient& client,
				foreplay::PlayerUpdate const& u){
			frontend->onNetworkConnect( client, u );
			frontend->setStatus(true);
			});
}

void Network::update()
{
	//auto mineUuid =  m_client->getSessionUuid();
	if ( !m_client /*|| !mineUuid*/) return;
	m_frontend->update( *m_client );

}
void Network::close()
{
	m_service->close();
	m_frontend->setStatus( false );
	m_client.reset();
}
}
}

