#include "Headers/stacked.h"
#include "ui_stacked.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <iostream>
#include <boost/asio.hpp>
#include <QtWidgets/QInputDialog>
#include "Libs/json.hpp"
#include "Libs/md5.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_MACOS
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

using json = nlohmann::json;
using boost::asio::ip::tcp;


stacked::stacked(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::stacked),
        client_(new Client) {
    QObject::connect(client_, &Client::formResultSuccess, this, &stacked::showPopupSuccess);

    ui->setupUi(this);
    setWindowTitle("SharedEditor - Login or Signup");
}

stacked::~stacked() {
    delete ui;
}

void stacked::closeEvent(QCloseEvent *e) {
    //comunica logout al server
    json j = json{
            {"operation", "req_logout_and_close"},
            {"username",  client_->getUser().toStdString()}
    };
    client_->sendAtServer(j);

}

/*
 *  indici
 *  0 = loginpage
 *  1 = registration form
 *  2 = editprofile
 *  3 = textedit
 *  4 = userpage
 *
 * */
void stacked::on_loginButton_clicked() {
    QString user = ui->user_log_line->text();
    QString psw = ui->psw_log_line->text();

    if (user.isEmpty() || psw.isEmpty()) {
        QDialog *dialog = new QDialog();
        dialog->setWindowTitle("Shared Editor");
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Empty username and/or password"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

        dialog->show();
    } else {
        json j = json{{"operation", "request_login"},
                      {"username",  user.toStdString()},
                      {"password",  md5(psw.toUtf8().constData())}};
        client_->sendAtServer(j);
    }
}

void stacked::sendmessage(message mess) {
    client_->write(mess);
}

void stacked::on_form_regButton_clicked() {
    //qui creare richiesta e inviarla al server
    //^[0-9a-zA-Z]+([0-9a-zA-Z][-._+])[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)([0-9a-zA-Z][.])[a-zA-Z]{2,6}$
    QRegularExpression email_regex("^([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]{2,5})$");
    QRegularExpression psw_regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.{8,})");

    QString email = ui->email_line->text();
    QString user = ui->user_line_form->text();
    QString psw1 = ui->psw_line_form->text();
    QString psw2 = ui->confirm_psw_line->text();

    if (!email_regex.match(email).hasMatch()) {
        QDialog *dialog = new QDialog();
        dialog->setWindowTitle("Shared Editor");
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Wrong email format"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

        dialog->show();
    } else if (!psw_regex.match(psw1).hasMatch()) {
        // check password
        QDialog *dialog = new QDialog();
        dialog->setWindowTitle("Shared Editor");
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Password must be at least 8 characters long "
                                     "and include at least one upper case letter, "
                                     "a number and a special character"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

        dialog->show();
    } else if (!(psw1 == psw2)) {
        QDialog *dialog = new QDialog();
        dialog->setWindowTitle("Shared Editor");
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("Non matching password!"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

        dialog->show();
    } else if (user.length() > 10) {
        QDialog *dialog = new QDialog();
        dialog->setWindowTitle("Shared Editor");
        QVBoxLayout *layout = new QVBoxLayout();
        dialog->setLayout(layout);
        layout->addWidget(new QLabel("User length max 10 char "));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

        dialog->show();
    } else {
        json j = json{
                {"operation", "request_signup"},
                {"email",     email.toStdString()},
                {"username",  user.toStdString()},
                {"password",  md5(psw1.toUtf8().constData())}
        };
        client_->sendAtServer(j);

    }
}

// type_request=="QUERY_ERROR"    LOGIN_ERROR
// || type_request=="CONNESSION_ERROR" || SIGNUP_ERROR_DUPLICATE_USERNAME || SIGNUP_ERROR_INSERT_FAILED)

