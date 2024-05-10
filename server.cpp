#include <iostream>
#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include "server.h"

using namespace boost::asio;


int main() {
    
    try {
        Server server(8080);
        service.run();
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';  
    }
    return 0;
}


