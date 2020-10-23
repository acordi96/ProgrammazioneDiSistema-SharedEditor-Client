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
#include <QResizeEvent>
#include <QtCore/QModelIndex>
#include <QtWidgets/QMessageBox>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>

#ifdef Q_OS_MACOS
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

Userpage::Userpage(QWidget *parent, Client *c) :
        QMainWindow(parent), client_(c) {
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
}

void Userpage::setupUserinfo() {
    userinfo = new QWidget(page);
    userinfo->setObjectName(QString::fromUtf8("Userinfo"));
    userinfo->setStyleSheet(QString::fromUtf8("background-color:rgb(255,255,255)"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(userinfo->sizePolicy().hasHeightForWidth());
    userinfo->setSizePolicy(sizePolicy);

    //layout più esterno -> i layout servono per usare gli spacer
    QHBoxLayout *hrzLayout4 = new QHBoxLayout(userinfo);
    hrzLayout4->setObjectName(QString::fromUtf8("hrzLayout4"));

    //vertical layout -> ci andranno tutti i widget
    QVBoxLayout *userVLayout3 = new QVBoxLayout();
    userVLayout3->setObjectName(QString::fromUtf8("userVLayout3"));
    QSpacerItem *topSpacer = new QSpacerItem(20, 25, QSizePolicy::Fixed, QSizePolicy::Fixed);
    userVLayout3->addItem(topSpacer);

    //user image + user Userpage label
    QHBoxLayout *hrzLayout3 = new QHBoxLayout();
    hrzLayout3->setObjectName(QString::fromUtf8("hrzLayout3"));
    hrzLayout3->setSpacing(1);
/***********************************TITLE & ICON *********************************************************/
    QWidget *user_image = new QWidget(userinfo);
    user_image->setStyleSheet(QString::fromUtf8("image: url(") + rsrcPath + QString::fromUtf8("/user.png);"));
    hrzLayout3->addWidget(user_image);

    QLabel *userpageLabel = new QLabel("Userpage", userinfo);
    userpageLabel->setObjectName(QString::fromUtf8("userpageLabel"));
    userpageLabel->setStyleSheet(QString::fromUtf8("font: 75 25pt \"Sawasdee Bold\";"));
    userpageLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    hrzLayout3->addWidget(userpageLabel);

    QSpacerItem *lay3spacer = new QSpacerItem(60, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    hrzLayout3->addSpacerItem(lay3spacer);

    userVLayout3->addLayout(hrzLayout3);

    QSpacerItem *vSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    userVLayout3->addItem(vSpacer);
/************************************** ICON ***********************************************/

    QHBoxLayout *iconLayout = new QHBoxLayout();
    iconLayout->setObjectName(QString::fromUtf8("iconSpace"));

    QWidget *iconSpace = new QWidget(userinfo);

    myIcon = new QWidget(iconSpace);
    myIcon->setObjectName(QString::fromUtf8("myIcon"));
    myIcon->setGeometry(QRect(-10, 10, 100, 100));
    myIcon->setStyleSheet(QString::fromUtf8("background-color:") + client_->getColor());

    usrLetters = new QLabel(client_->getUser()[0].toUpper(), myIcon);
    usrLetters->setObjectName(QString::fromUtf8("usrLetters"));
    usrLetters->setStyleSheet("font: 15pt \"Sawasdee Bold\"");
    usrLetters->setGeometry(QRect(15, 0, 80, 80));

    if (colorIsDark(client_->getColor())) {
        usrLetters->setStyleSheet(QString::fromUtf8("color:rgb(243,243,243);font:36pt \"Sawasdee\";"));
    } else {
        usrLetters->setStyleSheet(QString::fromUtf8("color:rgb(0,0,0);font:36pt \"Sawasdee\";"));
    }

    usrLetters->setAlignment(Qt::AlignCenter);

    QSpacerItem *iconSpacer = new QSpacerItem(120, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    iconLayout->addItem(iconSpacer);

    iconLayout->addWidget(iconSpace);
    QSpacerItem *iconSpacer2 = new QSpacerItem(90, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    iconLayout->addItem(iconSpacer2);

    userVLayout3->addLayout(iconLayout);
/**************************************EDIT BUTTON********************************************************/
    QSpacerItem *editVSpacer = new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    userVLayout3->addItem(editVSpacer);

    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->setObjectName(QString::fromUtf8("editLayout"));

    QSpacerItem *editSpacer = new QSpacerItem(100, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    editLayout->addItem(editSpacer);

    QPushButton *goEdit = new QPushButton("Edit Profile", userinfo);
    goEdit->setObjectName(QString::fromUtf8("editProfile"));
    goEdit->setStyleSheet(QString::fromUtf8("QPushButton#editProfile{\n"
                                            "background-color:#84ACD7;\n"
                                            "border:1px;\n"
                                            "border-radius:5px;\n"
                                            "color:#FFFFFF;\n"
                                            "font: 75 14pt \"Sawasdee Bold\";\n"
                                            "}\n"
                                            "QPushButton#editProfile:hover{\n"
                                            "background-color:#68DFBB;\n"
                                            "color:#FFFFFF;\n"
                                            "border-radius:5px;\n"
                                            "}"));

    //add Icon
    QIcon iconEdit;
    iconEdit.addFile(rsrcPath + QString::fromUtf8("/edit.png"), QSize(), QIcon::Normal, QIcon::On);
    goEdit->setIcon(iconEdit);
    goEdit->setIconSize(QSize(16, 16));
    goEdit->setFlat(true);

    connect(goEdit, &QPushButton::clicked, this, [=]() {
        emit this->goToEdit();
    });
    editLayout->addWidget(goEdit);

    QSpacerItem *editSpacer2 = new QSpacerItem(100, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    editLayout->addItem(editSpacer2);

    userVLayout3->addLayout(editLayout);
/**************************************** USER&EMAIL LABELS  ********************************************/
    QSpacerItem *iconVSpacer2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    userVLayout3->addItem(iconVSpacer2);

    QVBoxLayout *labelsLayout = new QVBoxLayout();
    labelsLayout->setSpacing(1);

    QLabel *usernameLabel = new QLabel(client_->getUser(), userinfo);
    usernameLabel->setObjectName(QString::fromUtf8("usernameButton"));
    usernameLabel->setFont(QFont(QString::fromUtf8("Sawasdee Bold"), 25, 60, false));
    usernameLabel->setAlignment(Qt::AlignLeading | Qt::AlignCenter | Qt::AlignVCenter);
    labelsLayout->addWidget(usernameLabel);

    email_lab = new QLabel(client_->getEmail(), userinfo);
    email_lab->setObjectName(QString::fromUtf8("email_lab"));
    email_lab->setStyleSheet(QString::fromUtf8("font: 13pt \"Sawasdee\";"));
    email_lab->setAlignment(Qt::AlignLeading | Qt::AlignCenter | Qt::AlignVCenter);
    labelsLayout->addWidget(email_lab);

    //icon_userLayout->addLayout(labelsLayout);

    QSpacerItem *vSpacer5 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    userVLayout3->addItem(vSpacer5);

    userVLayout3->addLayout(labelsLayout);

    QSpacerItem *vSpacer2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    userVLayout3->addItem(vSpacer2);
/************************************ NEW FILE BUTTON **********************************************/
    QHBoxLayout *newFBtnLayout = new QHBoxLayout();
    QSpacerItem *hSpacer = new QSpacerItem(100, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    newFBtnLayout->addItem(hSpacer);

    QPushButton *newFileButton = new QPushButton("New File", userinfo);
    newFileButton->setObjectName(QString::fromUtf8("newFileButton"));
    newFileButton->setGeometry(QRect(160, 345, 121, 31));
    newFileButton->setStyleSheet(QString::fromUtf8("QPushButton#newFileButton{\n"
                                                   "background-color:#84ACD7;\n"
                                                   "border:1px;\n"
                                                   "border-radius:5px;\n"
                                                   "color:#FFFFFF;\n"
                                                   "font: 75 14pt \"Sawasdee Bold\";\n"
                                                   "}\n"
                                                   "QPushButton#newFileButton:hover{\n"
                                                   "background-color:#68DFBB;\n"
                                                   "border-radius:5px;\n"
                                                   "}"));

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/Science-Plus2-Math-icon.png"), QSize(), QIcon::Normal, QIcon::On);
    newFileButton->setIcon(icon);
    newFileButton->setIconSize(QSize(32, 32));
    newFileButton->setFlat(true);

    connect(newFileButton, &QPushButton::clicked, this, &Userpage::handleNewFileButton);

    newFBtnLayout->addWidget(newFileButton);

    QSpacerItem *hSpacer2 = new QSpacerItem(100, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    newFBtnLayout->addItem(hSpacer2);

    userVLayout3->addLayout(newFBtnLayout);

    QSpacerItem *vSpacer3 = new QSpacerItem(20, 60, QSizePolicy::Minimum, QSizePolicy::Fixed);
    userVLayout3->addItem(vSpacer3);
/*************************************** URL INVITATION ************************************************/
    QHBoxLayout *urlLayout = new QHBoxLayout();

    lineURL = new QLineEdit(userinfo);
    lineURL->setObjectName(QString::fromUtf8("lineURL"));
    lineURL->setPlaceholderText(QString::fromUtf8("Insert Invitation Code Here..."));
    urlLayout->addWidget(lineURL);

    openURLbutton = new QPushButton("Accept", userinfo);
    openURLbutton->setObjectName(QString::fromUtf8("openURLbutton"));
    openURLbutton->setStyleSheet(QString::fromUtf8("QPushButton#openURLbutton{\n"
                                                   "background-color:#84ACD7;\n"
                                                   "border:1px;\n"
                                                   "border-radius:5px;\n"
                                                   "color:#FFFFFF;\n"
                                                   "font: 75 14pt \"Sawasdee Bold\";\n"
                                                   "}\n"
                                                   "QPushButton#openURLbutton:hover{\n"
                                                   "background-color:#68DFBB;\n"
                                                   "border-radius:5px;\n"
                                                   "}"));

    connect(openURLbutton, &QPushButton::clicked, this, &Userpage::handleOpenURLbutton);
    QSpacerItem *urlSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    urlLayout->addItem(urlSpacer);
    urlLayout->addWidget(openURLbutton);

    userVLayout3->addLayout(urlLayout);

    QSpacerItem *vSpacer4 = new QSpacerItem(20, 60, QSizePolicy::Minimum, QSizePolicy::Fixed);
    userVLayout3->addItem(vSpacer4);

    QSpacerItem *hSpacer4 = new QSpacerItem(10, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    hrzLayout4->addItem(hSpacer4);

    hrzLayout4->addLayout(userVLayout3);

    QSpacerItem *hSpacer5 = new QSpacerItem(10, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    hrzLayout4->addItem(hSpacer5);

}

void Userpage::setupRecentFiles() {
    recent = new QWidget(page);
    recent->setObjectName(QString::fromUtf8("Files"));
    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    recent->setMinimumSize(QSize(500, 580));
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

    QLabel *recentLabel = new QLabel("Files", recent);
    recentLabel->setObjectName(QString::fromUtf8("recentLabel"));
    recentLabel->setStyleSheet(QString::fromUtf8("font: 75 25pt \"Sawasdee Bold\";"));
    recentLabel->setAlignment(Qt::AlignCenter);

    hLayout3->addWidget(recentLabel);
    QSpacerItem *hSpacer3 = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    hLayout3->addItem(hSpacer3);

    QPushButton *upLogoutButton = new QPushButton(recent);
    upLogoutButton->setObjectName(QString::fromUtf8("upLogoutButton"));
    upLogoutButton->setToolTip(QString::fromUtf8("Logout"));
    upLogoutButton->setStyleSheet(QString::fromUtf8("QPushButton#upLogoutButton:hover{\n"
                                                    "background-color:#E6E7E8;\n"
                                                    "border:1px;\n"
                                                    "border-radius:5px;"
                                                    "}"));
    
    QIcon logoutIcon;
    logoutIcon.addFile(rsrcPath + QString::fromUtf8("/logout.png"), QSize(), QIcon::Normal, QIcon::On);
    upLogoutButton->setIcon(logoutIcon);
    upLogoutButton->setIconSize(QSize(32, 32));
    upLogoutButton->setFlat(true);

    connect(upLogoutButton, &QPushButton::clicked, this, [=]() {
        requestLogout();
        emit this->upLogout();
    });

    hLayout3->addWidget(upLogoutButton);
    QSpacerItem *vSpacer1 = new QSpacerItem(20, 25, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vLayout3->addItem(vSpacer1);
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
    QLabel *title = new QLabel(QString::fromUtf8("author    title"), recent);
    title->setGeometry(QRect(10, 35, 281, 20));
    title->setStyleSheet(QString::fromUtf8("font: 14pt \"Sawasdee Bold\";"));
    title->setAlignment(Qt::AlignCenter);

    vLayout3->addWidget(title);

    QPushButton *button;

    for (auto p : client_->getFiles()) {
        button = new QPushButton(scrollAreaWidgets);
        button->setObjectName(QString::fromStdString(generateFileButton(p.first.first, p.first.second)));
        button->setText("(" + QString::fromStdString(p.first.first) + "): " + QString::fromStdString(p.first.second));
        button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "border:1px;\n"
                                                "border-radius:5px;\n"
                                                "color:#FFFFFF;\n"
                                                "background-color:#84ACD7;\n"
                                                "font: 75 14pt \"Sawasdee Bold\";\n"
                                                "}"));

        button->setFlat(true);

        verticalLayout->addWidget(button);

        connect(button, SIGNAL(clicked()), SLOT(on_fileName_clicked()));
    }

    scrollAreaWidgets->setLayout(verticalLayout);
    scrollArea->setWidget(scrollAreaWidgets);

    vLayout3->addWidget(scrollArea);

    QSpacerItem *vSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);
    vLayout3->addItem(vSpacer);

    QGridLayout *grid = new QGridLayout();
    grid->setObjectName(QString::fromUtf8("grid"));
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(18);
    grid->setContentsMargins(6, 6, 6, 6);

    QPushButton *openButton = new QPushButton("Open", recent);
    QPushButton *renameButton = new QPushButton("Rename", recent);
    QPushButton *deleteButton = new QPushButton("Delete", recent);
    QPushButton *inviteButton = new QPushButton("Invite", recent);

    openButton->setObjectName(QString::fromUtf8("openButton"));
    openButton->setGeometry(QRect(40, 380, 89, 25));
    openButton->setGeometry(QRect(40, 380, 89, 25));
    openButton->setStyleSheet(QString::fromUtf8("QPushButton#openButton{\n"
                                                "background-color:#84ACD7;\n"
                                                "border:1px;\n"
                                                "border-radius:5px;\n"
                                                "color:#FFFFFF;\n"
                                                "font: 75 14pt \"Sawasdee Bold\";\n"
                                                "}\n"
                                                "QPushButton#openButton:hover{\n"
                                                "background-color:#68DFBB;\n"
                                                "border-radius:5px;\n"
                                                "}"));
    openButton->setFlat(true);

    connect(openButton, SIGNAL(clicked()), SLOT(on_openButton_clicked()));

    grid->addWidget(openButton, 0, 0, 1, 1);

    renameButton->setObjectName(QString::fromUtf8("renameButton"));
    renameButton->setGeometry(QRect(160, 380, 89, 25));
    renameButton->setStyleSheet(QString::fromUtf8("QPushButton#renameButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}\n"
                                                  "QPushButton#renameButton:hover{\n"
                                                  "background-color:#68DFBB;\n"
                                                  "border-radius:5px;\n"
                                                  "}"));
    renameButton->setFlat(true);

    grid->addWidget(renameButton, 0, 1, 1, 1);

    connect(renameButton, SIGNAL(clicked()), SLOT(on_renameButton_clicked()));

    deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
    deleteButton->setGeometry(QRect(160, 430, 89, 25));
    deleteButton->setStyleSheet(QString::fromUtf8("QPushButton#deleteButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}\n"
                                                  "QPushButton#deleteButton:hover{\n"
                                                  "background-color:#68DFBB;\n"
                                                  "border-radius:5px;\n"
                                                  "}"));
    deleteButton->setFlat(true);

    connect(deleteButton, SIGNAL(clicked()), SLOT(on_deleteButton_clicked()));

    grid->addWidget(deleteButton, 1, 0, 1, 1);

    inviteButton->setObjectName(QString::fromUtf8("inviteButton"));
    inviteButton->setGeometry(QRect(40, 430, 89, 25));
    inviteButton->setStyleSheet(QString::fromUtf8("QPushButton#inviteButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}\n"
                                                  "QPushButton#inviteButton:hover{\n"
                                                  "background-color:#68DFBB;\n"
                                                  "border-radius:5px;\n"
                                                  "}"));
    inviteButton->setFlat(true);

    connect(inviteButton, SIGNAL(clicked()), SLOT(on_inviteButton_clicked()));

    grid->addWidget(inviteButton, 1, 1, 1, 1);

    vLayout3->addLayout(grid);

    QSpacerItem *vSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
    vLayout3->addItem(vSpacer2);

    QSpacerItem *hSpacer4 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    hLayout4->addItem(hSpacer4);
    hLayout4->addLayout(vLayout3);

}

void Userpage::requestLogout() {
    json j = json{
            {"operation", "req_logout"},
            {"username",  client_->getUser().toStdString()},
    };
    client_->sendAtServer(j);

}

bool Userpage::colorIsDark(QString colorStr) {
    bool val = false;
    uint r = colorStr.mid(1, 2).toUInt(&val, 16);
    uint g = colorStr.mid(3, 2).toUInt(&val, 16);
    uint b = colorStr.mid(5, 2).toUInt(&val, 16);

    int brightness = (int) sqrt((r * r * 0.241) +
                                (g * g * 0.691) +
                                (b * b * 0.068));
    return brightness < 140;
}

void Userpage::handleNewFileButton() {

    QInputDialog modalWindow;
    QString label = "File name: ";
    modalWindow.setLabelText(label);
    modalWindow.setWindowTitle("New File");
    modalWindow.setMinimumSize(QSize(300, 150));
    modalWindow.setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::MinimumExpanding);
    modalWindow.setCancelButtonText("Cancel");
    modalWindow.setOkButtonText("Create");

    if (modalWindow.exec() == 1) {

        while (modalWindow.textValue() == "") {

            QDialog *dialog = new QDialog();
            dialog->setWindowTitle("Shared Editor");
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
            dialog->setWindowTitle("Shared Editor");
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
            dialog->setWindowTitle("Shared Editor");
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
        json j = json{
                {"operation", "request_new_file"},
                {"name",      modalWindow.textValue().toStdString()},
                {"username",  client_->getUser().toStdString()}
        };
        client_->sendAtServer(j);

        QString testo = modalWindow.textValue();
        client_->setFileName(testo);
    }
}

void Userpage::sendmessage(message mess) {
    client_->write(mess);
}

void Userpage::on_fileName_clicked(int i) {

    std::string s;
    if (i == 0) {
        QObject *sender = QObject::sender();
        QString str = sender->objectName();
        s = str.toStdString();
    } else {
        s = fileName;
    }
    if (selectedFile != "") {
        //deseleziono vecchio button e seleziono quello nuovo
        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}"));
        if (selectedFile == s) {
            selectedFile = "";
            return;
        }
    }
    selectedFile = s;
    QPushButton *b = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
    b->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                       "border:1px;\n"
                                       "border-radius:5px;\n"
                                       "background-color:#016FB9;\n"
                                       "color:#FFFFFF;\n"
                                       "font: 75 14pt \"Sawasdee Bold\";\n"
                                       "}"));

}

void Userpage::on_openButton_clicked() {

    if (selectedFile == "") {
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Shared Editor"),
                tr("Hey! Select a file"));
        return;
    }

    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;

    json j = json{
            {"operation", "open_file"},
            {"name",      filename},
            {"username",  owner}
    };
    client_->setFileName(QString::fromStdString(filename));
    client_->sendAtServer(j);
    //deseleziono vecchio button e seleziono quello nuovo
    QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
    deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                              "background-color:#84ACD7;\n"
                                              "border:1px;\n"
                                              "border-radius:5px;\n"
                                              "color:#FFFFFF;\n"
                                              "font: 75 14pt \"Sawasdee Bold\";\n"
                                              "}"));
    selectedFile = "";
}

void Userpage::on_renameButton_clicked() {
    if (selectedFile == "") {
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Shared Editor"),
                tr("Hey! Select a file"));
        return;
    }

    //controllare se l'utente ha il permesso per rinominare il file
    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;
    if (owner != client_->getUser().toStdString()) {
        //l'utente non ha il permesso di rinominare il file

        QMessageBox::information(
                this,
                tr("Shared Editor"),
                tr("Only who created the file can rename it"));
        //deselect button
        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}"));
    } else {

        QInputDialog modalWindow;
        QString label = "New name: ";
        modalWindow.setLabelText(label);
        modalWindow.setWindowTitle("Rename file");
        modalWindow.setMinimumSize(QSize(300, 150));
        modalWindow.setSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::MinimumExpanding);
        modalWindow.setCancelButtonText("Cancel");
        modalWindow.setOkButtonText("Rename");

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
                dialog->setWindowTitle("Shared Editor");
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
            json j = json{
                    {"operation", "request_new_name"},
                    {"old_name",  filename},
                    {"new_name",  modalWindow.textValue().toStdString()},
                    {"username",  client_->getUser().toStdString()}
            };
            client_->sendAtServer(j);
            //deselect button
            QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
            deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                      "background-color:#84ACD7;\n"
                                                      "border:1px;\n"
                                                      "border-radius:5px;\n"
                                                      "color:#FFFFFF;\n"
                                                      "font: 75 14pt \"Sawasdee Bold\";\n"
                                                      "}"));
            selectedFile = "";
        }
    }
}

