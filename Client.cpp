//
// Created by Sam on 22/apr/2020.
//

//#define serverRoute "93.43.250.236"
#define serverRoute "127.0.0.1"

#define serverPort "3000"

#include <QtWidgets/QMessageBox>
#include "Headers/Client.h"

Client::Client()
        : work_(new boost::asio::io_context::work(io_context_)),
          resolver_(io_context_),
          socket_(io_context_) {
    worker_ = std::thread([&]() {
        io_context_.run(); //boost thread loop start
    });
    this->maxBufferSymbol = 1;
    this->updateChangesCursor = true;
    do_connect();
}

void Client::do_connect() {
    auto endpoints = resolver_.resolve(serverRoute, serverPort);
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
                                    std::cout << "Messaggio ricevuto dal server: " << read_msg_.body() << std::endl;
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
                                    std::string response;
                                    //try {
                                    response = handleRequestType(messageFromClient, requestType);
                                    /*} catch (...) {
                                        std::cout << "GENERIC ERROR HandleRequest of: " << messageFromClient
                                                  << std::endl;
                                        do_read_header();
                                    }*/

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
            std::vector<std::string> owners = js.at("owners").get<std::vector<std::string>>();
            std::vector<std::string> filenames = js.at("filenames").get<std::vector<std::string>>();
            std::vector<std::string> invitations = js.at("invitations").get<std::vector<std::string>>();

            this->setFiles(owners, filenames, invitations);

            std::string email = js.at("email").get<std::string>();
            QString qEmail = QString::fromUtf8(email.data(), email.size());
            this->setEmail(qEmail);
            this->maxBufferSymbol = js.at("maxBufferSymbol").get<int>();
        }
        emit formResultSuccess(res);
        return type_request;
    } else if (type_request == "LOGIN_ERROR" || type_request == "QUERY_ERROR" || type_request == "CONNESSION_ERROR"
               || type_request == "SIGNUP_ERROR_DUPLICATE_USERNAME" || type_request == "SIGNUP_ERROR_INSERT_FAILED" ||
               type_request == "FILE_OPEN_FAILED") {
        QString res = QString::fromStdString(type_request);
        emit formResultSuccess(res);
    } else if (type_request == "insert_res") {
        //prendo il vettore di symbol
        std::vector<std::string> usernameToInsert = js.at("usernameToInsert").get<std::vector<std::string>>();
        std::vector<wchar_t> charToInsert = js.at("charToInsert").get<std::vector<wchar_t>>();
        std::vector<std::vector<int>> crdtToInsert = js.at("crdtToInsert").get<std::vector<std::vector<int>>>();
        for (int i = 0; i < usernameToInsert.size(); i++) {
            Symbol symbolToInsert(charToInsert[i], usernameToInsert[i], crdtToInsert[i]);
            emit insertSymbolWithId(symbolToInsert);
        }
        return type_request;
    } else if (type_request == "insertAndStyle_res") {
        //prendo il vettore di symbol
        std::vector<std::string> usernameToInsert = js.at("usernameToInsert").get<std::vector<std::string>>();
        std::vector<wchar_t> charToInsert = js.at("charToInsert").get<std::vector<wchar_t>>();
        std::vector<std::vector<int>> crdtToInsert = js.at("crdtToInsert").get<std::vector<std::vector<int>>>();
        std::string fontFamily = js.at("fontFamily").get<std::string>();
        int fontSize = js.at("size").get<int>();
        bool bold = js.at("bold").get<bool>();
        bool italic = js.at("italic").get<bool>();
        bool underlined = js.at("underlined").get<bool>();
        std::string color = js.at("color").get<std::string>();
        //TO DO:anche qui un solo elemento
        for (int i = 0; i < usernameToInsert.size(); i++) {
            Style style = {bold, underlined, italic, fontFamily, fontSize, color};
            Symbol symbolToInsert(charToInsert[i], usernameToInsert[i], crdtToInsert[i], style);
            emit insertSymbolWithStyle(symbolToInsert, false);
        }
        return type_request;
    } else if (type_request == "remove_res") {
        std::vector<std::string> usernameToErase = js.at("usernameToErase").get<std::vector<std::string>>();
        std::vector<wchar_t> charToErase = js.at("charToErase").get<std::vector<wchar_t>>();
        std::vector<std::vector<int>> crdtToErase = js.at("crdtToErase").get<std::vector<std::vector<int>>>();
        std::vector<Symbol> symbolsToErase;
        for (int i = 0; i < usernameToErase.size(); i++) {
            symbolsToErase.emplace_back(charToErase[i], usernameToErase[i], crdtToErase[i]);
        }
        emit eraseSymbols(symbolsToErase);
        return type_request;
    } else if (type_request == "new_file_created") {
        QString res = QString::fromStdString("new_file_created");
        emit formResultSuccess(res);
        emit clearEditor();
        std::string name = std::to_string(this->getUser().size()) + '_' + this->getUser().toStdString() +
                           js.at("filename").get<std::string>() + js.at("invitation").get<std::string>();
        std::pair<std::string, std::string> p = std::make_pair<std::string, std::string>(this->getUser().toStdString(),
                                                                                         js.at("filename").get<std::string>());
        this->files.insert(
                std::pair<std::pair<std::string, std::string>, std::string>(p, js.at("invitation").get<std::string>()));

        emit updateFile("", QString::fromStdString(name), "", QString::fromStdString("add_new_file"));
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
    } else if (type_request == "open_file") {
        //prima parte di file deve pulire il testo
        if (this->writing == 0) {
            this->symbols.clear();
            emit clearEditor();
            if(!js.contains("endOpenFile"))
                emit showLoading();
        }
        std::vector<int> usernameToInsert = js.at("usernameToInsert").get<std::vector<int>>();
        std::vector<wchar_t> charToInsert = js.at("charToInsert").get<std::vector<wchar_t>>();
        std::vector<std::vector<int>> crdtToInsert;
        std::vector<int> forceCRDT;
        if(!js.contains("first_open"))
            crdtToInsert = js.at("crdtToInsert").get<std::vector<std::vector<int>>>();
        std::vector<std::string> idToUsername = js.at("usernameToId").get<std::vector<std::string>>();
        std::vector<int> bold;
        if (!js.contains("boldDefault"))
            bold = js.at("bold").get<std::vector<int>>();
        std::vector<int> italic;
        if (!js.contains("italicDefault"))
            italic = js.at("italic").get<std::vector<int>>();
        std::vector<int> underlined;
        if (!js.contains("underlinedDefault"))
            underlined = js.at("underlined").get<std::vector<int>>();
        std::vector<std::string> colorj;
        if (!js.contains("colorDefault"))
            colorj = js.at("color").get<std::vector<std::string>>();
        std::vector<std::string> fontFamily;
        if (!js.contains("fontFamilyDefault"))
            fontFamily = js.at("fontFamily").get<std::vector<std::string>>();
        std::vector<int> size;
        if (!js.contains("sizeDefault"))
            size = js.at("size").get<std::vector<int>>();

        std::map<int, std::string> usernameToId;
        for (int i = 0; i < idToUsername.size(); i++)
            usernameToId.insert({i, idToUsername[i]});
        for (int i = 0; i < usernameToInsert.size(); i++) {
            //ricreo il simbolo
            Style style(js.contains("boldDefault") ? false : bold[i] == 1,
                        js.contains("underlinedDefault") ? false : underlined[i] == 1,
                        js.contains("italicDefault") ? false : italic[i] == 1,
                        js.contains("fontFamilyDefault") ? DEFAULT_FONT_FAMILY : (fontFamily[i] == "0"
                                                                                  ? DEFAULT_FONT_FAMILY
                                                                                  : fontFamily[i]),
                        js.contains("sizeDefault") ? DEFAULT_FONT_SIZE : (size[i] == 0 ? DEFAULT_FONT_SIZE : size[i]),
                        js.contains("colorDefault") ? DEFAULT_COLOR : (colorj[i] == "0" ? DEFAULT_COLOR : colorj[i]));
            if(js.contains("first_open")) {
                forceCRDT.clear();
                forceCRDT.push_back((this->writing * this->maxBufferSymbol) + i);
            }
            Symbol symbolToInsert(charToInsert[i], usernameToId.at(usernameToInsert[i]), js.contains("first_open") ? forceCRDT : crdtToInsert[i], style);
            emit insertSymbolWithStyle(symbolToInsert, true);
        }
        if (js.contains("endOpenFile")) {
            this->writing = 0;
            emit closeLoading();
            emit formResultSuccess(QString::fromStdString("file_opened"));
        } else {
            this->writing++;
        }
        return type_request;
    } else if (type_request == "invitation_success") {
        std::string name =
                std::to_string(js.at("owner").get<std::string>().size()) + '_' + js.at("owner").get<std::string>() +
                js.at("filename").get<std::string>();
        files.insert({std::pair<std::string, std::string>(js.at("owner").get<std::string>(),
                                                          js.at("filename").get<std::string>()), "///////////////"});
        emit updateFile("", QString::fromStdString(name), "", QString::fromStdString("add_new_file_invitation"));
        return type_request;
    } else if (type_request == "invitation_error"){
        QString res = QString::fromStdString("invitation_error");
        emit formResultSuccess(res);
    } else if (type_request == "file_renamed") {
        QString res = QString::fromStdString("file_renamed");
        for (auto &iter : files) {
            if (iter.first.first == js.at("owner").get<std::string>() &&
                iter.first.second == js.at("oldName").get<std::string>()) {
                files.insert(
                        {std::pair<std::string, std::string>(iter.first.first, js.at("newName").get<std::string>()),
                         iter.second});
                files.erase(iter.first);
                break;
            }
        }
        if (js.at("owner").get<std::string>() == this->getUser().toStdString())
                emit formResultSuccess(res);
        // chiamare funzione che aggiorna interfaccia grafica di userPage, con nuovo nome file
        emit updateFile(QString::fromStdString(js.at("oldName").get<std::string>()),
                        QString::fromStdString(js.at("newName").get<std::string>()),
                        QString::fromStdString(js.at("owner").get<std::string>()),
                        QString::fromStdString("rename_file"));
    } else if (type_request == "NEW_NAME_ALREADY_EXIST") {
        QString res = QString::fromStdString("new_name_already_exist");
        emit formResultSuccess(res);

    } else if (type_request == "file_deleted") {
        QString res = QString::fromStdString("file_deleted");

        if (js.at("owner").get<std::string>() == this->getUser().toStdString())
                emit formResultSuccess(res);
        QString name = QString::fromStdString(js.at("name").get<std::string>());
        QString del = QString::fromStdString(js.at("owner").get<std::string>());
        for (auto &iter : files) {
            if (iter.first.first == js.at("owner").get<std::string>() &&
                iter.first.second == js.at("name").get<std::string>()) {
                files.erase(iter.first);
                break;
            }
        }
        emit updateFile(name, del, "", QString::fromStdString("delete_file"));
    } else if (type_request == "ERRORE_ELIMINAZIONE_FILE") {
        QString res = QString::fromStdString("error_file_deleted");
        emit formResultSuccess(res);
    } else if (type_request == "FILE_IN_USE") {
        QString res = QString::fromStdString("error_file_in_use");
        emit formResultSuccess(res);
    } else if (type_request == "FILE_IN_USE_D") {
        QString res = QString::fromStdString("error_file_in_use_d");
        emit formResultSuccess(res);
    } else if (type_request == "update_participants") {
        std::vector<std::string> colors;
        std::vector<std::string> usernames;
        colors = js.at("colorsList").get<std::vector<std::string>>();
        usernames = js.at("usernames").get<std::vector<std::string>>();
        usersInFile users;
        for (int i = 0; i < usernames.size(); i++) {
            for(auto pair = users.begin(); pair != users.end(); ++pair)
                if(pair->first == usernames[i]) {
                    users.erase(pair);
                    break;
                }
            users.insert(std::pair<std::string, std::string>(usernames[i], colors[i]));
        }
        emit updateUserslist(users);

    } else if (type_request == "user_already_logged") {
        QString res = QString::fromUtf8("user_already_logged");
        emit formResultSuccess(res);
    } else if (type_request == "update_cursorPosition") {
        std::string username = js.at("username").get<std::string>();
        int pos = js.at("pos").get<int>();
        emit updateRemotePosition(QString::fromStdString(username), pos);
    } else if (type_request == "WRONG_OLD_PASSWORD") {
        QString res = QString::fromUtf8("wrong_old_password");
        emit formResultSuccess(res);
    } else if (type_request == "EDIT_SUCCESS") {
        QString res = QString::fromUtf8("edit_success");
        QString newEmail = QString::fromStdString(js.at("email").get<std::string>());
        QString newColor = QString::fromStdString(js.at("color").get<std::string>());

        if (!newEmail.isEmpty()) this->setEmail(newEmail);
        if (!newColor.isEmpty()) this->setColor(newColor);

        emit formResultSuccess(res);
    } else if (type_request == "EDIT_FAILED") {

        QString res = QString::fromUtf8("edit_failed");
        emit formResultSuccess(res);
    } else if (type_request == "styleChanged_res") {
        emit changeStyle(js);
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

QString Client::getEmail() const {
    return email;
}

void Client::setEmail(const QString &value) {
    email = value;
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

void Client::setFiles(const std::map<std::pair<std::string, std::string>, std::string> &value) {
    files = value;
}

void Client::setFiles(const std::vector<std::string> &owners, const std::vector<std::string> &filenames,
                      const std::vector<std::string> &invitations) {

    for (int i = 0; i < owners.size(); i++) {
        files.insert({std::pair<std::string, std::string>(owners[i], filenames[i]), invitations[i]});
    }
}

std::map<std::pair<std::string, std::string>, std::string> Client::getFiles() const {
    return files;
}

//stessa cosa di sotto ma con style
std::vector<int> Client::insertSymbolNewCRDT(int index, wchar_t character, const std::string &username, Style style) {
    std::vector<int> vector;
    if (this->symbols.empty()) {
        vector = {0};
        index = 0;
    } else if (index > this->symbols.size() - 1) {
        vector = {this->symbols.back().getPosizione().at(0) + 1};
        index = this->symbols.size();
    } else if (index == 0) {
        vector = {this->symbols.front().getPosizione().at(0) - 1};
    } else
        vector = generatePos(index);
    Symbol s(character, username, vector, style);

    this->symbols.insert(this->symbols.begin() + index, s);

    return vector;
}


//crea un nuovo symbol e lo inserisce nel crdt in posizione index
std::vector<int> Client::insertSymbolNewCRDT(int index, wchar_t character, const std::string &username) {
    std::vector<int> vector;
    if (this->symbols.empty()) {
        vector = {0};
        index = 0;
    } else if (index > this->symbols.size() - 1) {
        vector = {this->symbols.back().getPosizione().at(0) + 1};
        index = this->symbols.size();
    } else if (index == 0) {
        vector = {this->symbols.front().getPosizione().at(0) - 1};
    } else
        vector = generatePos(index);
    Symbol s(character, username, vector);

    this->symbols.insert(this->symbols.begin() + index, s);

    return vector;
}

//aggiunge al crdt un symbol sulla base del vettore posizione
int Client::generateIndexCRDT(Symbol symbol, int iter, int start,
                              int end) { //inserisci symbol gia' generato in un vettore di symbol nel posto giusto
    if (start == -1 && end == -1) {
        if (this->symbols.empty())
            return 0;
        if (symbol.getPosizione()[0] < this->symbols[0].getPosizione()[0])
            return 0;
        start = 0;
        end = this->symbols.size();
    }
    if (start == end) {
        return iter;
    }
    int newStart = -1;
    int newEnd = start;
    for (auto iterPositions = this->symbols.begin() + start;
         iterPositions != this->symbols.begin() + end; ++iterPositions) {
        if (iterPositions->getPosizione().size() > iter && symbol.getPosizione().size() > iter) {
            if (iterPositions->getPosizione()[iter] == symbol.getPosizione()[iter] && newStart == -1)
                newStart = newEnd;
            if (iterPositions->getPosizione()[iter] > symbol.getPosizione()[iter]) {
                if (newStart == -1)
                    return newEnd;
                else
                    return generateIndexCRDT(symbol, ++iter, newStart, newEnd);
            }
        }
        newEnd++;
    }

    return newEnd;
}

//rimuove dal crdt uno specifico symbol
std::vector<int> Client::eraseSymbolCRDT(std::vector<Symbol> symbolsToErase) {
    int lastFound = 0;
    std::vector<int> erased;
    for (auto iterSymbolsToErase = symbolsToErase.begin(); iterSymbolsToErase !=
                                                           symbolsToErase.end(); ++iterSymbolsToErase) { //cerca prima da dove hai trovato prima in poi
        int count = lastFound;
        if ((lastFound - 2) >= 0)
            lastFound -= 2;
        else if ((lastFound - 1) >= 0)
            lastFound--;
        bool foundSecondPart = false;
        for (auto iterSymbols = this->symbols.begin() + lastFound; iterSymbols != this->symbols.end(); ++iterSymbols) {
            if (*iterSymbolsToErase == *iterSymbols) {
                erased.push_back(generateIndexCRDT(*iterSymbols, 0, -1, -1));
                this->symbols.erase(iterSymbols);
                lastFound = count;
                foundSecondPart = true;
                break;
            }
            count++;
        }
        if (!foundSecondPart) { //se non hai trovato cerca anche nella prima parte
            for (auto iterSymbols = this->symbols.begin();
                 iterSymbols != this->symbols.begin() + lastFound; ++iterSymbols) {
                if (*iterSymbolsToErase == *iterSymbols) {
                    erased.push_back(generateIndexCRDT(*iterSymbols, 0, -1, -1));
                    this->symbols.erase(iterSymbols);
                    lastFound = count;
                    break;
                }
                count++;
            }
        }
    }
    return erased;
}

void Client::insertSymbolIndex(Symbol symbol, int index) {
    int i = 0;
    for (auto iterPositions = this->symbols.begin(); iterPositions != this->symbols.end(); ++iterPositions) {
        if (index == i) {
            this->symbols.insert(iterPositions, symbol);
            return;
        }
        i++;
    }
    this->symbols.insert(this->symbols.end(), symbol);
}

//genera nuovo vettore posizione per un nuovo symbol
std::vector<int> Client::generatePos(int index) {
    const std::vector<int> posBefore = symbols[index - 1].getPosizione();
    const std::vector<int> posAfter = symbols[index].getPosizione();
    std::vector<int> newPos;
    return generatePosBetween(posBefore, posAfter, newPos);
}

//richiamata da generatePos
std::vector<int> Client::generatePosBetween(std::vector<int> pos1, std::vector<int> pos2, std::vector<int> newPos) {
    int id1 = pos1.at(0);
    int id2 = pos2.at(0);

    if (id2 - id1 == 0) {
        newPos.push_back(id1);
        pos1.erase(pos1.begin());
        pos2.erase(pos2.begin());
        if (pos1.empty()) {
            newPos.push_back(pos2.front() - 1);
            return newPos;
        } else
            return generatePosBetween(pos1, pos2, newPos);
    } else if (id2 - id1 > 1) {
        newPos.push_back(pos1.front() + 1);
        return newPos;
    } else if (id2 - id1 == 1) {
        newPos.push_back(id1);
        pos1.erase(pos1.begin());
        if (pos1.empty()) {
            newPos.push_back(0); //
            return newPos;
        } else {
            newPos.push_back(pos1.front() + 1);
            return newPos;
        }
    }
    return std::vector<int>();
}

void Client::sendAtServer(const json &js) {
    std::string msg = js.dump().c_str();
    message mess;
    mess.body_length(msg.size());
    std::memcpy(mess.body(), msg.data(), mess.body_length());
    mess.body()[mess.body_length()] = '\0';
    mess.encode_header();
    std::cout << "Messaggio da inviare al server: " << mess.body() << std::endl;
    this->write(mess);
}
