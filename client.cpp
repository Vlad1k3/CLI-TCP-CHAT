#include "client.h"

io_service service;

using namespace boost::asio;


ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8080);
streambuf buf;

void read(ip::tcp::socket& socket) {
    async_read_until(socket, buf, '\n',
        [&socket](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string msg(boost::asio::buffer_cast<const char*>(buf.data()), bytes_transferred);
                std::cout << msg << std::endl;
                buf.consume(bytes_transferred);
                read(socket); 
            } else {
                std::cerr << "Ошибка чтения: " << error.message() << std::endl;
            }
        });
}

int main() {
    try {
        std::string username;
        std::cout << "Введите ваше имя: ";
        std::getline(std::cin, username);

        ip::tcp::socket socket(service);
        socket.connect(ep);

        std::cout << "Подключение к серверу." << std::endl;

        write(socket, buffer(username + "\n"));

        read(socket);

        while (true) {
            std::string input;
            std::cout << "Введите сообщение: ";
            std::getline(std::cin, input);
            input += "\n";

            std::string message = username + ": " + input;

            write(socket, buffer(message));

            if (input == "exit\n") {
                std::cout << "Закрытие соединения." << std::endl;
                break;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    return 0;
}
