#include "userpage.h"
#include "Client.h"
#include "stacked.h"



#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QScrollArea>
#include <QDebug>
#include <iostream>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QBuffer>
#include <QFileDialog>
#include <QMenu>
#include <QPixmap>
#include <QtCore/QModelIndex>
#include <QtWidgets/QMessageBox>

#ifdef Q_OS_MACOS
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

Userpage::Userpage(QWidget *parent,Client *c):
        QMainWindow(parent),client_(c)
{
    page = new QWidget(this);
    hLayout = new QHBoxLayout(page);
    hLayout->setSpacing(0);
    setupUserinfo();
    setupRecentFiles();
    hLayout->addWidget(userinfo);
    hLayout->addWidget(recent);
    std::cout << "\n creazione userpage";
    page->setLayout(hLayout);
    setCentralWidget(page);
    QObject::connect(client_, &Client::updateFile, this, &Userpage::updateRecentFiles);
    //QObject::connect(page, &stacked::updateRecentFiles, this, &Userpage::updateFiles);
}

void Userpage::setupRecentFiles(){
     recent = new QWidget(page);
     recent->setObjectName(QString::fromUtf8("Recent Files"));
     recent->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(0, 0, 255, 255), stop:1 rgba(255, 255, 255, 255));"));
     //recent->setContextMenuPolicy(Qt::CustomContextMenu);
     QLabel *recentLabel = new QLabel("Recent Files",recent);
     recentLabel->setObjectName(QString::fromUtf8("recentLabel"));
     recentLabel->setGeometry(QRect(20, 0, 261, 31));
     recentLabel->setStyleSheet(QString::fromUtf8("font: 75 22pt \"Sawasdee Bold\";"));
     recentLabel->setAlignment(Qt::AlignCenter);


     QScrollArea *scrollArea = new QScrollArea(recent);
     scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
     scrollArea->setGeometry(QRect(10,60,281,401));
     scrollArea->setFrameShape(QFrame::NoFrame);
     scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     scrollArea->setWidgetResizable(true);

     QWidget *scrollAreaWidgets = new QWidget(scrollArea);
     scrollAreaWidgets->setObjectName(QString::fromUtf8("scrollAreaWidgets"));
     QVBoxLayout *verticalLayout = new QVBoxLayout(scrollAreaWidgets);
     verticalLayout->setObjectName("verticalLayout");
     QLabel *title = new QLabel(QString::fromUtf8("author    title"),recent);
     title->setGeometry(QRect(20,35,261,20));
     title->setAlignment(Qt::AlignCenter);

     QPushButton *button;

     for (auto p : client_->files){
         //std::string s = iter1.next();
         std::cout <<"\nentrato nel while dei file ";
         button = new QPushButton(scrollAreaWidgets);
         //button->setContextMenuPolicy(Qt::CustomContextMenu);
         std::cout << "\n" << p.first << ": " << p.second << "\n";
         button->setObjectName(QString::fromStdString(p.first) + "_|_" + QString::fromStdString(p.second));
         button->setText(QString::fromStdString(p.first)+":   "+QString::fromStdString(p.second));
         button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                 "border:1px;\n"
                                                 "background-color: blue;\n"
                                                 "}"));

         button->setFlat(true);
         //button->setContentsMargins(10,0,0,0);
         verticalLayout->addWidget(button);

         connect(button,SIGNAL(clicked()),SLOT(on_fileName_clicked()));
         //connect(button, SIGNAL(customContextMenuRequested(QPoint)),
                // SLOT(customMenuRequested(QPoint)));
     }


     scrollAreaWidgets->setLayout(verticalLayout);
     scrollArea->setWidget(scrollAreaWidgets);

     QPushButton *openButton = new QPushButton("Open",recent);
     QPushButton *renameButton = new QPushButton("Rename",recent);
     QPushButton *deleteButton = new QPushButton("Delete",recent);
     QPushButton *inviteButton = new QPushButton("Invite",recent);

     openButton->setObjectName(QString::fromUtf8("openButton"));
     openButton->setGeometry(QRect(40,380,89,25));
     openButton->setGeometry(QRect(40, 380, 89, 25));
     openButton->setStyleSheet(QString::fromUtf8("QPushButton#openButton{\n"
     "border:1px;\n"
     "}\n"
     "QPushButton#openButton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));
     openButton->setFlat(true);

    connect(openButton,SIGNAL(clicked()),SLOT(on_openButton_clicked()));

     renameButton->setObjectName(QString::fromUtf8("renameButton"));
     renameButton->setGeometry(QRect(160, 380, 89, 25));
     renameButton->setStyleSheet(QString::fromUtf8("QPushButton#renameButton{\n"
     "border:1px;\n"
     "}\n"
     "QPushButton#renameButton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));
     renameButton->setFlat(true);
     connect(renameButton,SIGNAL(clicked()),SLOT(on_renameButton_clicked()));

     inviteButton->setObjectName(QString::fromUtf8("inviteButton"));
     inviteButton->setGeometry(QRect(40, 430, 89, 25));
     inviteButton->setStyleSheet(QString::fromUtf8("QPushButton#inviteButton{\n"
     "border:1px;\n"
     "}\n"
     "QPushButton#inviteButton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));
     inviteButton->setFlat(true);

     deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
     deleteButton->setGeometry(QRect(160, 430, 89, 25));
     deleteButton->setStyleSheet(QString::fromUtf8("QPushButton#deleteButton{\n"
     "border:1px;\n"
     "}\n"
     "QPushButton#deleteButton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));
     deleteButton->setFlat(true);
     connect(deleteButton,SIGNAL(clicked()),SLOT(on_deleteButton_clicked()));

     hLayout->addWidget(recent);

     QPushButton *upLogoutButton = new QPushButton(recent);
     upLogoutButton->setObjectName(QString::fromUtf8("upLogoutButton"));
     upLogoutButton->setGeometry(QRect(280,20,41,31));

     QIcon logoutIcon;
     logoutIcon.addFile(rsrcPath+QString::fromUtf8("/logout.png"),QSize(),QIcon::Normal,QIcon::On);
     upLogoutButton->setIcon(logoutIcon);
     upLogoutButton->setIconSize(QSize(32,32));
     upLogoutButton->setFlat(true);
     connect(upLogoutButton,&QPushButton::clicked,this,[=](){
        requestLogout();
        emit this->upLogout();
     });
     hLayout->addWidget(recent);
}

void Userpage::requestLogout(){
    json j = json{
             {"operation","req_logout"},
             {"username",client_->getUser().toStdString()},
    };
    std::string mess = j.dump().c_str();
    message msg;
    msg.body_length(mess.size());
    std::memcpy(msg.body(),mess.data(),msg.body_length());
    msg.body()[msg.body_length()]='\0';
    msg.encode_header();
    std::cout<<"Messaggio da inviare al server "<< msg.body() << std::endl;
    client_->write(msg);
}

void Userpage::setupUserinfo(){

     QVBoxLayout *userLayout = new QVBoxLayout();
     userLayout->setSpacing(0);
     userinfo = new QWidget(page);
     userinfo->setObjectName(QString::fromUtf8("user_info"));
     userinfo->setStyleSheet(QString::fromUtf8("background-color:rgb(255,255,255); "));

     myIcon = new QWidget(userinfo);
     myIcon->setObjectName(QString::fromUtf8("myIcon"));
     myIcon->setGeometry(QRect(10,120,101,91));
     myIcon->setStyleSheet(QString::fromUtf8("background-color:rgb(0,0,255)"));
     QPushButton *modifyIcon = new QPushButton(myIcon);
     modifyIcon->setObjectName(QString::fromUtf8("modifyIcon"));
     modifyIcon->setGeometry(QRect(80,70,21,21));
     modifyIcon->setStyleSheet(QString::fromUtf8("QPushButton#modifyIcon:hover{background-color:rgb(82,82,171);border:1px;}"));
     QIcon gearIcon;
     gearIcon.addFile(rsrcPath+QString::fromUtf8("/mechanical-gears-.png"),QSize(),QIcon::Normal,QIcon::On);
     modifyIcon->setIcon(gearIcon);
     modifyIcon->setFlat(true);

     QLabel *userpageLabel = new QLabel("Userpage",userinfo);
     userpageLabel->setObjectName(QString::fromUtf8("userpageLabel"));
     userpageLabel->setGeometry(QRect(80, 10, 221, 61));
     userpageLabel->setStyleSheet(QString::fromUtf8("font: 75 36pt \"Sawasdee\";"));
     userpageLabel->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

     QWidget *user_image = new QWidget(userinfo);
     user_image->setGeometry(QRect(10, 10, 61, 51));
     user_image->setStyleSheet(QString::fromUtf8("image: url(:/images/photo_2020-09-22_17-58-05.jpg);"));

     QPushButton *usernameButton = new QPushButton(client_->getUser(),userinfo);
     usernameButton->setObjectName(QString::fromUtf8("usernameButton"));
     usernameButton->setGeometry(QRect(110, 110, 171, 71));
     usernameButton->setStyleSheet(QString::fromUtf8("font: 75 25pt \"Sawasdee\";"));
     usernameButton->setFlat(true);

     QLabel *email_lab = new QLabel("email",userinfo);
     email_lab->setObjectName(QString::fromUtf8("email_lab"));
     email_lab->setGeometry(QRect(150, 180, 91, 31));
     email_lab->setStyleSheet(QString::fromUtf8("font: 14pt \"Sawasdee\";"));
     email_lab->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

     QPushButton *newFileButton = new QPushButton("New File",userinfo);
     newFileButton->setObjectName(QString::fromUtf8("newFileButton"));
     newFileButton->setGeometry(QRect(90, 320, 121, 31));
     newFileButton->setStyleSheet(QString::fromUtf8("QPushButton#newFileButton{\n"
        "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(0, 0, 255, 255), stop:1 rgba(255, 255, 255, 255));\n"
        "border: 1px;\n"
     "}\n"
     "QPushButton#newFileButton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));

     QIcon icon;
     icon.addFile(QString::fromUtf8(":/images/Science-Plus2-Math-icon.png"), QSize(), QIcon::Normal, QIcon::On);
     newFileButton->setIcon(icon);
     newFileButton->setIconSize(QSize(32, 32));
     newFileButton->setFlat(false);

     this->lineURL = new QLineEdit(userinfo);
     lineURL->setObjectName(QString::fromUtf8("lineURL"));
     lineURL->setGeometry(QRect(10,400,211,25));
     lineURL->setPlaceholderText(QString::fromUtf8("Insert Invitation Code"));

     QPushButton *openURLbutton = new QPushButton("Accept",userinfo);
     openURLbutton->setObjectName(QString::fromUtf8("openURLbutton"));
     openURLbutton->setGeometry(QRect(230,400,71,25));
     openURLbutton->setStyleSheet(QString::fromUtf8("QPushButton#openURLbutton{\n"
     "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(0, 0, 255, 255), stop:1 rgba(255, 255, 255, 255));\n"
     "border: 0px;\n"
     "}\n"
     "QPushButton#openURLbutton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));

     userLayout->addWidget(userinfo);
     hLayout->addLayout(userLayout);

     connect(newFileButton, SIGNAL (released()), this, SLOT (handleNewFileButton()));
     connect(modifyIcon,&QPushButton::clicked,this,&Userpage::iconSelector);
     connect(openURLbutton, SIGNAL (released()), this, SLOT (handleOpenURLbutton()));
}
void Userpage::iconSelector(){
    /*
    selector = new QWidget();
    QWidget *widget;
    QPushButton *button;
    selector->setFixedSize(600,400);
    selector->move(200,100);
    int offY = 20, startY=80;
    int offX = 30, startX=70;
    int dim = 80;

    for(int i=0; i<3;i++){
        widget = new QWidget(selector);
        widget->setGeometry(QRect(startX+i*(offX+dim),startY,dim,dim));
        widget->setStyleSheet(QString::fromUtf8("image:url(")+rsrcPath+QString::fromUtf8("/beach.png")+QString::fromUtf8(");"));
        button = new QPushButton(widget);
        //button->setObjectName(QString::fromUtf8("icon_")+i);
        std::cout<<"\n"<<button->objectName().toStdString()<<std::endl;
        button->setGeometry(QRect(0,0,80,80));
        button->setFlat(true);

    }

    connect(button,&QPushButton::clicked,this,&Userpage::changeIcon);
    selector->show();
    */

    /******** QFileDialog -> open from my own PC ********/

    QFileDialog fileDialog(this,tr("Choose Icon..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(QStringList()<<"image/jpg"<<"image/png");

    if(fileDialog.exec() != QDialog::Accepted){
        return;
    }
    const QString selected = fileDialog.selectedFiles().first();
    std::cout<<"\nSelected file:  "<<selected.toStdString()<<std::endl;

    myIcon->setStyleSheet(QString::fromUtf8("image:url(")+selected+QString::fromUtf8(");"));

    // inviare al server

    const QImage image = QImage(selected);
    QByteArray ban;

    QDataStream out(&ban,QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_12);
    out<<image; //serialize image

    json j = json{
             {"operation","send-icon"},
             {"username",client_->getUser().toStdString()},
             {"icon",ban.toBase64().data()},
    };

    std::string mess = j.dump().c_str();
    message msg;
    msg.body_length(mess.size());
    std::memcpy(msg.body(), mess.data(), msg.body_length());
    msg.body()[msg.body_length()] = '\0';
    msg.encode_header();
    std::cout <<"Richiesta da inviare al server "<< msg.body() << std::endl;
    sendmessage(msg);


}

void Userpage::changeIcon(){
  myIcon->setStyleSheet(QString::fromUtf8("image:url(")+rsrcPath+QString::fromUtf8("/beach.png")+QString::fromUtf8(");"));

}

void Userpage::handleNewFileButton()
{

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
            std::cout << "\n username inviato per la creazione del file: " + client_->getUser().toStdString();
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
        client_->setFileName(testo);
        std::cout << "CLICK SUL PULSANTE CREATE " << testo.toStdString() ;
    }
    //modalWindow.exec()
    //ui->stackedWidget->setCurrentIndex(3);
}


void Userpage::sendmessage(message mess) {
    client_->write(mess);
}

/*
void Userpage::customMenuRequested(QPoint pos) {
    //QModelIndex index=button->indexAt(pos);


    QObject *sender =  QObject::sender();
    QString str = sender->objectName();
    fileName = str.toStdString();
    //QMessageBox::information(0, "Button", sender->objectName());
    std::cout << "\n bottone schiacciato " << fileName <<"\n";


    std::cout << "\n creazione del menu \n" << "pos = " << pos.x() <<","<<pos.y();
    QMenu *menu=new QMenu(this);
    menu->addAction("Open", this,SLOT(openFile()));
    menu->addAction("Rename", this,SLOT(renameFile()));
    menu->addAction("Delete", this, SLOT(deleteFile()));

    menu->exec(QCursor::pos());
}
*/

void Userpage::on_fileName_clicked(int i){

    std::cout << "\n valore di i : " << i;
    std::string s;
    if(i==0) {
        QObject *sender = QObject::sender();
        QString str = sender->objectName();
        s = str.toStdString();
        //QMessageBox::information(0, "Button", sender->objectName());
        std::cout << "\n bottone schiacciato" << s << "\n";
    }else {
        s = fileName;
    }
    if(selectedFile!=""){
        //deseleziono vecchio button e seleziono quello nuovo
        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "border:1px;\n"
                                                "background-color: blue;\n"
                                                "}"));
    }
    selectedFile = s;
    QPushButton *b = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
    b->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                            "border:1px;\n"
                                            "background-color: red;\n"
                                            "}"));

    std::cout << "\n\ndopo focus";

}

void Userpage::on_openButton_clicked(){
    std::cout << "\n\n il tasto open funziona \n\n";
    std::cout << "\n file selezionato: " << selectedFile;

    std::cout << "\napri file\n";

    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Attenzione!"),
                tr("Selezionare un file da aprire") );
        return;
    }

    //QMessageBox::information(0, "Button", sender->objectName());
    std::cout << "\n bottone schiacciato: " << fileName <<"\n";
    //on_fileName_clicked(1);

    std::string username;
    std::string name;
    for(int i = 0; i < selectedFile.length(); i++) {
        if(selectedFile[i] == '_' && selectedFile[i+1] == '|' && selectedFile[i+2] == '_') { //parse button name (username+"_|_"+name)
            i += 3;
            while(selectedFile[i] != '\0')
                name += selectedFile[i++];
            break;
        }
        username += selectedFile[i];
    }
    try {
        json j = json{ //TODO: fixare apertura file con nome utente
                {"operation","open_file"},
                {"name",name},
                {"username",username}
        };
        client_->setFileName(QString::fromStdString(name));
        std::cout<< "\nFle Name Attuale: "<<client_->getFileName().toStdString()<<std::endl;
        //PRENDI I DATI E INVIA A SERVER
        std::cout << "\ninvio richiesta apertura file: " << selectedFile << " \n username inviato per la creazione del file: " << client_->getUser().toStdString();
        std::string mess = j.dump().c_str();
        message msg;
        msg.body_length(mess.size());
        std::memcpy(msg.body(), mess.data(), msg.body_length());
        msg.body()[msg.body_length()] = '\0';
        msg.encode_header();
        std::cout <<"\n Richiesta da inviare al server "<< msg.body() << std::endl;
        sendmessage(msg);
        //deseleziono il file
        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "border:1px;\n"
                                                "background-color: blue;\n"
                                                "}"));

        selectedFile = "";


    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }


}

void Userpage::on_renameButton_clicked() {
    std::cout << "\n\n il tasto rename funziona \n\n";
    std::cout << "\n file selezionato: " << selectedFile;

    std::cout<<"\nrinomina file";
    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Attenzione!"),
                tr("Selezionare un file da rinominare") );
        return;
    }

    //controllare se l'utente ha il permesso per rinominare il file
    std::string name;
    std::string username;

    for(int i = 0; i < selectedFile.length(); i++) {
        if(selectedFile[i] == '_' && selectedFile[i+1] == '|' && selectedFile[i+2] == '_') { //parse button name (username+"_|_"+name)
            i += 3;
            while(selectedFile[i] != '\0')
                name += selectedFile[i++];
            break;
        }
        username += selectedFile[i];
    }
    if(username != client_->getUser().toStdString()){
        //l'utente non ha il permesso di rinominare il file
        std::cout << "\n no permesso\n";

        QMessageBox::information(
                this,
                tr("Attenzione!"),
                tr("Impossibile rinominare il file, azione permessa solo a chi lo ha creato") );
    }else {

        QInputDialog modalWindow;
        QString label = "New name: ";
        modalWindow.setLabelText(label);
        modalWindow.setWindowTitle("Rename file");
        modalWindow.setMinimumSize(QSize(300, 150));
        modalWindow.setSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::MinimumExpanding);
        modalWindow.setCancelButtonText("cancel");
        modalWindow.setOkButtonText("rename");

        if (modalWindow.exec() == 1) {

            while (modalWindow.textValue() == "") {

                QDialog *dialog = new QDialog();
                QVBoxLayout *layout = new QVBoxLayout();
                dialog->setLayout(layout);
                layout->addWidget(new QLabel("Insert a name for the new file"));
                QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
                layout->addWidget(buttonBox);

                connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

                dialog->exec();

                if (modalWindow.exec() == 0)
                    break;

            }
            //controllo valdità formato nome file
            while (modalWindow.textValue().toStdString().find_first_of('\\') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('/') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of(':') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('*') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('?') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('"') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('<') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('>') != std::string::npos ||
                   modalWindow.textValue().toStdString().find_first_of('|') != std::string::npos
                    ) {
                QDialog *dialog = new QDialog();
                QVBoxLayout *layout = new QVBoxLayout();
                dialog->setLayout(layout);
                layout->addWidget(new QLabel("Error! characters \\,/,:,*,?,\",<,>,| are not allowed in a file's name"));
                QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
                layout->addWidget(buttonBox);
                connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
                dialog->exec();
                if (modalWindow.exec() == 0)
                    break;
            }

            while (modalWindow.textValue().length() > 100) {

                QDialog *dialog = new QDialog();
                QVBoxLayout *layout = new QVBoxLayout();
                dialog->setLayout(layout);
                layout->addWidget(new QLabel("Error! The max length of a file's name is 100 characters"));
                QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
                layout->addWidget(buttonBox);
                connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
                dialog->exec();
                if (modalWindow.exec() == 0)
                    break;
            }

            //invio al server la richiesta per rinominare un file

            try {
                json j = json{
                        {"operation", "request_new_name"},
                        {"old_name",  name},
                        {"new_name",  modalWindow.textValue().toStdString()},
                        {"username",  client_->getUser().toStdString()}
                };

                //PRENDI I DATI E INVIA A SERVER
                std::cout << "\n username inviato per rinominare file: " + client_->getUser().toStdString();
                std::string mess = j.dump().c_str();
                message msg;
                msg.body_length(mess.size());
                std::memcpy(msg.body(), mess.data(), msg.body_length());
                msg.body()[msg.body_length()] = '\0';
                msg.encode_header();
                std::cout << "\n Richiesta da inviare al server " << msg.body() << std::endl;
                sendmessage(msg);
                // deseleziono il pulsante che ho rinominato
                QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
                deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                          "border:1px;\n"
                                                          "background-color: blue;\n"
                                                          "}"));
                selectedFile = "";

            } catch (std::exception &e) {
                std::cerr << "Exception: " << e.what() << "\n";
            }
        }
    }

}

