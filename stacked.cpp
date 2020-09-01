#include "stacked.h"
#include "ui_stacked.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <iostream>
#include <boost/asio.hpp>
#include <QtWidgets/QInputDialog>
#include "json.hpp"
#include "lib/md5.h"

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
    //Userpage *up = new Userpage(this,client_);
    ui->setupUi(this);
    //ui->stackedWidget->addWidget(up);
    ui->stackedWidget->addWidget(te);

    //Userpage *up = new Userpage();
    //ui->stackedWidget->addWidget(up);
}

stacked::~stacked()
{
    delete ui;
}
/*
 *  indici
 *  0 = loginpage
 *  1 = registration form
 *  2 = texteditor
 *  3 = userpage
 *
 * */
void stacked::on_loginButton_clicked(){
    QString user = ui->user_log_line->text();
    QString psw = ui->psw_log_line->text();
   /* QPushButton *b1 = new QPushButton();
    QPushButton *b2 = new QPushButton();
    b2->setObjectName("button2");
    b1->setObjectName("button1");
    ui->listaProva->*/


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
                   {"password", md5(psw.toUtf8().constData())
                   }
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
                    {"password",md5(psw1.toUtf8().constData())}
             };
             /*
              *  PRENDI I DATI E INVIA A SERVER
              * */
             std::cout << "\n username inviato per la registrazione: " + user.toStdString() ;
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



// type_request=="QUERY_ERROR"    LOGIN_ERROR
// || type_request=="CONNESSION_ERROR" || SIGNUP_ERROR_DUPLICATE_USERNAME || SIGNUP_ERROR_INSERT_FAILED)

void stacked::showPopupSuccess(QString result) {
    if(result == "LOGIN_SUCCESS" || result == "SIGNUP_SUCCESS") {
        Userpage *up = new Userpage(this,client_);
        //ui->setupUi(this);
        ui->stackedWidget->addWidget(up);
        ui->stackedWidget->setCurrentIndex(3);
        //ui->stackedWidget->setCurrentIndex(2);
        std::cout << "Il colore e' " << client_->getColor().toStdString() << std::endl;
    } else  {
        QDialog *dialog = new QDialog();
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        if(result == "QUERY_ERROR" || result =="CONNESSION_ERROR" || result == "SIGNUP_ERROR_INSERT_FAILED"){
            layout->addWidget(new QLabel("Error with the DB, try again"));
        }else if(result == "LOGIN_ERROR"){
            layout->addWidget(new QLabel("Error! Invalid username and/or password"));
        }else if(result == "SIGNUP_ERROR_DUPLICATE_USERNAME"){
            layout->addWidget(new QLabel("Error! Username already exixts"));
        }else if(result == "new_file_created"){

            layout->addWidget(new QLabel("File correclty created"));
            ui->stackedWidget->setCurrentIndex(2);
            //ui->stackedWidget->setCurrentIndex(3);
        }else if(result == "new_file_already_exist"){
            std::cout << "\n file già esistente ";
            layout->addWidget(new QLabel("File already exists"));
        }else if (result == "file_opened"){
            std::cout<<"\n file aperto";
            ui->stackedWidget->setCurrentIndex(2);

        }

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
/*
void stacked::on_fileButton_clicked(){

    ui->stackedWidget->setCurrentIndex(3);
}*/
/*
void stacked::on_newFileButton_clicked(){

    QInputDialog modalWindow;
    QString label = "File name: ";
    modalWindow.setLabelText(label);
    modalWindow.setWindowTitle("New FIle");
    modalWindow.setMinimumSize(QSize(300, 150));
    modalWindow.setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::MinimumExpanding);
    modalWindow.setCancelButtonText("cancel");
    modalWindow.setOkButtonText("create");

    if ( modalWindow.exec() == 1)
    {

        while (modalWindow.textValue() == ""){

            QDialog *dialog = new QDialog();
            QVBoxLayout *layout = new QVBoxLayout();
            dialog->setLayout(layout);
            layout->addWidget(new QLabel("Insert a name for the new file"));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);

            connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);

            dialog->exec();

            if(modalWindow.exec()==0)
                break;

        }
        //controllo valdità formato nome file
        while( modalWindow.textValue().toStdString().find_first_of('\\')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('/')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of(':')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('*')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('?')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('"')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('<')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('>')!=std::string::npos ||
               modalWindow.textValue().toStdString().find_first_of('|')!=std::string::npos
                ){
            QDialog *dialog = new QDialog();
            QVBoxLayout *layout = new QVBoxLayout();
            dialog->setLayout(layout);
            layout->addWidget(new QLabel("Error! characters \\,/,:,*,?,\",<,>,| are not allowed in a file's name"));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);
            connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);
            dialog->exec();
            if(modalWindow.exec()==0)
                break;
        }

        while (modalWindow.textValue().length() > 100){

            QDialog *dialog = new QDialog();
            QVBoxLayout *layout = new QVBoxLayout();
            dialog->setLayout(layout);
            layout->addWidget(new QLabel("Error! The max length of a file's name is 100 characters"));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);
            connect(buttonBox,&QDialogButtonBox::accepted,dialog,&QDialog::accept);
            dialog->exec();
            if(modalWindow.exec()==0)
                break;
        }

        //invio al server la richiesta per creare un nuovo file
        try {
            json j = json{
                    {"operation","request_new_file"},
                    {"name",modalWindow.textValue().toStdString()},
                    {"username",client_->getUser().toStdString()}
            };

              //PRENDI I DATI E INVIA A SERVER

            std::string mess = j.dump().c_str();
            message msg;
            msg.body_length(mess.size());
            std::memcpy(msg.body(), mess.data(), msg.body_length());
            msg.body()[msg.body_length()] = '\0';
            msg.encode_header();
            std::cout <<"Richiesta da inviare al server "<< msg.body() << std::endl;
            sendmessage(msg);

        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }

        QString testo = modalWindow.textValue();
        std::cout << "CLICK SUL PULSANTE CREATE " << testo.toStdString() ;
    }
    //modalWindow.exec();
    //ui->stackedWidget->setCurrentIndex(3);
}*/
void stacked::on_form_cancButton_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void stacked::logout(){
    ui->stackedWidget->setCurrentIndex(3);
    //ui->stackedWidget->setCurrentIndex(2);
}

void stacked::closeAll(){
    this->close();
}

