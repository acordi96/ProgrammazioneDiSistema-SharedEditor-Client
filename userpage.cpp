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

#ifdef Q_OS_MAC
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
         button->setContextMenuPolicy(Qt::CustomContextMenu);
         std::cout << "\n" << p.first << ": " << p.second << "\n";
         button->setObjectName(QString::fromStdString(p.first) + "_|_" + QString::fromStdString(p.second));
         button->setText(QString::fromStdString(p.first)+":   "+QString::fromStdString(p.second));
         button->setFlat(true);
         //button->setContentsMargins(10,0,0,0);
         verticalLayout->addWidget(button);

         connect(button,SIGNAL(clicked()),SLOT(on_fileName_clicked()));
         connect(button, SIGNAL(customContextMenuRequested(QPoint)),
                 SLOT(customMenuRequested(QPoint)));
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


     renameButton->setObjectName(QString::fromUtf8("renameButton"));
     renameButton->setGeometry(QRect(160, 380, 89, 25));
     renameButton->setStyleSheet(QString::fromUtf8("QPushButton#renameButton{\n"
     "border:1px;\n"
     "}\n"
     "QPushButton#renameButton:hover{\n"
     "background-color: rgb(255,0,0);\n"
     "}"));
     renameButton->setFlat(true);


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


     hLayout->addWidget(recent);
    /*
    recent = new QWidget();
    recent->setObjectName(QString::fromUtf8("recent_files"));

    QLabel *label = new QLabel(QString::fromUtf8("Recent Files: "),recent);
    label->setGeometry(QRect(10,10,101,17));


    QScrollArea *scrollArea = new QScrollArea(recent);
    /*QDialog *title = new QDialog();
    title->setGeometry(QRect(30,90,100,130));
    title->setWindowIconText("titolo");//
    scrollArea->setGeometry(QRect(10,60,300,300));
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(QString::fromUtf8("QPushButton {border: 0px;}"
                                                "QPushButton:hover {background-color: rgb(32, 74, 135)}"));
    QWidget *scrollAreaWidgets = new QWidget(scrollArea);
    QVBoxLayout *verticalLayout = new QVBoxLayout(scrollAreaWidgets);
    scrollAreaWidgets->setStyleSheet(QString::fromUtf8("QPushButton {border: 0px;}"
                                                       "QPushButton:hover {background-color: rgb(32, 74, 135);}"));
    QLabel *title = new QLabel(QString::fromUtf8("author    title"),scrollArea);
    title->setGeometry(QRect(0,0,300,20));

    QPushButton *button;

    for (auto p : client_->files){
        //std::string s = iter1.next();
        std::cout <<"\nentrato nel while dei file ";
        button = new QPushButton(scrollAreaWidgets);
        button->setContextMenuPolicy(Qt::CustomContextMenu);
        std::cout << "\n" << p.first << ": " << p.second << "\n";
        button->setObjectName(QString::fromStdString(p.first) + "_|_" + QString::fromStdString(p.second));
        button->setText(QString::fromStdString(p.first)+":   "+QString::fromStdString(p.second));
        //button->setContentsMargins(10,0,0,0);
        verticalLayout->addWidget(button);

        connect(button,SIGNAL(clicked()),SLOT(on_fileName_clicked()));
        connect(button, SIGNAL(customContextMenuRequested(QPoint)),
                SLOT(customMenuRequested(QPoint)));
    }


    scrollAreaWidgets->setLayout(verticalLayout);
    scrollArea->setWidget(scrollAreaWidgets);


    hLayout->addWidget(recent);
*/
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

     QLineEdit *lineURL= new QLineEdit(userinfo);
     lineURL->setObjectName(QString::fromUtf8("lineURL"));
     lineURL->setGeometry(QRect(10,400,211,25));
     lineURL->setPlaceholderText(QString::fromUtf8("Insert URL here..."));

     QPushButton *openURLbutton = new QPushButton("Open URL",userinfo);
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
     /*
    userinfo=new QWidget();
    userinfo->setObjectName(QString::fromUtf8("user_info"));
    userinfo->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255); "));

    QVBoxLayout *userLayout = new QVBoxLayout(userinfo);
    userLayout->setContentsMargins(0,50,0,200);

    //will be changed for icon
    QLabel *label_2 = new QLabel(userinfo);
    label_2->setGeometry(QRect(113,70,67,17));
    label_2->setText(QString::fromUtf8("Icon"));
    label_2->setAlignment(Qt::AlignHCenter);
    userLayout->addWidget(label_2,Qt::AlignHCenter);

    QLabel *label_3 = new QLabel(userinfo);
    label_3->setGeometry(QRect(113,120,111,17));
    label_3->setText(client_->getUser());
    label_3->setAlignment(Qt::AlignHCenter);
    userLayout->addWidget(label_3,Qt::AlignHCenter);

    newFileButton = new QPushButton(userinfo);
    newFileButton->setGeometry(QRect(113,280,89,25));
    newFileButton->setStyleSheet(QString::fromUtf8("QPushButton:hover{color: rgb(32,74,135);}"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Science-Plus2-Math-icon.png"),QSize(),QIcon::Normal,QIcon::Off);
    newFileButton->setIcon(icon);
    newFileButton->setFlat(true);
    newFileButton->setText(QString::fromUtf8("New File"));
    newFileButton->setObjectName("newFileButton");
    userLayout->addWidget(newFileButton,Qt::AlignHCenter);
    userinfo->setLayout(userLayout);

    hLayout->addWidget(userinfo);
*/

     connect(newFileButton, SIGNAL (released()), this, SLOT (handleNewFileButton()));
     connect(modifyIcon,&QPushButton::clicked,this,&Userpage::iconSelector);
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
    //std::string s = QObject::sender()->objectName().toStdString();
    std::cout << "\n Premuto bottone " << s <<std::endl;
    std::string username;
    std::string name;
    for(int i = 0; i < s.length(); i++) {
        if(s[i] == '_' && s[i+1] == '|' && s[i+2] == '_') { //parse button name (username+"_|_"+name)
            i += 3;
            while(s[i] != '\0')
                name += s[i++];
            break;
        }
        username += s[i];
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
        std::cout << "\ninvio richiesta apertura file: " << s << " \n username inviato per la creazione del file: " << client_->getUser().toStdString();
        std::string mess = j.dump().c_str();
        message msg;
        msg.body_length(mess.size());
        std::memcpy(msg.body(), mess.data(), msg.body_length());
        msg.body()[msg.body_length()] = '\0';
        msg.encode_header();
        std::cout <<"\n Richiesta da inviare al server "<< msg.body() << std::endl;
        sendmessage(msg);

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }


}

