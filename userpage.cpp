#include "Headers/userpage.h"
#include "Headers/Client.h"
#include "Headers/stacked.h"

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

    page->setLayout(hLayout);

    setCentralWidget(page);
    QObject::connect(client_, &Client::updateFile, this, &Userpage::updateRecentFiles);
    //QObject::connect(page, &stacked::updateRecentFiles, this, &Userpage::updateFiles);
}

void Userpage::setupRecentFiles(){
    recent = new QWidget(page);
    recent->setObjectName(QString::fromUtf8("Recent Files"));
    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    recent->setMinimumSize(QSize(500,580));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(recent->sizePolicy().hasHeightForWidth());
    recent->setSizePolicy(sizePolicy1);
    recent->setStyleSheet(QString::fromUtf8("background-color:rgb(255,255,255)"));

    QHBoxLayout *hLayout4 = new QHBoxLayout(recent);
    hLayout4->setObjectName(QString::fromUtf8("hLayout4"));

    QVBoxLayout *vLayout3 = new QVBoxLayout();
    vLayout3->setObjectName(QString::fromUtf8("vLayout3"));

    QHBoxLayout *hLayout3 = new QHBoxLayout();
    hLayout3->setObjectName(QString::fromUtf8("hLayout3"));

    QLabel *recentLabel = new QLabel("Recent Files",recent);
    recentLabel->setObjectName(QString::fromUtf8("recentLabel"));
    //recentLabel->setGeometry(QRect(10, 0, 281, 31));
    recentLabel->setStyleSheet(QString::fromUtf8("font: 75 22pt \"Sawasdee Bold\";"));
    recentLabel->setAlignment(Qt::AlignCenter);

    hLayout3->addWidget(recentLabel);
    QSpacerItem *hSpacer3 = new QSpacerItem(40,20,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    hLayout3->addItem(hSpacer3);

    QPushButton *upLogoutButton = new QPushButton(recent);
    upLogoutButton->setObjectName(QString::fromUtf8("upLogoutButton"));
    //upLogoutButton->setGeometry(QRect(400,0,40,30));

    QIcon logoutIcon;
    logoutIcon.addFile(rsrcPath+QString::fromUtf8("/logout.png"),QSize(),QIcon::Normal,QIcon::On);
    upLogoutButton->setIcon(logoutIcon);
    upLogoutButton->setIconSize(QSize(32,32));
    upLogoutButton->setFlat(true);

    connect(upLogoutButton,&QPushButton::clicked,this,[=](){
       requestLogout();
       emit this->upLogout();
    });

    hLayout3->addWidget(upLogoutButton);
    vLayout3->addLayout(hLayout3);

    QScrollArea *scrollArea = new QScrollArea(recent);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    //scrollArea->setGeometry(QRect(10,60,281,300));
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setLineWidth(0);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);

    QWidget *scrollAreaWidgets = new QWidget(scrollArea);
    scrollAreaWidgets->setObjectName(QString::fromUtf8("scrollAreaWidgets"));
    QVBoxLayout *verticalLayout = new QVBoxLayout(scrollAreaWidgets);
    verticalLayout->setObjectName("verticalLayout");
    QLabel *title = new QLabel(QString::fromUtf8("author    title"),recent);
    title->setGeometry(QRect(10,35,281,20));
    title->setAlignment(Qt::AlignCenter);

    vLayout3->addWidget(title);

    QPushButton *button;

     for (auto p : client_->files){
         button = new QPushButton(scrollAreaWidgets);
         button->setObjectName(QString::fromStdString(generateFileButton(p.first.first, p.first.second)));
         button->setText("("+QString::fromStdString(p.first.first)+"): "+QString::fromStdString(p.first.second));
         button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                 "border:1px;\n"
                                                 "background-color: blue;\n"
                                                 "}"));

         button->setFlat(true);
         //button->setContentsMargins(10,0,0,0);
         verticalLayout->addWidget(button);

         connect(button,SIGNAL(clicked()),SLOT(on_fileName_clicked()));
         /*connect(button, SIGNAL(customContextMenuRequested(QPoint)),
                 SLOT(customMenuRequested(QPoint)));*/
     }

    scrollAreaWidgets->setLayout(verticalLayout);
    scrollArea->setWidget(scrollAreaWidgets);

    vLayout3->addWidget(scrollArea);

    QSpacerItem *vSpacer= new QSpacerItem(20,40,QSizePolicy::Minimum,QSizePolicy::Fixed);
    vLayout3->addItem(vSpacer);

    QGridLayout *grid = new QGridLayout();
    grid->setObjectName(QString::fromUtf8("grid"));
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(18);
    grid->setContentsMargins(6,6,6,6);

    QPushButton *openButton = new QPushButton("Open",recent);
    QPushButton *renameButton = new QPushButton("Rename",recent);
    QPushButton *deleteButton = new QPushButton("Delete",recent);
    QPushButton *inviteButton = new QPushButton("Invite",recent);

    openButton->setObjectName(QString::fromUtf8("openButton"));
    openButton->setGeometry(QRect(40,380,89,25));
    openButton->setGeometry(QRect(40, 380, 89, 25));
    openButton->setStyleSheet(QString::fromUtf8("QPushButton#openButton{\n"
                                                "background-color:")+
                              client_->getColor()+
                              QString::fromUtf8(";border:1px;\n"
                                                "}\n"
                                                "QPushButton#openButton:hover{\n"
                                                "background-color: rgb(255,0,0);\n"
                                                "}"));
    openButton->setFlat(true);

    connect(openButton,SIGNAL(clicked()),SLOT(on_openButton_clicked()));
    connect(inviteButton,SIGNAL(clicked()),SLOT(on_inviteButton_clicked()));
    grid->addWidget(openButton,0,0,1,1);

    renameButton->setObjectName(QString::fromUtf8("renameButton"));
    renameButton->setGeometry(QRect(160, 380, 89, 25));
    renameButton->setStyleSheet(QString::fromUtf8("QPushButton#renameButton{\n"
                                                  "background-color:")+
                                client_->getColor()+
                                QString::fromUtf8(";border:1px;\n"
                                                  "}\n"
                                                  "QPushButton#renameButton:hover{\n"
                                                  "background-color: rgb(255,0,0);\n"
                                                  "}"));
    renameButton->setFlat(true);

    grid->addWidget(renameButton,0,1,1,1);
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
                                                  "background-color:")+
                                client_->getColor()+
                                QString::fromUtf8(";border:1px;\n"
                                                  "}\n"
                                                  "QPushButton#deleteButton:hover{\n"
                                                  "background-color: rgb(255,0,0);\n"
                                                  "}"));
    deleteButton->setFlat(true);

     connect(deleteButton,SIGNAL(clicked()),SLOT(on_deleteButton_clicked()));

     hLayout->addWidget(recent);

    grid->addWidget(deleteButton,1,0,1,1);

    inviteButton->setObjectName(QString::fromUtf8("inviteButton"));
    inviteButton->setGeometry(QRect(40, 430, 89, 25));
    inviteButton->setStyleSheet(QString::fromUtf8("QPushButton#inviteButton{\n"
                                                  "background-color:")+
                                client_->getColor()+
                                QString::fromUtf8(";border:1px;\n"
                                                  "}\n"
                                                  "QPushButton#inviteButton:hover{background-color: rgb(255,0,0);}"));
    inviteButton->setFlat(true);

    grid->addWidget(inviteButton,1,1,1,1);

    vLayout3->addLayout(grid);

    QSpacerItem *vSpacer2 = new QSpacerItem(20,40,QSizePolicy::Minimum,QSizePolicy::Minimum);
    vLayout3->addItem(vSpacer2);

    QSpacerItem *hSpacer4 = new QSpacerItem(30,20,QSizePolicy::Fixed,QSizePolicy::Minimum);
    hLayout4->addItem(hSpacer4);
    hLayout4->addLayout(vLayout3);