void stacked::showPopupSuccess(QString result) {
    QDialog *dialog = new QDialog();
    dialog->setWindowTitle(QString::fromUtf8("Shared Editor"));
    QVBoxLayout *layout = new QVBoxLayout();;
    dialog->setLayout(layout);
    if (result == "LOGIN_SUCCESS" || result == "SIGNUP_SUCCESS") {

        if (result == "SIGNUP_SUCCESS") {
            ui->email_line->clear();
            ui->user_line_form->clear();
            ui->psw_line_form->clear();
            ui->confirm_psw_line->clear();

            layout->addWidget(new QLabel("Signup completed. Welcome in Shared Editor!"));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);

            connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
            dialog->show();
            ui->stackedWidget->setCurrentIndex(0);
        } else {
            up = new Userpage(this, client_);
            te = new TextEdit(client_);
            connect(te, &TextEdit::logout, this, &stacked::logout);
            connect(up, &Userpage::upLogout, this, &stacked::logout);
            connect(te, &TextEdit::closeFile, this, &stacked::closeFile);
            connect(te, &TextEdit::closeAll, this, &stacked::closeAll);
            connect(up, &Userpage::goToEdit, this, &stacked::editPage);
            connect(client_, &Client::showLoading, this, &stacked::showLoading);

            setWindowTitle("SharedEditor - Userpage");
            ui->stackedWidget->addWidget(te);
            ui->stackedWidget->addWidget(up);
            ui->stackedWidget->setCurrentIndex(4);
        }
    } else {

        if (result == "QUERY_ERROR" || result == "CONNESSION_ERROR" || result == "SIGNUP_ERROR_INSERT_FAILED") {
            layout->addWidget(new QLabel("Error with the DB, try again"));
        } else if (result == "LOGIN_ERROR") {
            layout->addWidget(new QLabel("Invalid username and/or password"));
        } else if (result == "user_already_logged") {
            layout->addWidget(new QLabel("Error! User already logged!"));
        } else if (result == "SIGNUP_ERROR_DUPLICATE_USERNAME") {
            layout->addWidget(new QLabel("Username already registered"));
        } else if (result == "new_file_created") {

            layout->addWidget(new QLabel("File correctly created"));
            //index 3 = text editor
            setWindowTitle("SharedEditor - " + client_->getUser() + " @ " + client_->getFileName());
            ui->stackedWidget->setCurrentIndex(3);
        } else if (result == "new_file_already_exist") {
            layout->addWidget(new QLabel("File already exists"));
        } else if (result == "file_opened") {
            setWindowTitle("SharedEditor - " + client_->getUser() + " @ " + client_->getFileName());
            ui->stackedWidget->setCurrentIndex(3);
        } else if (result == "errore_rinomina_file") {
            layout->addWidget(new QLabel("Rename file error"));
        } else if (result == "file_renamed") {
            layout->addWidget(new QLabel("File correctly renamed"));
        } else if (result == "new_name_already_exist") {
            layout->addWidget(new QLabel("New name already exist"));
        } else if (result == "file_deleted") {
            layout->addWidget(new QLabel("File correctly deleted"));
        } else if (result == "error_file_in_use_d") {
            layout->addWidget(new QLabel("File in use, delete failed"));
        } else if (result == "error_file_in_use") {
            layout->addWidget(new QLabel("File in use, rename failed"));
        } else if (result == "wrong_old_password") {
            layout->addWidget(new QLabel("Wrong old password!"));
        } else if (result == "edit_success") {
            up->updateInfo();
            ui->stackedWidget->setCurrentIndex(4);
            layout->addWidget(new QLabel("Profile updated!"));
        } else if (result == "edit_failed") {
            ui->stackedWidget->setCurrentIndex(4);
            layout->addWidget(new QLabel("Error in update profile."));
        } else if (result == "invitation_error"){
            layout->addWidget(new QLabel("Invalid invitation code!"));
        }

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);

        connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
        if (result != "file_opened") dialog->show();
    }
}

void stacked::on_reglogButton_clicked() {
    setWindowTitle("SharedEditor - Login or Signup");
    ui->stackedWidget->setCurrentIndex(1);

}

