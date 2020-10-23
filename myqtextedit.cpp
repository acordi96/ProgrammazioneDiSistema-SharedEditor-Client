#include "myqtextedit.h"
#include <QMimeData>

#define DEFAULT_FONT_FAMILY "Helvetica"
#ifdef __linux__
    #define DEFAULT_FONT_SIZE 11
#else //winzoz
    #define DEFAULT_FONT_SIZE 9
#endif

//bool MyQTextEdit::canInsertFromMimeData(const QMimeData *source) const{
//    if(source->hasText()){
//        return true;
//    }else
//        return QTextEdit::canInsertFromMimeData(source);
//}

void MyQTextEdit::insertFromMimeData(const QMimeData *source){
    if (source->hasText()){
        QString text = source->text();
        QTextCursor cursor = textCursor();

        QTextCharFormat fmt;
        fmt.setFontFamily(DEFAULT_FONT_FAMILY);
        fmt.setFontItalic(false);
        fmt.setFontUnderline(false);
        fmt.setFontWeight(QFont::Normal);
        fmt.setFontPointSize(DEFAULT_FONT_SIZE);
        fmt.setForeground(QColor(QString::fromStdString("#000000")));
        cursor.setCharFormat(fmt);
        cursor.insertText(text);

    }
}
