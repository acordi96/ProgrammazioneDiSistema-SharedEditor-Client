#ifndef USERPAGE_H
#define USERPAGE_H

#include <QMainWindow>
#include "Client.h"


QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QSCrollArea;
class QHBoxLayout;
QT_END_NAMESPACE

class Userpage : public QMainWindow
{
    Q_OBJECT
public:
    Userpage(QWidget *parent = 0,Client *c= nullptr);
private:
    void setupRecentFiles();
    void setupUserinfo();
    void sendmessage(message mess);
    Client *client_;
    QHBoxLayout *hLayout;
    QWidget *page;
    QWidget *recent;
    QWidget *userinfo;
    std::string fileName;



private slots:
    void handleNewFileButton();
    void openFile();
    void renameFile();
    void deleteFile();
    //void onRightClicked();


public slots:
    void customMenuRequested(QPoint pos);
    void on_fileName_clicked(int i = 0);


private:
    QPushButton *newFileButton;

};

#endif // USERPAGE_H
