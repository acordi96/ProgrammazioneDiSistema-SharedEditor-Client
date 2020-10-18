//
// Created by cordi on 09/10/19.
//

#ifndef PROGETTO_MALNATI2_TEXTEDIT_H
#define PROGETTO_MALNATI2_TEXTEDIT_H

#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QLabel>

#include "Headers/Client.h"
#include "Headers/customcursor.h"

QT_BEGIN_NAMESPACE
class QAction;

class QComboBox;

class QFontComboBox;

class QTextEdit;

class QTextCharFormat;

class QMenu;

class QPrinter;

QT_END_NAMESPACE

class QEvent;

typedef std::map<std::string, std::string> usersInFile;

class TextEdit : public QMainWindow {
Q_OBJECT

public:
    TextEdit(Client *c, QWidget *parent = 0);

    bool load(const QString &f);

    QString getFileName() const;

public slots:

    void fileNew();

    void showSymbol(int pos, QChar c);

    void showSymbolWithId(Symbol symbolToInsert);

    void showSymbolWithStyle(Symbol symbolToInsert);

    void updateRemotePosition(QString user, int pos);

    void eraseSymbols(std::vector<Symbol> symbolsToErase);
    //TO DO:mettere user
    //void show_Symbol(std::pair<int,char> tuple);


    //per lo stile
    void changeStyle(json js);

protected:
    void closeEvent(QCloseEvent *e) override;

    bool eventFilter(QObject *obj, QEvent *ev) override;

signals:

    void logout();

    void closeFile();

    void closeAll();

    void updateCursor();

private slots:

    void fileOpen();

    bool fileSave();

    bool fileSaveAs();

    void filePrint();

    void filePrintPreview();

    void filePrintPdf();

    void textBold();

    void textUnderline();

    void textItalic();

    void textFamily(const QString &f);

    void textSize(const QString &p);

    void textStyle(int styleIndex);

    void textColor();
    //void textAlign(QAction *a);

    //void currentCharFormatChanged(const QTextCharFormat &format);

    void cursorPositionChanged();

    void clipboardDataChanged();

    void about();

    void printPreview(QPrinter *);

    void drawRemoteCursors();

    void updateListParticipants(usersInFile users);

    void highlightcharacter();

    void clearHighlights();

private:
    Client *client_;

    void setupFileActions();

    void setupEditActions();

    void setupTextActions();

    bool maybeSave();

    void setCurrentFileName(const QString &fileName);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    void fontChanged(const QFont &f);

    void colorChanged(const QColor &c);

    void resetText();

    void resetCursors();

    void updateCursors(const CustomCursor &cursor);

    void updateConnectedUser(QString user, QString color);

    void updateConnectedUsers(usersInFile users);

    void updateCursors();

    //for debug
    void setupConnectedUsers();

    void requestLogout();

    void closingFile();


    void drawGraphicCursor();

    void incrementPosition(int pos, int count);

    void decreentPosition(int pos, int count);

    //funzioni per lo stile

    void requestStyleChanged(std::string fontFamily);

    void requestStyleChanged(int fontSize);

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;
    /*QAction *actionAlignLeft;
    QAction *actionAlignCenter;
    QAction *actionAlignRight;
    QAction *actionAlignJustify;*/
    /*QAction *actionUndo;
    QAction *actionRedo;*/
#ifndef QT_NO_CLIPBOARD
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
#endif

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    QTextEdit *textEdit;
    QListWidget *connectedUsers;

    bool key_to_handle = false;
    CustomCursor _newCursor = CustomCursor();
    CustomCursor _oldCursor = CustomCursor();
    QString _currentText = QString{};
    std::map<QString, CustomCursor> _cursorsVector;
    std::map<QString, QColor> _listParticipantsAndColors;
    std::map<QString, QLabel *> _labels;

    QTimer *timer;
};


#endif //PROGETTO_MALNATI2_TEXTEDIT_H