void Userpage::on_deleteButton_clicked() {

    if (selectedFile == "") {
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr("Shared Editor"),
                tr("Hey! Select a file"));
        return;
    }
    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;

    if (owner == client_->getUser().toStdString()) { //sei l'owner
        QMessageBox msgBox;
        msgBox.setWindowTitle(" Shared Editor ");
        msgBox.setText("Do you really want to delete the selected file?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if (ret == QMessageBox::No) {
            QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
            deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                      "background-color:#84ACD7;\n"
                                                      "border:1px;\n"
                                                      "border-radius:5px;\n"
                                                      "color:#FFFFFF;\n"
                                                      "font: 75 14pt \"Sawasdee Bold\";\n"
                                                      "}"));
            selectedFile = "";
            return;
        } else {
            json j = json{
                    {"operation", "delete_file"},
                    {"name",      filename},
                    {"username",  client_->getUser().toStdString()},
                    {"owner",     owner}
            };
            client_->sendAtServer(j);
            QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
            deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                      "background-color:#84ACD7;\n"
                                                      "border:1px;\n"
                                                      "border-radius:5px;\n"
                                                      "color:#FFFFFF;\n"
                                                      "font: 75 14pt \"Sawasdee Bold\";\n"
                                                      "}"));
            selectedFile = "";
        }

    } else { //non sei l'owner: rimuovi invito
        QMessageBox msgBox;
        msgBox.setWindowTitle(" Shared Editor");
        msgBox.setText("Do you really want to remove the file invitation?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if (ret == QMessageBox::No) {
            QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
            deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                      "background-color:#84ACD7;\n"
                                                      "border:1px;\n"
                                                      "border-radius:5px;\n"
                                                      "color:#FFFFFF;\n"
                                                      "font: 75 14pt \"Sawasdee Bold\";\n"
                                                      "}"));
            selectedFile = "";
            return;
        } else {
            json j = json{
                    {"operation", "delete_file"},
                    {"name",      filename},
                    {"username",  client_->getUser().toStdString()},
                    {"owner",     owner}
            };
            client_->sendAtServer(j);
            //deselect button
            QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
            deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                      "background-color:#84ACD7;\n"
                                                      "border:1px;\n"
                                                      "border-radius:5px;\n"
                                                      "color:#FFFFFF;\n"
                                                      "font: 75 14pt \"Sawasdee Bold\";\n"
                                                      "}"));
            selectedFile = "";
        }
    }

}

