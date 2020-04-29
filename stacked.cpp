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


stacked::stacked(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::stacked),
    client_(new Client)
{
    TextEdit *te = new TextEdit(client_);
    connect(te,&TextEdit::logout,this,&stacked::logout);
    connect(te,&TextEdit::closeAll,this,&stacked::closeAll);
    QObject::connect(client_, &Client::formResultSuccess, this, &stacked::showPopupSuccess);

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
               /*
                *  PRENDI I DATI E INVIA A SERVER
                * */

               json j = json{
                   {"operation", "request_login"},
                   {"username",user.toStdString()},
                   {"password",psw.toStdString()}
                   };
               std::string mess = j.dump().c_str();
                message msg;
                msg.body_length(mess.size());
                std::memcpy(msg.body(), mess.data(), msg.body_length());
                msg.body()[msg.body_length()] = '\0';
                msg.encode_header();
                std::cout <<"Richiesta da inviare al server "<< msg.body() << std::endl;
                sendmessage(msg);
           }//try
           catch (std::exception& e){
               std::cerr << "Exception: " << e.what() << "\n";
           };
    }//else esterno
}
void stacked::sendmessage(message mess) {
    client_->write(mess);
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

             json j = json{
                    {"operation","request_signup"},
                    {"email",email.toStdString()},
                    {"username",user.toStdString()},
                    {"password",psw1.toStdString()}
             };
             /*
              *  PRENDI I DATI E INVIA A SERVER
              * */
             std::string mess = j.dump().c_str();
            message msg;
            msg.body_length(mess.size());
            std::memcpy(msg.body(), mess.data(), msg.body_length());
            msg.body()[msg.body_length()] = '\0';
            msg.encode_header();
            std::cout <<"Richiesta da inviare al server "<< msg.body() << std::endl;
            sendmessage(msg);
            /*
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
*/
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }
}

void stacked::showPopupSuccess(QString result) {
    if(result == "LOGIN_SUCCESS" || result == "SIGNUP_SUCCESS") {
        ui->stackedWidget->setCurrentIndex(2);
        std::cout << "Il colore e' " << client_->getColor().toStdString() << std::endl;
    } else  {
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
}

void stacked::on_reglogButton_clicked(){
    ui->stackedWidget->setCurrentIndex(1);

}
void stacked::on_fileButton_clicked(){

    ui->stackedWidget->setCurrentIndex(3);
}
void stacked::on_newFileButton_clicked(){
    ui->stackedWidget->setCurrentIndex(3);
}
void stacked::logout(){
    ui->stackedWidget->setCurrentIndex(2);
}
void stacked::closeAll(){
    this->close();
}
