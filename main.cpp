#include "Headers/registrationform.h"
#include "Headers/stacked.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    stacked *s = new stacked();

    s->show();
    return a.exec();
}
