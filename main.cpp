#include "registrationform.h"
#include "stacked.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    stacked *s = new stacked();

    s->show();
    return a.exec();
}
