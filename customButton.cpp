//
// Created by gabriele on 07/08/2020.
//

#include <iostream>
#include "customButton.h"


customButton::customButton(QPushButton *parent):QPushButton() {

}

void customButton::refreshFileList(std::string oldName, std::string newName) {
    std::cout << "\n" << oldName << "\n" << newName;
}


