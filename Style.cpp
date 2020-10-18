//
// Created by Sam on 14/ott/2020.
//

#include "Headers/Style.h"

Style::Style() : bold(false), underlined(false), italic(false), fontFamily(DEFAULT_FONT_FAMILY), fontSize(DEFAULT_FONT_SIZE),color(DEFAULT_COLOR) {}

//per prender
Style::Style(bool bold, bool underlined, bool italic, std::string fontFamily, int fontSize) : bold(bold), underlined(underlined), italic(italic), fontFamily(fontFamily), fontSize(fontSize){}

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

    return QTextCharFormat();
}

void Style::setTextCharFormat(QTextCharFormat tcf) {
    this->bold = tcf.fontWeight();
    this->italic = tcf.fontItalic();
    this->underlined = tcf.fontUnderline();
    //TODO: others
}