void stacked::on_form_cancButton_clicked() {
    ui->psw_log_line->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void stacked::logout() {
    setWindowTitle("SharedEditor - Login or Signup");
    ui->user_log_line->clear();
    ui->psw_log_line->clear();
    ui->user_log_line->setFocus();
    client_->setFiles(std::map<std::pair<std::string, std::string>, std::string>());
    ui->stackedWidget->removeWidget(te);
    ui->stackedWidget->removeWidget(up);
    ui->stackedWidget->setCurrentIndex(0);
    delete up;
    delete te;
}

void stacked::closeFile() {
    setWindowTitle("SharedEditor - Userpage");
    up->clearLineURL();
    ui->stackedWidget->setCurrentIndex(4);
}

void stacked::closeAll() {
    this->close();
}

void stacked::on_psw_log_line_returnPressed() {
    on_loginButton_clicked();
}

void stacked::showLoading() {
    QMessageBox *msgB = new QMessageBox(QMessageBox::Information,tr("Shared Editor"),tr("Loading..."),
                                        QMessageBox::NoButton,this,Qt::CustomizeWindowHint);
    msgB->setStandardButtons(0);
    msgB->setIconPixmap(QPixmap(rsrcPath+QString::fromUtf8("/hourglass.png")));
    msgB->setWindowFlag(Qt::WindowCloseButtonHint,false);
    connect(client_,&Client::closeLoading,msgB,&QMessageBox::accept);
    msgB->show();
}

void stacked::editPage() {
    ui->edit_email_line->setText(client_->getEmail());
    ui->edit_user_label->setText(client_->getUser());
    ui->edit_icon->setStyleSheet(QString::fromUtf8("background-color:") + client_->getColor());
    ui->edit_oldpsw_line->clear();
    ui->edit_oldpsw_line->setStyleSheet("");
    ui->edit_newpsw_line->clear();
    ui->edit_confnewpsw_line->clear();
    ui->edit_oldpsw_line->setFocus();
    setWindowTitle("Shared Editor - Edit Profile");
    ui->stackedWidget->setCurrentIndex(2);

}

void stacked::on_edit_backButton_clicked() {
    setWindowTitle("Shared Editor - Userpage");
    ui->stackedWidget->setCurrentIndex(4);
}

void stacked::on_colorPicker_clicked() {
    QColorDialog *colorDialog = new QColorDialog();
    colorDialog->setOption(QColorDialog::DontUseNativeDialog, false);
    colorDialog->setOption(QColorDialog::NoButtons, false);
    colorDialog->show();
    QObject::connect(colorDialog, &QColorDialog::colorSelected, this, [=]() {
        ui->edit_icon->setStyleSheet(QString::fromUtf8("background-color:") + colorDialog->selectedColor().name());
        selectedColor = colorDialog->selectedColor().name();
    });
}

void stacked::on_edit_saveButton_clicked() {
    QRegularExpression email_regex("^([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]{2,5})$");
    QRegularExpression psw_regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.{8,})");

    QString email = ui->edit_email_line->text();
    QString user = ui->edit_user_label->text();
    QString oldPsw = ui->edit_oldpsw_line->text();
    QString newPsw1 = ui->edit_newpsw_line->text();
    QString newPsw2 = ui->edit_confnewpsw_line->text();
    QString pswChecked, emailChecked, newColor;

    if (oldPsw.isEmpty()) {
        QMessageBox::warning(this, tr(" Shared Editor"), tr("Insert old password to validate changes."));
        ui->edit_oldpsw_line->setStyleSheet(QString::fromUtf8("QLineEdit#edit_oldpsw_line{\n"
                                                              "border-radius: 3px;\n"
                                                              "border-style: solid;\n"
                                                              "border-width: 1px;\n"
                                                              "border-color: red}"));
        return;
    }

    if (newPsw1.isEmpty() && newPsw2.isEmpty()) {
        pswChecked = "";
    } else {
        if (!psw_regex.match(newPsw1).hasMatch()) {
            // check password
            QDialog *dialog = new QDialog();
            dialog->setWindowTitle("Shared Editor");
            QVBoxLayout *layout = new QVBoxLayout();
            dialog->setLayout(layout);
            layout->addWidget(new QLabel("Password must be at least 8 characters long \n"
                                         "and include at least one upper case letter, \n"
                                         "a number and a special character."));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);

            connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

            dialog->show();
            return;
        } else if (!(newPsw1 == newPsw2)) {
            QDialog *dialog = new QDialog();
            dialog->setWindowTitle("Shared Editor");
            QVBoxLayout *layout = new QVBoxLayout();
            dialog->setLayout(layout);
            layout->addWidget(new QLabel("Non matching new passwords!"));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);

            connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

            dialog->show();
            return;
        } else if (newPsw1 == oldPsw) {
            pswChecked = "";
        } else
            pswChecked = QString::fromStdString(md5(newPsw1.toStdString()));
    }
    if (email != client_->getEmail()) {
        //controllo formato mail
        if (!email_regex.match(email).hasMatch()) {
            QDialog *dialog = new QDialog();
            dialog->setWindowTitle("Shared Editor");
            QVBoxLayout *layout = new QVBoxLayout();
            dialog->setLayout(layout);
            layout->addWidget(new QLabel("Wrong email format."));
            QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout->addWidget(buttonBox);

            connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

            dialog->show();
            return;
        }
        emailChecked = email;
    } else {
        emailChecked = "";
    }
    if (selectedColor != client_->getColor())
        newColor = selectedColor;
    else
        newColor = "";

    try {
        json j = json{
                {"operation",   "request_update_profile"},
                {"email",       emailChecked.toStdString()},
                {"username",    user.toStdString()},
                {"oldPassword", md5(oldPsw.toUtf8().constData())},
                {"newPassword", pswChecked.toStdString()},
                {"newColor",    newColor.toStdString()}
        };
        client_->sendAtServer(j);
    } catch (std::exception &e) {
        std::cerr << "Exception :" << e.what() << "\n";
    }
}