void Userpage::openFile(){
    std::cout << "\napri file\n";

    //QMessageBox::information(0, "Button", sender->objectName());
    std::cout << "\n bottone schiacciato: " << fileName <<"\n";
    on_fileName_clicked(1);


}

void Userpage::renameFile(){
    std::cout<<"\nrinomina file";

    //controllare se l'utente ha il permesso per rinominare il file
    std::string name;
    std::string username;

    for(int i = 0; i < fileName.length(); i++) {
        if(fileName[i] == '_' && fileName[i+1] == '|' && fileName[i+2] == '_') { //parse button name (username+"_|_"+name)
            i += 3;
            while(fileName[i] != '\0')
                name += fileName[i++];
            break;
        }
        username += fileName[i];
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

            } catch (std::exception &e) {
                std::cerr << "Exception: " << e.what() << "\n";
            }
        }
    }

}

void Userpage::deleteFile(){
    std::cout<<"\ncancella file";
    std::string name;
    std::string username;

    for(int i = 0; i < fileName.length(); i++) {
        if(fileName[i] == '_' && fileName[i+1] == '|' && fileName[i+2] == '_') { //parse button name (username+"_|_"+name)
            i += 3;
            while(fileName[i] != '\0')
                name += fileName[i++];
            break;
        }
        username += fileName[i];
    }
    if(username != client_->getUser().toStdString()){
        //l'utente non ha il permesso di rinominare il file
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

        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << "\n";
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

/*void Userpage::updateFiles() {
    setupRecentFiles();
}*/