void Userpage::updateRecentFiles(QString old, QString newN, QString owner, QString request) {
    if (request == "add_new_file") { //add button
        std::pair<std::string, std::string> parsed = parseFileButton(newN.toStdString());
        std::string owner = parsed.first;
        std::string filename = parsed.second;
        std::string invitation = filename.substr(filename.size() - 15, filename.size());
        filename.erase(filename.size() - 15);

        QPushButton *button;
        auto scrollAreaWidgets = page->findChild<QWidget *>("scrollAreaWidgets");
        button = new QPushButton(scrollAreaWidgets);

        button->setObjectName(QString::fromStdString(generateFileButton(owner, filename)));
        button->setText("(" + QString::fromStdString(owner) + "): " + QString::fromStdString(filename));
        button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "background-color:#84ACD7;\n"
                                                "border:1px;\n"
                                                "border-radius:5px;\n"
                                                "color:#FFFFFF;\n"
                                                "font: 75 14pt \"Sawasdee Bold\";\n"
                                                "}"));
        button->setFlat(true);
        page->findChild<QVBoxLayout *>("verticalLayout")->addWidget(button);
        connect(button, SIGNAL(clicked()), SLOT(on_fileName_clicked()));

        return;
    }
    if (request == "add_new_file_invitation") {
        std::pair<std::string, std::string> parsed = parseFileButton(newN.toStdString());
        std::string owner = parsed.first;
        std::string filename = parsed.second;

        QPushButton *button;
        auto scrollAreaWidgets = page->findChild<QWidget *>("scrollAreaWidgets");
        button = new QPushButton(scrollAreaWidgets);

        button->setObjectName(QString::fromStdString(generateFileButton(owner, filename)));
        button->setText("(" + QString::fromStdString(owner) + "): " + QString::fromStdString(filename));
        button->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                "background-color:#84ACD7;\n"
                                                "border:1px;\n"
                                                "border-radius:5px;\n"
                                                "color:#FFFFFF;\n"
                                                "font: 75 14pt \"Sawasdee Bold\";\n"
                                                "}"));
        button->setFlat(true);
        page->findChild<QVBoxLayout *>("verticalLayout")->addWidget(button);
        connect(button, SIGNAL(clicked()), SLOT(on_fileName_clicked()));
        lineURL->clear();
        return;
    }
    if (request == "delete_file") { //aggiorniamo dopo una delete
        QPushButton *b = recent->findChild<QPushButton *>(
                QString::fromStdString(generateFileButton(newN.toStdString(), old.toStdString())));
        page->findChild<QVBoxLayout *>("verticalLayout")->removeWidget(b);
        delete b;
        this->selectedFile = "";
    }
    if (request == "rename_file") { //aggiorniamo dopo rename
        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(generateFileButton(owner.toStdString(), old.toStdString())));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}"));
        recent->findChild<QPushButton *>(
                QString::fromStdString(generateFileButton(owner.toStdString(), old.toStdString())))->setText(
                "(" + owner + "): " + newN);
        recent->findChild<QPushButton *>(
                QString::fromStdString(generateFileButton(owner.toStdString(), old.toStdString())))->setObjectName(
                QString::fromStdString(generateFileButton(owner.toStdString(), newN.toStdString())));
        this->selectedFile = "";
    }

}

