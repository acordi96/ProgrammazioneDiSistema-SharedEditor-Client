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
#include <QMenu>
#include <QtCore/QModelIndex>

Userpage::Userpage(QWidget *parent,Client *c):
                    QMainWindow(parent),client_(c)
{
    page = new QWidget(this);
    hLayout = new QHBoxLayout(page);
    setupRecentFiles();
    setupUserinfo();
    hLayout->addWidget(recent);
    hLayout->addWidget(userinfo);
    std::cout << "\n creazione userpage";
    page->setLayout(hLayout);
    setCentralWidget(page);

}

void Userpage::setupRecentFiles(){
    recent = new QWidget();
    recent->setObjectName(QString::fromUtf8("recent_files"));

    QLabel *label = new QLabel(QString::fromUtf8("Recent Files: "),recent);
    label->setGeometry(QRect(10,10,101,17));


    QScrollArea *scrollArea = new QScrollArea(recent);
    scrollArea->setGeometry(QRect(10,60,121,101));
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(QString::fromUtf8("QPushButton {border: 0px;}"
                                                 "QPushButton:hover {background-color: rgb(32, 74, 135)}"));
    QWidget *scrollAreaWidgets = new QWidget(scrollArea);
    QVBoxLayout *verticalLayout = new QVBoxLayout(scrollAreaWidgets);
    scrollAreaWidgets->setStyleSheet(QString::fromUtf8("QPushButton {border: 0px;}"
                                                       "QPushButton:hover {background-color: rgb(32, 74, 135);}"));
    /*
     *  for (int i =0; i< num_Files; i++ ){
     *          button = new QPushButton(scrollAreaWidgets);
     *          button->setObjectName(QString::fromUtf8("button"+i);
     *          button->setText(QString::fromUtf8(nome_file);
     *  }
     *
     * */
    QPushButton *button;
   /* for (int var = 0; var < 8; ++var) {
        button = new QPushButton(scrollAreaWidgets);
        button->setObjectName(QString::fromUtf8("button_")+QString::number(var));
        button->setText(QString::fromUtf8("nomefile.txt"));
        verticalLayout->addWidget(button);
    }*/
    QListIterator<std::string> iter1(client_->files);
    std::cout << "\n prima del while files" << " dimensione " << client_->files.size();

    /*while(iter1.hasNext())*/
    /*for(int i=0;i<client_->files.size();i++)*/
    while(iter1.hasNext()){
        std::string s = iter1.next();
        std::cout <<"\nentrato nel while dei file ";
        button = new QPushButton(scrollAreaWidgets);
        button->setContextMenuPolicy(Qt::CustomContextMenu);
        //QRightClickButton *button = new QRightClickButton(this);
        //connect(button, SIGNAL(rightClicked()), this, SLOT(onRightClicked()));
        std::cout << "\n" << s;
        button->setObjectName(QString::fromStdString(s));
        button->setText(QString::fromStdString(s));
        verticalLayout->addWidget(button);

        connect(button,SIGNAL(clicked()),SLOT(on_button_clicked()));
        connect(button, SIGNAL(customContextMenuRequested(QPoint)),
                SLOT(customMenuRequested(QPoint)));
    }


    scrollAreaWidgets->setLayout(verticalLayout);
    scrollArea->setWidget(scrollAreaWidgets);


    hLayout->addWidget(recent);

}

void Userpage::setupUserinfo(){
    userinfo=new QWidget();
    userinfo->setObjectName(QString::fromUtf8("user_info"));
    userinfo->setStyleSheet(QString::fromUtf8("background-color: rgb(252, 233, 79); "));

    QVBoxLayout *userLayout = new QVBoxLayout(userinfo);
    userLayout->setContentsMargins(0,50,0,200);

    /* will be changed for icon */
    QLabel *label_2 = new QLabel(userinfo);
    label_2->setGeometry(QRect(113,70,67,17));
    label_2->setText(QString::fromUtf8("Icon"));
    label_2->setAlignment(Qt::AlignHCenter);
    userLayout->addWidget(label_2,Qt::AlignHCenter);

    QLabel *label_3 = new QLabel(userinfo);
    label_3->setGeometry(QRect(113,120,111,17));
    label_3->setText(QString::fromUtf8("Connected User"));
    label_3->setAlignment(Qt::AlignHCenter);
    userLayout->addWidget(label_3,Qt::AlignHCenter);

    newFileButton = new QPushButton(userinfo);
    newFileButton->setGeometry(QRect(113,280,89,25));
    newFileButton->setStyleSheet(QString::fromUtf8("QPushButton:hover{color: rgb(32,74,135);}"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/Science-Plus2-Math-icon.png"),QSize(),QIcon::Normal,QIcon::Off);
    newFileButton->setIcon(icon);
    newFileButton->setFlat(true);
    newFileButton->setText(QString::fromUtf8("New File"));
    newFileButton->setObjectName("newFileButton");
    userLayout->addWidget(newFileButton,Qt::AlignHCenter);
    userinfo->setLayout(userLayout);

    hLayout->addWidget(userinfo);

    connect(newFileButton, SIGNAL (released()), this, SLOT (handleButton()));
}

void Userpage::handleButton()
{
    // change the text
        //newFileButton->setText("Example");
    // resize button
        //newFileButton->resize(100,100);
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
        std::cout << "CLICK SUL PULSANTE CREATE " << testo.toStdString() ;
    }
    //modalWindow.exec()
    //ui->stackedWidget->setCurrentIndex(3);
}


void Userpage::sendmessage(message mess) {
    client_->write(mess);
}

void Userpage::onRightClicked() {
    std::cout << "\n click tasto destro \n";
    //creo context menu
    QMenu *menu = new QMenu(this);
   // QMenu *menu = new QMenu(this);
    menu->addAction(new QAction("Action 1", this));
    menu->addAction(new QAction("Action 2", this));
    menu->addAction(new QAction("Action 3", this));


}

void Userpage::customMenuRequested(QPoint pos) {
    //QModelIndex index=button->indexAt(pos);
    std::cout << "\n creazione del menu \n" << "pos = " << pos.x() <<","<<pos.y();
    QMenu *menu=new QMenu(this);
    menu->addAction(new QAction("Open", this));
    menu->addAction(new QAction("Rename", this));
    menu->addAction(new QAction("Delete", this));
    //pos.setX(pos.x());
    pos.setY(pos.y()+100);
    menu->exec(mapToGlobal(pos));
    //menu->popup(pos);
}

void Userpage::on_button_clicked(){
    QObject *sender =  QObject::sender ();
    QString str = sender->objectName();
    std::string s = str.toStdString();
    //QMessageBox::information(0, "Button", sender->objectName());
    std::cout << "\n bottone schiacciato" << s <<"\n";

    try {
        json j = json{
                {"operation","open_file"},
                {"name",s},
                {"username",client_->getUser().toStdString()}
        };

        //PRENDI I DATI E INVIA A SERVER
        std::cout << "\ninvio richiesta apertura file: " << s << " \n username inviato per la creazione del file: " << client_->getUser().toStdString();
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


}




