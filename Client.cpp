//
// Created by Sam on 22/apr/2020.
//

#include "Client.h"
Client::Client()
        : work_(new boost::asio::io_context::work(io_context_)),
          resolver_(io_context_),
          socket_(io_context_){
    worker_= std::thread([&](){
        io_context_.run(); //boost thread loop start
    });
    do_connect();
}
void Client::do_connect()
{
    auto endpoints = resolver_.resolve("127.0.0.1", "3000");
    boost::asio::async_connect(socket_, endpoints,[this](boost::system::error_code ec, tcp::endpoint)
    {
        if (!ec)
        {
            std::cout<<"CONNESSO AL SERVER"<<std::endl;

            do_read_header();
        } else
            std::cout<<ec.message()<<std::endl;
    });
}
void Client::write(const message& msg)
{
    boost::asio::post(io_context_,[this, msg]()
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            do_write();
        }
    });
}

void Client::close()
{
    boost::asio::post(io_context_, [this]() { socket_.close(); });
}

void Client::do_read_header()
{
    memset(read_msg_.data(), 0, read_msg_.length());
    boost::asio::async_read(socket_, boost::asio::buffer(read_msg_.data(), message::header_length),[this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec && read_msg_.decode_header())
        {
            do_read_body();
        }
        else
        {
            socket_.close();
        }
    });
}

void Client::do_read_body()
{
    boost::asio::async_read(socket_,boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),[this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            std::cout << "Messaggio ricevuto dal server: " << read_msg_.body() << std::endl;
            json messageFromClient = json::parse(read_msg_.body());
            std::string requestType = messageFromClient.at("response").get<std::string>();
            std::string response = handleRequestType(messageFromClient, requestType);

            do_read_header();
        }
        else
        {
            socket_.close();
        }
    });
}
std::string Client::handleRequestType(const json &js, const std::string &type_request) {
    if(type_request=="LOGIN_SUCCESS" || type_request=="SIGNUP_SUCCESS"|| type_request=="LOGIN_ERROR" || type_request=="QUERY_ERROR" || type_request=="CONNESSION_ERROR"){
        QString res = QString::fromStdString(type_request);
        if(type_request=="LOGIN_SUCCESS") {
            std::string name = js.at("username").get<std::string>();
            std::string color = js.at("colorUser").get<std::string>();
            QString Qname = QString::fromUtf8(name.data(), name.size());
            QString Qcolor = QString::fromUtf8(color.data(), color.size());

            this->setColor(Qcolor);
            this->setUser(Qname);
        }
        emit formResultSuccess(res);
        return type_request;
       }else if(type_request=="insert_res"){
        std::pair<int, char> corpo;
        corpo = js.at("corpo").get<std::pair<int, char>>();
        std::pair<int, QChar> corpo2(corpo.first, static_cast<QChar>(corpo.second));
        //non funziona
        emit insertSymbol(corpo.first,static_cast<QChar>(corpo.second));
        //emit showSymbol(corpo);
        return type_request;
    }else if(type_request=="remove_res"){
        int start, end;
        start = js.at("start").get<int>();
        end = js.at("end").get<int>();
        emit eraseSymbols(start,end);
        return type_request;
    }
    return type_request;
}
void Client::do_write(message write_msgs_)
{
    boost::asio::async_write(socket_,boost::asio::buffer(write_msgs_.data(),write_msgs_.length()),[this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {

        }
        else
        {
            std::cout << ec.message();
            socket_.close();
        }
    });
}
void Client::do_write()
{
    boost::asio::async_write(socket_,boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()),[this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                do_write();
            }
        }
        else
        {
            socket_.close();
        }
    });
}

const QString &Client::getUser() const {
    return user;
}

void Client::setUser(const QString &user) {
    Client::user = user;
}

const QString &Client::getColor() const {
    return color;
}

void Client::setColor(const QString &color) {
    Client::color = color;
}