void Userpage::handleOpenURLbutton() {
    if (!this->lineURL->text().isEmpty()) {
        json j = json{
                {"operation",       "validate_invitation"},
                {"invitation_code", this->lineURL->text().toStdString()}
        };
        client_->sendAtServer(j);
    } else {
        QMessageBox::information(
                this,
                tr("Shared Editor"),
                tr("Insert a valid invitation code!"));
    }
}

void Userpage::on_inviteButton_clicked() {
    std::pair<std::string, std::string> parsed = parseFileButton(selectedFile);
    std::string owner = parsed.first;
    std::string filename = parsed.second;
    if (selectedFile == "") {
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr(" Shared Editor"),
                tr("Hey!Select a file"));
        return;
    }
    if (owner != client_->getUser().toStdString()) {
        //nessun file selezionato
        QMessageBox::information(
                this,
                tr(" Shared Editor "),
                tr("Hey!You must be the owner in order to invite"));

        QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
        deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                                  "background-color:#84ACD7;\n"
                                                  "border:1px;\n"
                                                  "border-radius:5px;\n"
                                                  "color:#FFFFFF;\n"
                                                  "font: 75 14pt \"Sawasdee Bold\";\n"
                                                  "}"));

        selectedFile = "";
        return;
    }
    const char *code;
    for (auto &iter : client_->files) {
        if (iter.first.second == filename && iter.first.first == client_->getUser().toStdString()) {
            code = iter.second.c_str();
            break;
        }
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->clear();
    clipboard->setText(code);
    QMessageBox::information(
            this,
            tr("Invitation code:"),
            tr(code));
    QPushButton *deselect = recent->findChild<QPushButton *>(QString::fromStdString(selectedFile));
    deselect->setStyleSheet(QString::fromUtf8("QPushButton{\n"
                                              "background-color:#84ACD7;\n"
                                              "border:1px;\n"
                                              "border-radius:5px;\n"
                                              "color:#FFFFFF;\n"
                                              "font: 75 14pt \"Sawasdee Bold\";\n"
                                              "}"));
    selectedFile = "";
}

