//
// Created by gabriele on 07/08/2020.
//

#ifndef PROGETTO_PROVA_CUSTOMBUTTON_H
#define PROGETTO_PROVA_CUSTOMBUTTON_H


#include <QtWidgets/QPushButton>

class customButton : public QPushButton{
    Q_OBJECT
public:
        explicit customButton(QPushButton *parent=0);
public slots:
    void refreshFileList(std::string oldName,std::string newName);



};


#endif //PROGETTO_PROVA_CUSTOMBUTTON_H
