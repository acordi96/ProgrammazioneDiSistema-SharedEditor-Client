//
// Created by Sam on 22/apr/2020.
//

#define serverRoute "93.43.250.236"
//#define serverRoute "127.0.0.1"

#include <QtWidgets/QMessageBox>
#include "Client.h"

Client::Client()
        : work_(new boost::asio::io_context::work(io_context_)),
          resolver_(io_context_),
          socket_(io_context_) {
    worker_ = std::thread([&]() {
        io_context_.run(); //boost thread loop start
    });
    do_connect();
}

void Client::do_connect() {
    auto endpoints = resolver_.resolve(serverRoute, "3000");
    boost::asio::async_connect(socket_, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
        if (!ec) {
            std::cout << "CONNESSO AL SERVER" << std::endl;

            do_read_header();
        } else
            std::cout << ec.message() << std::endl;
    });
}

void Client::write(const message &msg) {
    boost::asio::post(io_context_, [this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}

void Client::close() {
    boost::asio::post(io_context_, [this]() { socket_.close(); });
}

void Client::do_read_header() {
    memset(read_msg_.data(), 0, read_msg_.length());
    boost::asio::async_read(socket_, boost::asio::buffer(read_msg_.data(), message::header_length),
                            [this](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec && read_msg_.decode_header()) {
                                    do_read_body();
                                } else {
                                    socket_.close();
                                }
                            });
}

void Client::do_read_body() {
    boost::asio::async_read(socket_, boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                            [this](boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec) {
                                    std::cout << "\n Messaggio ricevuto dal server: " << read_msg_.body() << std::endl;
                                    json messageFromClient;
                                    try {
                                        std::string message = read_msg_.body();
                                        if (message.find_last_of('}') < (message.size() + 1))
                                            message[message.find_last_of('}') + 1] = '\0';
                                        messageFromClient = json::parse(message);
                                    } catch (...) {
                                        std::cout << "parse error: " << messageFromClient << std::endl;
                                        do_read_header();
                                    }
                                    std::string requestType = messageFromClient.at("response").get<std::string>();
                                    std::cout << "\n requestType : " + requestType;
                                    std::string response;
                                    try {
                                        response = handleRequestType(messageFromClient, requestType);
                                    } catch (...) {
                                        std::cout << "GENERIC ERROR HandleRequest of: " << messageFromClient
                                                  << std::endl;
                                        do_read_header();
                                    }

                                    do_read_header();
                                } else {
                                    socket_.close();
                                }
                            });
}