//    hLayout->addWidget(recent);
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
     userinfo->setStyleSheet(QString::fromUtf8("background-color:rgb(255,255,255);"));


     QWidget *user_image = new QWidget(userinfo);
     user_image->setGeometry(QRect(80, 10, 61, 51));
     user_image->setStyleSheet(QString::fromUtf8("image: url(:/images/photo_2020-09-22_17-58-05.jpg);"));

     QLabel *userpageLabel = new QLabel("Userpage",userinfo);
     userpageLabel->setObjectName(QString::fromUtf8("userpageLabel"));
     userpageLabel->setGeometry(QRect(160, 10, 221, 61));
     userpageLabel->setStyleSheet(QString::fromUtf8("font: 75 25pt \"Sawasdee\";"));
     userpageLabel->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

     myIcon = new QWidget(userinfo);
     myIcon->setObjectName(QString::fromUtf8("myIcon"));
     myIcon->setGeometry(QRect(80,120,101,91));
     myIcon->setStyleSheet(QString::fromUtf8("background-color:")+client_->getColor());


     QLabel *usernameLabel = new QLabel(client_->getUser(),userinfo);
     usernameLabel->setObjectName(QString::fromUtf8("usernameButton"));
     usernameLabel->setGeometry(QRect(230, 110, 171, 71));
     usernameLabel->setFont(QFont(QString::fromUtf8("Sawasdee"),25,60,false));
     usernameLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);


     //QLabel *email_lab = new QLabel(client_->getEmail(),userinfo);
     QLabel *email_lab = new QLabel("email",userinfo);
     email_lab->setObjectName(QString::fromUtf8("email_lab"));
     email_lab->setGeometry(QRect(230, 180, 91, 31));
     email_lab->setStyleSheet(QString::fromUtf8("font: 14pt \"Sawasdee\";"));
     email_lab->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

     QPushButton *newFileButton = new QPushButton("New File",userinfo);
     newFileButton->setObjectName(QString::fromUtf8("newFileButton"));
     newFileButton->setGeometry(QRect(90, 320, 121, 31));
     newFileButton->setStyleSheet(QString::fromUtf8("QPushButton#newFileButton{\n"
                                                    "background-color:")+
                                  client_->getColor()+
                                  QString::fromUtf8(";border:1px;\n"
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
     lineURL->setGeometry(QRect(30,480,211,25));
     lineURL->setPlaceholderText(QString::fromUtf8("Insert URL here..."));

     QPushButton *openURLbutton = new QPushButton("Accept",userinfo);
     openURLbutton->setObjectName(QString::fromUtf8("openURLbutton"));
     openURLbutton->setGeometry(QRect(260,480,71,25));
     openURLbutton->setStyleSheet(QString::fromUtf8("QPushButton#openURLbutton{\n"
                                                    "background-color:")+
                                  client_->getColor()+
                                  QString::fromUtf8(";border:1px;\n"
                                                    "}\n"
                                                    "QPushButton#openURLbutton:hover{\n"
                                                    "background-color: rgb(255,0,0);\n"
                                                    "}"));

     userLayout->addWidget(userinfo);
     hLayout->addLayout(userLayout);

     connect(newFileButton, SIGNAL (released()), this, SLOT (handleNewFileButton()));
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

    std::string s;
    if(i==0) {
        QObject *sender = QObject::sender();
        QString str = sender->objectName();
        s = str.toStdString();
        //QMessageBox::information(0, "Button", sender->objectName());
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
        if(selectedFile == s) {
            selectedFile = "";
            return;
        }
    }
    selectedFile = s;
    QPushButton *b = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
    b->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                            "border:1px;\n"
                                            "background-color: red;\n"
                                            "}"));


}

