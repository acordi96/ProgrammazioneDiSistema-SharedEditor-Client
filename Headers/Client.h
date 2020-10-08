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
#include "Libs/json.hpp"
#include "message.h"
#include "Symbol.h"
#include "MessageSymbol.h"
#include <QColor>

#ifndef PROGETTO_PROVA_CLIENT_H
#define PROGETTO_PROVA_CLIENT_H


using json = nlohmann::json;
using boost::asio::ip::tcp;
typedef std::deque<message> message_queue;
typedef std::map<std::string, std::string> usersInFile;

Q_DECLARE_METATYPE(usersInFile)
class Client : public QObject {
Q_OBJECT
public:
    Client();

    void write(const message &msg);

    void do_write(message mess);

    void close();

    std::string do_header();

    std::string do_body();

    std::string handleRequestType(const json &json, const std::string &type_request);

    const QString &getUser() const;

    void setUser(const QString &user);

    const QString &getColor() const;

    void setColor(const QString &color);

    void setFiles(const std::vector<std::string> &owners, const std::vector<std::string> &filenames,
                  const std::vector<std::string> &invitations);

    std::map<std::pair<std::string, std::string>, std::string> files; //<<owner, filename>, invitation>
    std::vector<Symbol> symbols;

    QString getFileName() const;

    void setFileName(const QString &value);

    QString getEmail() const;

    void setEmail(const QString &value);

    std::vector<int> insertSymbolNewCRDT(int index, char character, const std::string &username);

    int generateIndexCRDT(Symbol symbol, int iter, int start, int end);

    std::pair<int, int> eraseSymbolCRDT(Symbol symbolStart, Symbol symbolEnd);

    void insertSymbolIndex(const Symbol &symbol, int index);

    std::vector<int> generatePos(int index);

    std::vector<int> generatePosBetween(std::vector<int> pos1, std::vector<int> pos2, std::vector<int> newPos);

    void sendAtServer(const json &js);

signals:

    void formResultSuccess(QString result);

    void updateFile(QString old, QString newN, QString owner, QString request);

    void insertSymbol(int pos, QChar c);

    void insertSymbolWithId(QString username, int pos, QChar c);

    void eraseSymbols(QString user, int startIndex, int endIndex);

    void showSymbol(std::pair<int, char> tuple);

    void updateUserslist(usersInFile users);

    void clearEditor();

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
    QString email;
    QString color;
    QString fileName;
    int writing = 0;
};

#endif //PROGETTO_PROVA_CLIENT_H