std::string Client::handleRequestType(const json &js, const std::string &type_request) {
    if (type_request == "LOGIN_SUCCESS" || type_request == "SIGNUP_SUCCESS") {
        QString res = QString::fromStdString(type_request);
        std::string name = js.at("username").get<std::string>();
        QString Qname = QString::fromUtf8(name.data(), name.size());
        this->setUser(Qname);
        std::string color = js.at("colorUser").get<std::string>();
        QString Qcolor = QString::fromUtf8(color.data(), color.size());
        this->setColor(Qcolor);

        if (type_request == "LOGIN_SUCCESS") {
            std::list<std::string> files = js.at("files").get<std::list<std::string>>();
            std::cout << "User " << this->getUser().toStdString() << " loggato! File disponibili:" << std::endl;
            for (auto p : files) {
                std::cout << p << std::endl;
            }
            this->setFiles(files);

        }
        emit formResultSuccess(res);
        return type_request;
    } else if (type_request == "LOGIN_ERROR" || type_request == "QUERY_ERROR" || type_request == "CONNESSION_ERROR"
               || type_request == "SIGNUP_ERROR_DUPLICATE_USERNAME" || type_request == "SIGNUP_ERROR_INSERT_FAILED" ||
               type_request == "FILE_OPEN_FAILED") {
        QString res = QString::fromStdString(type_request);
        emit formResultSuccess(res);
    } else if (type_request == "insert_res") {
        std::pair<int, char> corpo;
        corpo = js.at("corpo").get<std::pair<int, char>>();
        int participantId;
        participantId = js.at("participant").get<int>();

        std::pair<int, QChar> corpo2(corpo.first, static_cast<QChar>(corpo.second));
        //non funziona
        emit insertSymbolWithId(participantId, corpo.first, static_cast<QChar>(corpo.second));
        //emit showSymbol(corpo);
        return type_request;
    } else if (type_request == "remove_res") {
        int start, end;
        start = js.at("start").get<int>();
        end = js.at("end").get<int>();
        emit eraseSymbols(start, end);
        return type_request;
    } else if (type_request == "new_file_created") {
        std::cout << "richiesta file tornata ";
        QString res = QString::fromStdString("new_file_created");
        emit formResultSuccess(res);
        std::string name = js.at("filename").get<std::string>() + "_|_" + this->getUser().toStdString() + js.at("invitation").get<std::string>();
        emit updateFile("", QString::fromStdString(name));
        return type_request;
    } else if (type_request == "new_file_already_exist") {
        std::cout << "Errore file gia esistente";
        QString res = QString::fromStdString(type_request);
        emit formResultSuccess(res);
        return type_request;

    } else if (type_request == "invalid_name") {
        std::cout << "Errore nome file invalido";
        //QString res = QString::fromStdString(type_request);
        //emit formResultSuccess(res);
        return type_request;
    } else if (type_request == "errore_salvataggio_file_db") {
        std::cout << "Errore caricamento file nel db";
        QString res = QString::fromStdString(type_request);
        emit formResultSuccess(res);
        return type_request;
    } else if (type_request == "file_opened") {
        //file aperto con successo
        std::cout << "file aperto correttamente ";
        QString res = QString::fromStdString("file_opened");
        emit formResultSuccess(res);
        emit clearEditor();
        std::string toWrite = js.at("toWrite");
        for (int i = 0; i < toWrite.length(); i++)
                emit insertSymbol(i, toWrite[i]);
        return type_request;

    } else if (type_request == "open_file") {
        int maxBuffer = js.at("maxBuffer");
        QString res = QString::fromStdString("file_opened");
        std::string toWrite = js.at("toWrite");
        int part = js.at("partToWrite");

        std::unique_lock<std::mutex> lk(this->m);
        while (this->writing != part) {
            this->cv.wait(lk);
        }
        emit clearEditor();
        for (int i = 0; i < toWrite.length(); i++) {
            emit insertSymbol((maxBuffer * part) + i, toWrite[i]);
        }
        if (this->writing == js.at("ofPartToWrite")) {
            this->writing = 0;
            std::cout << "file aperto correttamente ";
            emit formResultSuccess(res);
        } else {
            std::cout << "written " << part << " json" << std::endl;
            this->writing++;
        }
        this->cv.notify_one();

        return type_request;
    } else if (type_request == "invitation_success") {
        std::string name = js.at("owner").get<std::string>() + "_|_" + js.at("filename").get<std::string>()+"///////////////";
        emit updateFile("", QString::fromStdString(name));
        return type_request;
    } else if (type_request == "FILE_RENAMED") {
        //file rinominato correttamente
        //aggiorno nome file nella lista dei file
        std::cout << "\n nuovo nome: " << js.at("newName").get<std::string>();
        /*QMutableListIterator<std::string> iter1(files);
        while(iter1.hasNext() ){
            std::string s = iter1.next();
            std::cout <<"\nentrato nel while dei file ";
            if (s==js.at("oldName").get<std::string>()){
                iter1.setValue(js.at("newName").get<std::string>());
                break;
            }
        }*/
        QString res = QString::fromStdString("file_renamed");
        emit formResultSuccess(res);
        // chiamare funzione che aggiorna interfaccia grafica di userPage, con nuovo nome file
        emit updateFile(QString::fromStdString(js.at("oldName").get<std::string>()),QString::fromStdString(js.at("newName").get<std::string>()));
    }else if(type_request == "NEW_NAME_ALREADY_EXIST"){
        std::cout <<"\n il nuovo nome esiste già";
        QString res = QString::fromStdString("new_name_already_exist");
        emit formResultSuccess(res);

    }else if(type_request == "FILE_DELETED"){
        QString res = QString::fromStdString("file_deleted");

        std::multimap<std::string,std::pair<std::string, std::string>> :: iterator iter;
        int i = 0;

        for (iter = files.begin(); iter != files.end() && i == 0 ; iter++){
            //std::cout << "\n\n file:  " << iter->first << "  " << iter->second;
            if(iter->first == this->user.toStdString() && iter->second.first == js.at("name").get<std::string>()){
                // elimino il file
                files.erase(iter);
                i=1;
            }
        }

        emit formResultSuccess(res);
        QString name = QString::fromStdString(js.at("name").get<std::string>());
        QString del = "";
        emit updateFile(name,del);
    }else if(type_request == "ERRORE_ELIMINAZIONE_FILE"){
        QString res = QString::fromStdString("error_file_deleted");
        emit formResultSuccess(res);
    }else if(type_request == "FILE_IN_USE"){
        QString res = QString::fromStdString("error_file_in_use");
        emit formResultSuccess(res);
    }else if(type_request == "FILE_IN_USE_D"){
        QString res = QString::fromStdString("error_file_in_use_d");
        emit formResultSuccess(res);
    }else if(type_request == "update_participants"){
        std::vector<int> othersOnFile;
        std::vector<std::string> colors;
        std::vector<std::string> usernames;
        othersOnFile = js.at("idList").get<std::vector<int>>();
        colors = js.at("colorsList").get<std::vector<std::string>>();
        usernames = js.at("usernames").get<std::vector<std::string>>();
        int j = 0;
        for(auto u:usernames) {
            QString username = QString::fromStdString(u);
            emit insertParticipant(othersOnFile[j], username);
            j++;
        }

        int i = 0;
        for(auto c:colors) {
            QString color = QString::fromStdString(c);
            emit updateCursorParticipant(othersOnFile[i], color);
            i++;
        }

    }
    return type_request;
}

void Client::do_write(message write_msgs_) {
    boost::asio::async_write(socket_, boost::asio::buffer(write_msgs_.data(), write_msgs_.length()),
                             [this](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {

                                 } else {
                                     std::cout << ec.message();
                                     socket_.close();
                                 }
                             });
}

void Client::do_write() {
    boost::asio::async_write(socket_, boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
                             [this](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty()) {
                                         do_write();
                                     }
                                 } else {
                                     socket_.close();
                                 }
                             });
}

QString Client::getFileName() const {
    return fileName;
}

void Client::setFileName(const QString &value) {
    fileName = value;
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

void Client::setFiles(const std::list<std::string> &list) {
    std::string user;
    std::string fileName;
    std::string invitation;
    for (auto p : list) {
        //std::string token = s.substr(0, s.find(delimiter));
        user = p.substr(0, p.find("_"));
        invitation = p.substr(p.size() - 15, p.size());
        p.erase(p.size() - 15);
        fileName = p.substr(p.find("_") + 1, p.size());

        files.insert({user, std::pair<std::string, std::string>(fileName, invitation)});
    }
    //Client::files = list;
}

/*void Client::setFiles(const QList<std::string> &list) {
    Client::files = list;
}*/
