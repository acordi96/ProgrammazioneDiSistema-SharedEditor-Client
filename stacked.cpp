#include "stacked.h"
#include "ui_stacked.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <iostream>
#include <boost/asio.hpp>
#include "json.hpp"

using json = nlohmann::json;
using boost::asio::ip::tcp;

boost::asio::io_service io_service;

tcp::resolver resolver(io_service);
tcp::resolver::query query(tcp::v4(), "127.0.0.1", "3000");
tcp::resolver::iterator iterator = resolver.resolve(query);

tcp::socket s(io_service);
stacked::stacked(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::stacked)
{
    TextEdit *te = new TextEdit();
    connect(te,&TextEdit::logout,this,&stacked::logout);
    connect(te,&TextEdit::closeAll,this,&stacked::closeAll);
    ui->setupUi(this);
    ui->stackedWidget->addWidget(te);
}

stacked::~stacked()
{
    delete ui;
}
/*
 *  indici
 *  0 = loginpage
 *  1 = registration form
 *  2 = userpage
 *  3 = texteditor
 *
 * */
void stacked::on_loginButton_clicked(){
    QString user = ui->user_log_line->text();
    QString psw = ui->psw_log_line->text();

    if(user.isEmpty() || psw.isEmpty()){
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Error! Empty username and/or password"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addWidget(buttonBox);

        connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);
        connect(buttonBox,&QDialogButtonBox::rejected,dialog,&QDialog::reject);

        dialog->show();
    }
    else{
        try {
               boost::asio::connect(s, iterator);
               /*
                *  PRENDI I DATI E INVIA A SERVER
                * */

               json j = json{
                   {"operation", "request_login"},
                   {"username",user.toStdString()},
                   {"password",psw.toStdString()}
                   };
               std::string msg = j.dump().c_str();
               size_t size_msg = msg.size();
               //std::cout<<data<<std::endl;
               boost::asio::write(s, boost::asio::buffer(msg, size_msg));
               /*
                *  RICEVI DA SERVER E IN BASE A RISPOSTA APRI PAGINA
                * */
               char messageFromServer[1500];
               boost::system::error_code error;
               memset(messageFromServer, 0, 1500);
               size_t lengthMessage = s.read_some(boost::asio::buffer(messageFromServer), error);
               json messageServer = json::parse(messageFromServer);
               std::string requestType = messageServer.at("response").get<std::string>();

               if(requestType == "LOGIN_SUCCESS"){
                   ui->stackedWidget->setCurrentIndex(2);
               }
               else{

                   QDialog *dialog = new QDialog();
                   QVBoxLayout *layout = new QVBoxLayout();
                   dialog->setLayout(layout);
                   layout->addWidget(new QLabel("Error! Invalid username and/or password"));
                   QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
                   layout->addWidget(buttonBox);

                   connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);
                   connect(buttonBox,&QDialogButtonBox::rejected,dialog,&QDialog::reject);

                   dialog->show();
               }
           }//try
           catch (std::exception& e){
               std::cerr << "Exception: " << e.what() << "\n";
           };
    }//else esterno
}
void stacked::on_form_regButton_clicked(){
    //qui creare richiesta e inviarla al server
    //^[0-9a-zA-Z]+([0-9a-zA-Z][-._+])[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)([0-9a-zA-Z][.])[a-zA-Z]{2,6}$
    QRegularExpression email_regex("^([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]{2,5})$");
    QRegularExpression psw_regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.{8,})");

    QString email = ui->email_line->text();
    QString user = ui->user_line_form->text();
    QString psw1 = ui->psw_line_form->text();
    QString psw2 = ui->confirm_psw_line->text();

    if(!email_regex.match(email).hasMatch()){
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Error! Not accpted email format"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);

        dialog->show();
    }
    else if(!psw_regex.match(psw1).hasMatch()){
     // check password
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Password must be at least 8 characters long "
                                     "and include at least one upper case letter, "
                                     "a number and a special character"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);

        dialog->show();
     //ui->stackedWidget->setCurrentIndex(3);
    }else if(!(psw1==psw2)){
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Non matching password!"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);

        dialog->show();
    }
    else{
        try {
             boost::asio::connect(s, iterator);

             json j = json{
                    {"operation","SIGNUP_R"},
                    {"email",email.toStdString()},
                    {"username",user.toStdString()},
                    {"password",psw1.toStdString()}
             };
             /*
              *  PRENDI I DATI E INVIA A SERVER
              * */
             std::string msg = j.dump().c_str();
             size_t size_msg = msg.size();
             boost::asio::write(s, boost::asio::buffer(msg, size_msg));
             /*
              *  RICEVI DA SERVER E IN BASE A RISPOSTA APRI PAGINA
              * */
             char messageFromServer[1500];
             boost::system::error_code error;
             memset(messageFromServer, 0, 1500);
             size_t lengthMessage = s.read_some(boost::asio::buffer(messageFromServer), error);
             json messageServer = json::parse(messageFromServer);
             std::string requestType = messageServer.at("response").get<std::string>();

             if(requestType=="SIGNUP_OK"){
                 ui->stackedWidget->setCurrentIndex(2);
             }
             else{
                 QDialog *dialog = new QDialog();
                 QVBoxLayout *layout = new QVBoxLayout();
                 dialog->setLayout(layout);
                 layout->addWidget(new QLabel("Error! Something went wrong. Try again later."));
                 QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
                 layout->addWidget(buttonBox);

                 connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);

                 dialog->show();
             }

        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }
}
void stacked::on_reglogButton_clicked(){
    ui->stackedWidget->setCurrentIndex(1);

}
void stacked::on_fileButton_clicked(){

    ui->stackedWidget->setCurrentIndex(3);
}
void stacked::logout(){
    ui->stackedWidget->setCurrentIndex(2);
}
void stacked::closeAll(){
    this->close();
}