void Userpage::on_openButton_clicked(){

    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Hey!"),
                tr("Select a file") );
        return;
    }

    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;
    try {
        json j = json{
                {"operation","open_file"},
                {"name", filename},
                {"username",owner}
        };
        client_->setFileName(QString::fromStdString(filename));
        //PRENDI I DATI E INVIA A SERVER
        std::string mess = j.dump().c_str();
        message msg;
        msg.body_length(mess.size());
        std::memcpy(msg.body(), mess.data(), msg.body_length());
        msg.body()[msg.body_length()] = '\0';
        msg.encode_header();
        std::cout <<"Richiesta da inviare al server "<< msg.body() << std::endl;
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
    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Hey!"),
                tr("Select a file") );
        return;
    }

    //controllare se l'utente ha il permesso per rinominare il file
    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;
    if(owner != client_->getUser().toStdString()){
        //l'utente non ha il permesso di rinominare il file

        QMessageBox::information(
                this,
                tr("Hey!"),
                tr("Only who created the file can rename it") );
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
                        {"old_name",  filename},
                        {"new_name",  modalWindow.textValue().toStdString()},
                        {"username",  client_->getUser().toStdString()}
                };

                //PRENDI I DATI E INVIA A SERVER
                std::string mess = j.dump().c_str();
                message msg;
                msg.body_length(mess.size());
                std::memcpy(msg.body(), mess.data(), msg.body_length());
                msg.body()[msg.body_length()] = '\0';
                msg.encode_header();
                std::cout << "Richiesta da inviare al server " << msg.body() << std::endl;
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

    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Hey!"),
                tr("Select a file") );
        return;
    }
    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;

    if(owner == client_->getUser().toStdString()) { //sei l'owner
        QMessageBox msgBox;
        msgBox.setText("Do you really want to delete the selected file?");
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
                try {
                    json j = json{
                            {"operation", "delete_file"},
                            {"name",  filename},
                            {"username",  client_->getUser().toStdString()},
                            {"owner", owner}
                    };

                    //PRENDI I DATI E INVIA A SERVER
                    std::string mess = j.dump().c_str();
                    message msg;
                    msg.body_length(mess.size());
                    std::memcpy(msg.body(), mess.data(), msg.body_length());
                    msg.body()[msg.body_length()] = '\0';
                    msg.encode_header();
                    std::cout << "Richiesta da inviare al server " << msg.body() << std::endl;
                    sendmessage(msg);
                    //deseleziono file eliminato
                    selectedFile = "";

                } catch (std::exception &e) {
                    std::cerr << "Exception: " << e.what() << "\n";
                }

        }

    } else { //non sei l'owner: rimuovi invito
        QMessageBox msgBox;
        msgBox.setText("Do you really want to remove the file invitation?");
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
            try {
                json j = json{
                        {"operation", "delete_file"},
                        {"name",  filename},
                        {"username",  client_->getUser().toStdString()},
                        {"owner", owner}
                };

                //PRENDI I DATI E INVIA A SERVER
                std::string mess = j.dump().c_str();
                message msg;
                msg.body_length(mess.size());
                std::memcpy(msg.body(), mess.data(), msg.body_length());
                msg.body()[msg.body_length()] = '\0';
                msg.encode_header();
                std::cout << "Richiesta da inviare al server " << msg.body() << std::endl;
                sendmessage(msg);
                //deseleziono file eliminato
                selectedFile = "";

            } catch (std::exception &e) {
                std::cerr << "Exception: " << e.what() << "\n";
            }

        }
    }




}

