#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class registrationform; }
QT_END_NAMESPACE

class registrationform : public QMainWindow
{
    Q_OBJECT

public:
    registrationform(QWidget *parent = nullptr);
    ~registrationform();

private:
    Ui::registrationform *ui;
};

#endif // REGISTRATIONFORM_H
