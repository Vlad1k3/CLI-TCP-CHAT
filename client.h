#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;

extern io_service service; 

void do_read(ip::tcp::socket& socket);

#endif 