#include "registrationform.h"
#include "ui_registrationform.h"

registrationform::registrationform(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registrationform)
{
    ui->setupUi(this);
}

registrationform::~registrationform()
{
    delete ui;
}
