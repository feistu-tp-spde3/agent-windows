#include "UdpListener.hpp"
#include "Configuration.hpp"
#include "Windivert.hpp"
#include "PacketSniffer.hpp"
#include "LinuxPacketSniffer.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


int __cdecl main(int argc, char **argv)
{
	Configuration config("config_agent.xml");
    boost::shared_ptr<PacketSniffer> sniffer;
#ifdef _WIN32
    sniffer = boost::make_shared<Windivert>();
#elif __linux__
    sniffer = boost::make_shared<LinuxPacketSniffer>();
#else
#error "Operating system not supported"
#endif
	UdpListener listener;
	listener.run(config);

	sniffer->init(config);
	sniffer->run(config);
      
	return 0;
}