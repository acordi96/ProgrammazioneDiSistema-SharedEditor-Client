#ifndef STACKED_H
#define STACKED_H

#include <QDialog>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QColorDialog>
#include "Headers/textedit.h"
#include "Headers/userpage.h"
#include "Headers/Client.h"
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
protected:
    void closeEvent(QCloseEvent *e) override;
public slots:
    void showPopupSuccess(QString result);
private slots:
   void on_loginButton_clicked();
   void on_form_regButton_clicked();
   void on_reglogButton_clicked();
   void on_form_cancButton_clicked();
   void on_psw_log_line_returnPressed();
   void on_edit_backButton_clicked();
   void on_colorPicker_clicked();
   void on_edit_saveButton_clicked();
   //void on_fileButton_clicked();
   //void on_newFileButton_clicked();
   void logout();
   void closeAll();
   void closeFile();
   void editPage();
   void showLoading(bool active);
/*signals:
    void updateRecentFiles();*/
private:
   Userpage *up;
   TextEdit *te;

   //for loading
   QWidget *loadLabel;
   QLabel *label;
   QWidget *hourglass;
   QHBoxLayout *layout;
   QColorDialog *colorDialog;
   QString selectedColor="";
};

#endif // STACKED_H
