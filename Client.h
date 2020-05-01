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

    const QString &getUser() const;
    void setUser(const QString &user);

    const QString &getColor() const;
    void setColor(const QString &color);

signals:
    void formResultSuccess(QString result);
    void insertSymbol(int pos, QChar c);
    void eraseSymbols(int startIndex, int endIndex);
    void showSymbol(std::pair<int,char> tuple);
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


    //cose da salvare
    QString user;
    QString color;
};

#endif //PROGETTO_PROVA_CLIENT_H
