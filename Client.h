//
// Created by Sam on 22/apr/2020.
//
#include <iostream>
#include <string>
#include <thread>
#include <deque>
#include <QDataStream>
#include <QTimer>
#include <memory>
#include <boost/asio.hpp>
#include "json.hpp"
#include "message.h"

#ifndef PROGETTO_PROVA_CLIENT_H
#define PROGETTO_PROVA_CLIENT_H


using json = nlohmann::json;
using boost::asio::ip::tcp;
typedef std::deque<message> message_queue;

class Client : public QObject{
    Q_OBJECT
public:
    Client();
    void write(const message& msg);
    void do_write(message mess);
    void close();
    std::string do_header();
    std::string do_body();
    std::string handleRequestType(const json &json, const std::string &type_request);
    signals:
            void formResultSuccess(QString result);
    void insertSymbol(std::pair<int, QChar> corpo);
private:
    void do_connect();
    void do_read_header();
    void do_read_body();
    void do_write();

    boost::asio::io_context io_context_;
    std::shared_ptr<boost::asio::io_context::work> work_;
    tcp::resolver resolver_;
    std::thread worker_;
    tcp::socket socket_;
    message read_msg_;
    message_queue write_msgs_;
};

#endif //PROGETTO_PROVA_CLIENT_H
