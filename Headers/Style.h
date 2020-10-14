//
// Created by Sam on 14/ott/2020.
//

#ifndef PROGETTO_PROVA_STYLE_H
#define PROGETTO_PROVA_STYLE_H

#define DEFAULT_FONT_FAMILY "Times New Roman"
#define DEFAULT_FONT_SIZE 14
#define DEFAULT_COLOR "#00ffffff"

#include <string>

class Style {
private:
    bool bold;
    bool underlined;
    std::string fontFamily;
    int fontSize;
    std::string color;
public:
    Style();

    bool isBold() const;

    void setBold(bool bold);

    bool isUnderlined() const;

    void setUnderlined(bool underlined);

    const std::string &getFontFamily() const;

    void setFontFamily(const std::string &fontFamily);

    int getFontSize() const;

    void setFontSize(int fontSize);

    const std::string &getColor() const;

    void setColor(const std::string &color);
};


#endif //PROGETTO_PROVA_STYLE_H
