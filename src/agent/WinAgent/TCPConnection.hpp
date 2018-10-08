#pragma once

#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <iostream>

#include "Configuration.hpp"

class TCPConnection;
extern std::unique_ptr<TCPConnection> tcpConnection;

class TCPConnection {
private:
	// socket na komunikaciu
	std::shared_ptr<boost::asio::ip::tcp::socket> mSocket;

	std::shared_ptr<boost::asio::io_service> io_service;

	// vlakno v ktorom sa budu spracuvat spravy
	std::thread mThread;
public:
	void establishConnection(const boost::asio::ip::address &address, int port, Configuration &config);

	void receiveMessage();
};