void Userpage::on_deleteButton_clicked() {
    std::cout << "\n\n il tasto delete funziona \n\n";
    std::cout << "\n file selezionato: " << selectedFile;

    std::cout<<"\ncancella file";

    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Attenzione!"),
                tr("Selezionare un file da eliminare") );
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Confirm you want to delete the selected file?");
    //msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::No){
        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "border:1px;\n"
                                                  "background-color: blue;\n"
                                                  "}"));
        selectedFile = "";
        return;
    }else{
        std::string name;
        std::string username;

        for(int i = 0; i < selectedFile.length(); i++) {
            if(selectedFile[i] == '_' && selectedFile[i+1] == '|' && selectedFile[i+2] == '_') { //parse button name (username+"_|_"+name)
                i += 3;
                while(selectedFile[i] != '\0')
                    name += selectedFile[i++];
                break;
            }
            username += selectedFile[i];
        }
        if(username != client_->getUser().toStdString()){
            //l'utente non ha il permesso di eliminare il file
            std::cout << "\n no permesso\n";

            QMessageBox::information(
                    this,
                    tr("Attenzione!"),
                    tr("Impossibile eliminare il file, azione permessa solo a chi lo ha creato") );
        }else {

            try {
                json j = json{
                        {"operation", "delete_file"},
                        {"name",  name},
                        {"username",  client_->getUser().toStdString()}
                };

                //PRENDI I DATI E INVIA A SERVER
                std::cout << "\n username inviato per eliminare file: " + client_->getUser().toStdString();
                std::string mess = j.dump().c_str();
                message msg;
                msg.body_length(mess.size());
                std::memcpy(msg.body(), mess.data(), msg.body_length());
                msg.body()[msg.body_length()] = '\0';
                msg.encode_header();
                std::cout << "\n Richiesta da inviare al server " << msg.body() << std::endl;
                sendmessage(msg);
                //deseleziono file eliminato
                selectedFile = "";

            } catch (std::exception &e) {
                std::cerr << "Exception: " << e.what() << "\n";
            }
        }
    }

}

