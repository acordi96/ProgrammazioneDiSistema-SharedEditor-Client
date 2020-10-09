#ifndef USERPAGE_H
#define USERPAGE_H

#include <QLineEdit>
#include <QMainWindow>
#include "Headers/Client.h"


QT_BEGIN_NAMESPACE
class QLabel;

class QPushButton;

class QSCrollArea;

class QHBoxLayout;

QT_END_NAMESPACE

class Userpage : public QMainWindow {
Q_OBJECT
public:
    Userpage(QWidget *parent = 0, Client *c = nullptr);

    static std::pair<std::string, std::string> parseFileButton(const std::string& button);

    static std::string generateFileButton(const std::string &owner, const std::string &filename);

private:
    void setupRecentFiles();

    void setupUserinfo();
    void sendmessage(message mess);

    void requestLogout();

    Client *client_;
    QHBoxLayout *hLayout;
    QWidget *page;
    QWidget *recent;
    QWidget *userinfo;
    QWidget *selector;
    std::string fileName;
    std::string selectedFile = "";

    QLineEdit *urlLine;

    QWidget *myIcon;

private slots:

    void handleNewFileButton();

    /*void openFile();
    void renameFile();
    void deleteFile();*/
    void iconSelector();

    void changeIcon();
    //void onRightClicked();

signals:

    void upLogout();

public slots:

    //void customMenuRequested(QPoint pos);
    void on_openButton_clicked();

    void on_renameButton_clicked();

    void on_deleteButton_clicked();

    void on_inviteButton_clicked();

    void on_fileName_clicked(int i = 0);

    void updateRecentFiles(QString old, QString newN, QString owner, QString request);

    void handleOpenURLbutton();

    //void updateFiles();


private:
    QPushButton *newFileButton;
    QLineEdit *lineURL;
    QPushButton *openURLbutton;

    bool colorIsDark(QString color);
};

#endif // USERPAGE_H