void Userpage::updateRecentFiles(QString old, QString newN, QString owner, QString request) {
    if(request == "add_new_file") { //add button
        std::pair<std::string, std::string> parsed = parseFileButton(newN.toStdString());
        std::string owner = parsed.first;
        std::string filename = parsed.second;
        std::string invitation = filename.substr(filename.size() - 15, filename.size());
        filename.erase(filename.size() - 15);

        QPushButton *button;
        auto scrollAreaWidgets = page->findChild<QWidget *>("scrollAreaWidgets");
        button = new QPushButton(scrollAreaWidgets);
        //button->setContextMenuPolicy(Qt::CustomContextMenu);
        //std::cout << "\n" << p.first << ": " << p.second << "\n";
        button->setObjectName(QString::fromStdString(generateFileButton(owner, filename)));
        button->setText("("+QString::fromStdString(owner)+"): "+QString::fromStdString(filename));
        button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "border:1px;\n"
                                                "background-color: blue;\n"
                                                "}"));
        button->setFlat(true);
        page->findChild<QVBoxLayout *>("verticalLayout")->addWidget(button);
        connect(button,SIGNAL(clicked()),SLOT(on_fileName_clicked()));

        return;
    }
    if(request == "add_new_file_invitation") {
        std::pair<std::string, std::string> parsed = parseFileButton(newN.toStdString());
        std::string owner = parsed.first;
        std::string filename = parsed.second;

        QPushButton *button;
        auto scrollAreaWidgets = page->findChild<QWidget *>("scrollAreaWidgets");
        button = new QPushButton(scrollAreaWidgets);
        //button->setContextMenuPolicy(Qt::CustomContextMenu);
        //std::cout << "\n" << p.first << ": " << p.second << "\n";
        button->setObjectName(QString::fromStdString(generateFileButton(owner, filename)));
        button->setText("("+QString::fromStdString(owner)+"): "+QString::fromStdString(filename));
        button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "border:1px;\n"
                                                "background-color: blue;\n"
                                                "}"));
        button->setFlat(true);
        page->findChild<QVBoxLayout *>("verticalLayout")->addWidget(button);
        connect(button,SIGNAL(clicked()),SLOT(on_fileName_clicked()));

        return;
    }
    if(request == "delete_file"){ //aggiorniamo dopo una delete
        QPushButton * b = recent->findChild<QPushButton *>(QString::fromStdString(generateFileButton(newN.toStdString(), old.toStdString())));
        page->findChild<QVBoxLayout *>("verticalLayout")->removeWidget(b);
        delete b;
    }
    if(request == "rename_file"){ //aggiorniamo dopo rename
        recent->findChild<QPushButton *>(QString::fromStdString(generateFileButton(owner.toStdString(), old.toStdString())))->setText("("+owner +"): "+newN);
        recent->findChild<QPushButton *>(QString::fromStdString(generateFileButton(owner.toStdString(), old.toStdString())))->setObjectName(QString::fromStdString(generateFileButton(owner.toStdString(), newN.toStdString())));
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
        std::cout << "Richiesta da inviare al server " << msg.body() << std::endl;
        sendmessage(msg);
    }
}

