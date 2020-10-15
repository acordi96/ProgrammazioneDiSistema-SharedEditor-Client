// Created by Sam on 15/ott/2020.
//
#ifndef PROGETTO_PROVA_STYLE_H
#define PROGETTO_PROVA_STYLE_H

#define DEFAULT_FONT_FAMILY "Times New Roman"
#define DEFAULT_FONT_SIZE 14
#define DEFAULT_COLOR "#00ffffff"

#include <string>

class Style {
private:
    int bold; //1 se è bold, 0 se non lo è, 2 quando lo devo passare e voglio igorarlo
    int underlined; //stessa cosa
    std::string fontFamily;
    int fontSize;
    std::string color;
public:
    Style();

    int getBold() const;

    void setBold(int bold);

    int getUnderlined() const;

    void setUnderlined(int underlined);

    const std::string &getFontFamily() const;

    void setFontFamily(const std::string &fontFamily);

    int getFontSize() const;

    void setFontSize(int fontSize);

    const std::string &getColor() const;

    void setColor(const std::string &color);
};


#endif //PROGETTO_PROVA_STYLE_H

