#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

#include <QTextEdit>
#include <QWidget>

class MyQTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MyQTextEdit(QWidget *parent):QTextEdit(parent){};

    void insertFromMimeData(const QMimeData *source) override;

//    bool canInsertFromMimeData(const QMimeData *source) const override;


};

#endif // MYQTEXTEDIT_H