void Userpage::on_inviteButton_clicked() {
    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;
    if(selectedFile==""){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Hey!"),
                tr("Select a file") );
        return;
    }
    if(owner != client_->getUser().toStdString()){
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Hey!"),
                tr("You must be the owner in order to invite") );
        return;
    }
    const char * code;
    for(auto &iter : client_->files) {
        if(iter.first.second == filename && iter.first.first == client_->getUser().toStdString()) {
            code = iter.second.c_str();
            break;
        }
    }
    QMessageBox::information(
            this,
            tr("Codice Invito:"),
            tr(code));
}

std::pair<std::string, std::string> Userpage::parseFileButton(const std::string& button) {
    int dimOwner = button[0];
    dimOwner -= 48;
    std::string owner;
    std::string filename;
    int i;
    if(button[1] != '_') {
        int j = button[1];
        j -= 48;
        dimOwner *= 10;
        dimOwner += j;
        for(i = 3; i < dimOwner + 3; i++) {
            owner.push_back(button[i]);
        }
        for(; i < button.size(); i++) {
            filename.push_back(button[i]);
        }
    } else {
        for(i = 2; i < dimOwner + 2; i++) {
            owner.push_back(button[i]);
        }
        for(; i < button.size(); i++) {
            filename.push_back(button[i]);
        }
    }
    return std::pair<std::string, std::string>(owner, filename);
}

std::string Userpage::generateFileButton(const std::string& owner, const std::string& filename) {
    return std::to_string(owner.size()) + '_' + owner + filename;
}
