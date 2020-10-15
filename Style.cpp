//
// Created by Sam on 15/ott/2020.
//
#include "Headers/Style.h"

Style::Style() : bold(false), underlined(false), fontFamily(DEFAULT_FONT_FAMILY), fontSize(DEFAULT_FONT_SIZE),color(DEFAULT_COLOR) {}

int Style::getBold() const {
    return bold;
}

void Style::setBold(int bold) {
    Style::bold = bold;
}

int Style::getUnderlined() const {
    return underlined;
}

void Style::setUnderlined(int underlined) {
    Style::underlined = underlined;
}

const std::string &Style::getFontFamily() const {
    return fontFamily;
}

void Style::setFontFamily(const std::string &fontFamily) {
    Style::fontFamily = fontFamily;
}

int Style::getFontSize() const {
    return fontSize;
}

void Style::setFontSize(int fontSize) {
    Style::fontSize = fontSize;
}

const std::string &Style::getColor() const {
    return color;
}

void Style::setColor(const std::string &color) {
    Style::color = color;
}

