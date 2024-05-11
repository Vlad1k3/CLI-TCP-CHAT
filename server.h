#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include <ctime>

using namespace boost::asio;

io_service service;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(ip::tcp::socket socket, std::deque<std::shared_ptr<Session>>& sessions)
        : socket(std::move(socket)), sessions_(sessions) {}

    void start() {
        read();
    }

    

private:
    void read() {
        
        auto self(shared_from_this());
        async_read_until(socket, read_msg, '\n',
            [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    std::string msg(boost::asio::buffers_begin(read_msg.data()),
                                    boost::asio::buffers_begin(read_msg.data()) + bytes_transferred);
                    read_msg.consume(bytes_transferred);

                 
                    std::string::size_type pos = msg.find(':');
                    if (pos != std::string::npos) {
                        std::string username = msg.substr(0, pos);
                        std::string text = msg.substr(pos + 2); 

                        for (auto& session : sessions_) {
                            if (session != shared_from_this()) {
                                session->write(username + ": " + text);
                            }
                        }
                        std::time_t now = std::time(nullptr);

                        std::cout << std::asctime(std::localtime(&now)) << username << ": " << text << '\n';  
                    }

                    read();
                } else {
               
                }
            });
    }

    void write(const std::string& msg) {
        service.post([this, msg]() {
            bool write_in_progress = !write_msgs.empty();
            write_msgs.push_back(msg);
            if (!write_in_progress) {
                do_write();
            }
        });
    }

    void do_write() {
        auto self(shared_from_this());
        async_write(socket, buffer(write_msgs.front()),
            [this, self](const boost::system::error_code& error, std::size_t /*length*/) {
                if (!error) {
                    write_msgs.pop_front();
                    if (!write_msgs.empty()) {
                        do_write();
                    }
                } else {
              
                }
            });
    }

    ip::tcp::socket socket;
    std::deque<std::shared_ptr<Session>>& sessions_;
    std::deque<std::string> write_msgs;
    streambuf read_msg;
};

class Server {
public:
    Server(short port) : acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port)) {
        start_accept();
        std::cout << "Server booted" << std::endl;
    }


private:
    ip::tcp::acceptor acceptor;
    std::deque<std::shared_ptr<Session>> sessions_;

    void start_accept() {
        acceptor.async_accept([this](const boost::system::error_code& error, ip::tcp::socket socket) {
            if (!error) {
                std::cout << "New user connected\n" << "\n";  
                auto new_session = std::make_shared<Session>(std::move(socket), sessions_);
                sessions_.push_back(new_session);
                new_session->start();
            }
            start_accept();
        });
    }
};
#endif