std::pair<std::string, std::string> Userpage::parseFileButton(const std::string &button) {
    int dimOwner = button[0];
    dimOwner -= 48;
    std::string owner;
    std::string filename;
    int i;
    if (button[1] != '_') {
        int j = button[1];
        j -= 48;
        dimOwner *= 10;
        dimOwner += j;
        for (i = 3; i < dimOwner + 3; i++) {
            owner.push_back(button[i]);
        }
        for (; i < button.size(); i++) {
            filename.push_back(button[i]);
        }
    } else {
        for (i = 2; i < dimOwner + 2; i++) {
            owner.push_back(button[i]);
        }
        for (; i < button.size(); i++) {
            filename.push_back(button[i]);
        }
    }
    return std::pair<std::string, std::string>(owner, filename);
}

std::string Userpage::generateFileButton(const std::string &owner, const std::string &filename) {
    return std::to_string(owner.size()) + '_' + owner + filename;
}

void Userpage::clearLineURL() {
    lineURL->clear();
}

void Userpage::updateInfo() {
    email_lab->setText(client_->getEmail());
    myIcon->setStyleSheet(QString::fromUtf8("background-color:") + client_->getColor());
    if (colorIsDark(client_->getColor())) {
        usrLetters->setStyleSheet(QString::fromUtf8("color:rgb(243,243,243);font:36pt \"Sawasdee\";"));
    } else {
        usrLetters->setStyleSheet(QString::fromUtf8("color:rgb(0,0,0);font:36pt \"Sawasdee\";"));
    }

}
