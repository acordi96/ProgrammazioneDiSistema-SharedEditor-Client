//
// Created by Sam on 14/ott/2020.
//

#ifndef PROGETTO_PROVA_STYLE_H
#define PROGETTO_PROVA_STYLE_H

#define DEFAULT_FONT_FAMILY "Helvetica"
#define DEFAULT_COLOR "#000000"
#ifdef __linux__
    #define DEFAULT_FONT_SIZE 11
#else //winzoz
    #define DEFAULT_FONT_SIZE 9
#endif

#include <string>
#include <QtGui/QTextCharFormat>

class Style {
private:
    bool bold;
    bool underlined;
    bool italic;
    std::string fontFamily;
    int fontSize;
    std::string color;
public:
    Style();

    Style(bool bold, bool underlined, bool italic, std::string fontFamily, int fontSize, std::string color);

    bool isBold() const;

    void setBold(bool bold);

    bool isUnderlined() const;

    void setUnderlined(bool underlined);

    bool isItalic() const;

    void setItalic(bool italic);

    const std::string &getFontFamily() const;

    void setFontFamily(const std::string &fontFamily);

    int getFontSize() const;

    void setFontSize(int fontSize);

    const std::string &getColor() const;

    void setColor(const std::string &color);

    QTextCharFormat getTextCharFormat();

    void setTextCharFormat(QTextCharFormat tcf);

};

#endif //PROGETTO_PROVA_STYLE_H
