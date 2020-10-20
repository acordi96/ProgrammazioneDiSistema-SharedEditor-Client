#include "myqtextedit.h"
#include <QMimeData>

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
        fmt.setFontFamily("Sans Serif");
        fmt.setFontItalic(false);
        fmt.setFontUnderline(false);
        fmt.setFontWeight(QFont::Normal);
        fmt.setFontPointSize(11);
        cursor.setCharFormat(fmt);
        cursor.insertText(text);

    }
}
