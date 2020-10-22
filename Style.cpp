//
// Created by Sam on 14/ott/2020.
//

#include "Headers/Style.h"

Style::Style() : bold(false), underlined(false), italic(false), fontFamily(DEFAULT_FONT_FAMILY), fontSize(DEFAULT_FONT_SIZE),color(DEFAULT_COLOR) {}

Style::Style(bool bold, bool underlined, bool italic, std::string fontFamily, int fontSize, std::string cl) : bold(bold), underlined(underlined), italic(italic), fontFamily(fontFamily), fontSize(fontSize), color(cl){}

bool Style::isBold() const {
    return bold;
}

void Style::setBold(bool bold) {
    Style::bold = bold;
}

bool Style::isUnderlined() const {
    return underlined;
}

void Style::setUnderlined(bool underlined) {
    Style::underlined = underlined;
}

bool Style::isItalic() const {
    return italic;
}

void Style::setItalic(bool italic) {
    Style::italic = italic;
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

QTextCharFormat Style::getTextCharFormat() {
    QTextCharFormat format;

    format.setFontWeight(this->bold);
    format.setFontItalic(this->italic);
    format.setFontUnderline(this->underlined);
    format.setForeground(QColor(QString::fromStdString(this->color)));
    format.setFontPointSize(this->fontSize);
    format.setFontFamily(QString::fromStdString(this->fontFamily));

    return QTextCharFormat();
}

void Style::setTextCharFormat(QTextCharFormat tcf) {
    if(tcf.fontWeight() == QFont::Bold)
        this->bold = true;
    else
        this->bold = false;
    this->italic = tcf.fontItalic();
    this->underlined = tcf.fontUnderline();
    this->color = tcf.foreground().color().name().toStdString();
    this->fontFamily = tcf.fontFamily().toStdString();
    this->fontSize = tcf.fontPointSize();
}
