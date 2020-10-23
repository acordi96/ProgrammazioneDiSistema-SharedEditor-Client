QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        customcursor.cpp \
        main.cpp \
    myqtextedit.cpp \
        stacked.cpp \
        textedit.cpp \
        Client.cpp  \
        message.cpp \
        userpage.cpp\
        Symbol.cpp \
        MessageSymbol.cpp\
        Style.cpp\
        Libs/md5.cpp \

HEADERS += \
        Headers/Client.h \
        Headers/customcursor.h \
        Headers/message.h \
        Headers/MessageSymbol.h \
        Headers/stacked.h \
        Headers/Symbol.h \
        Headers/textedit.h \
        Headers/userpage.h \
        Headers/Style.h\
        Libs/md5.h \
        Libs/json.hpp \
        myqtextedit.h

RESOURCES += \
    project_icons.qrc
INCLUDEPATH += /home/cordi/boost/include
LIBS += "-L/home/cordi/boost/lib"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    stacked.ui
