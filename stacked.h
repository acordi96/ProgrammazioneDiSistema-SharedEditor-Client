#ifndef STACKED_H
#define STACKED_H

#include <QMainWindow>
#include "textedit.h"
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
private slots:
   void on_loginButton_clicked();
   void on_form_regButton_clicked();
   void on_reglogButton_clicked();
   void on_fileButton_clicked();
   void logout();
   void closeAll();
};

#endif // STACKED_H
