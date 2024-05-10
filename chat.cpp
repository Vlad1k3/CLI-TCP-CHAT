#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

void read(ip::tcp::socket& socket, streambuf& buf) {
    async_read_until(socket, buf, '\n',
        [&socket, &buf](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string msg(boost::asio::buffer_cast<const char*>(buf.data()), bytes_transferred);
                std::cout << msg << std::endl;
                buf.consume(bytes_transferred);
                read(socket, buf); 
            } else {
                std::cerr << "Ошибка чтения: " << error.message() << std::endl;
            }
        });
}

int main() {
    try {
        io_service service;
        ip::tcp::socket socket(service);

        socket.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8080));

        streambuf buf;
        read(socket, buf);
        

        service.run(); 

    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    return 0;
}
