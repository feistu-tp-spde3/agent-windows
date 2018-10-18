#include "UdpListener.hpp"
#include "TCPConnection.hpp"

void UdpListener::run(Configuration &config)
{
	mThread = std::thread([&config]() 
    {
		boost::asio::io_service io_service;
        boost::system::error_code error;
        const int udpDefaultPort = 8888;
        boost::asio::ip::udp::socket udpSocket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), udpDefaultPort));

		udpSocket.set_option(boost::asio::socket_base::broadcast(true));
        boost::asio::ip::udp::endpoint senderEndpoint;
		int senderPort = -1;
        const int bufferSize = 1000;
        
		char buffer[bufferSize];
		std::size_t bytesReceived = 0;
		bool messageReceived = false;

		while (!messageReceived)
		{
			try 
            {
				bytesReceived = udpSocket.receive_from(boost::asio::buffer(buffer), senderEndpoint);
			}
			catch (std::exception &e)
			{
				std::cout << "[UDPListener] Exception: " << e.what() << std::endl;
			}

			if (bytesReceived)
			{
				std::string message(buffer, bytesReceived);
				std::cout << "[UDPListener] Message received: " << message << std::endl;

				if (message.find("agentSearch", 0) != std::string::npos)
				{
					messageReceived = true;
					udpSocket.close();

					// vyparsovanie portu zo spravy
					senderPort = atoi(message.substr(message.find("/", 0) + 1, message.size() - message.find("/", 0)).c_str());
				}
			}
		}

		if (messageReceived && senderPort != -1)
		{
			tcpConnection = std::make_unique<TCPConnection>();
			tcpConnection->establishConnection(senderEndpoint.address(), senderPort, config);
		}
	});
}

void UdpListener::join()
{
	mThread.join();
}