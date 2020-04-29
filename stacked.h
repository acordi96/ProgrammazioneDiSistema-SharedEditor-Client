#ifndef STACKED_H
#define STACKED_H

#include <QMainWindow>
#include "textedit.h"
#include "Client.h"
namespace Ui {
class stacked;
}

class stacked : public QMainWindow
{
    Q_OBJECT

public:
    explicit stacked(QWidget *parent = nullptr);
    ~stacked();

private:
    Ui::stacked *ui;
    Client *client_;
    void sendmessage(message mess);
    std::string readMessage();
public slots:
    void showPopupSuccess(QString result);
private slots:
   void on_loginButton_clicked();
   void on_form_regButton_clicked();
   void on_reglogButton_clicked();
   void on_fileButton_clicked();
   void on_newFileButton_clicked();
   void logout();
   void closeAll();
};

#endif // STACKED_H