void Userpage::updateRecentFiles(QString old, QString newN) {
    if(newN == ""){
        //aggiorniamo dopo una delete

        std::cout<< "\n\nprima della find\nold = "<< old.toStdString();
        QPushButton * b = recent->findChild<QPushButton *>(client_->getUser() + "_|_" + old);
        page->findChild<QVBoxLayout *>("verticalLayout")->removeWidget(b);
        delete b;

    }else{
        //aggiorniamo dopo rename
        recent->findChild<QPushButton *>(client_->getUser() + "_|_" + old)->setText(client_->getUser() +":   "+newN);
        recent->findChild<QPushButton *>(client_->getUser() + "_|_" + old)->setObjectName(client_->getUser() + "_|_" + newN);
    }

}

void Userpage::handleOpenURLbutton() {
    if(!this->lineURL->text().isEmpty()) {
        json j = json{
                {"operation", "validate_invitation"},
                {"invitation_code",  this->lineURL->text().toStdString()}
        };

        //PRENDI I DATI E INVIA A SERVER
        std::string mess = j.dump().c_str();
        message msg;
        msg.body_length(mess.size());
        std::memcpy(msg.body(), mess.data(), msg.body_length());
        msg.body()[msg.body_length()] = '\0';
        msg.encode_header();
        std::cout << "\n Richiesta da inviare al server " << msg.body() << std::endl;
        sendmessage(msg);
    }
}

/*void Userpage::updateFiles() {
    setupRecentFiles();
*/


