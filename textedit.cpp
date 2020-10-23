//
// Created by cordi on 09/10/19.
//


#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QTextBlockFormat>
#include <QDockWidget>
#include <QLabel>
#include <QColor>
#include <QPushButton>

#if defined(QT_PRINTSUPPORT_LIB)

#include <QtPrintSupport/qtprintsupportglobal.h>

#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)

#include <QPrintDialog>

#endif

#include <QPrinter>

#if QT_CONFIG(printpreviewdialog)

#include <QPrintPreviewDialog>

#endif
#endif
#endif

#include "Headers/textedit.h"

#ifdef Q_OS_MACOS
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

#ifdef __linux__
#define DEFAULT_FONT_SIZE 11
#else //winzoz
#define DEFAULT_FONT_SIZE 9
#endif

#define MAX_ALLOWED_CHARS 1000

TextEdit::TextEdit(Client *c, QWidget *parent)
        : QMainWindow(parent), client_(c) {
#ifdef Q_OS_MACOS
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    textEdit = new MyQTextEdit(this);
    timerHighlight = new QTimer(this);
    timerHighlight->setSingleShot(true);
    timerUpdateCursor = new QTimer(this);
    timerUpdateCursor->setSingleShot(true);

    connect(timerHighlight, &QTimer::timeout, this, &TextEdit::clearHighlights);
    connect(timerUpdateCursor, &QTimer::timeout, this, &TextEdit::resetUpdateCursor);
    connect(this, &TextEdit::updateCursor, this, &TextEdit::drawGraphicCursor);

    connect(client_, &Client::insertSymbol, this, &TextEdit::showSymbol);
    connect(client_, &Client::insertSymbolWithId, this, &TextEdit::showSymbolWithId);
    connect(client_, &Client::insertSymbolWithStyle, this, &TextEdit::showSymbolWithStyle);

    connect(client_, &Client::updateUserslist, this, &TextEdit::updateListParticipants);

    connect(client_, &Client::eraseSymbols, this, &TextEdit::eraseSymbols);
    connect(client_, &Client::clearEditor, textEdit, &QTextEdit::clear);
    connect(client_, &Client::updateRemotePosition, this, &TextEdit::updateRemotePosition);
    // controllare che funziona
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &TextEdit::cursorPositionChanged);
    //stile
    connect(client_, &Client::changeStyle, this, &TextEdit::changeStyle);

    qRegisterMetaType<usersInFile>("std::map<std::string,std::string>");
    qRegisterMetaType<Symbol>("Symbol");
    qRegisterMetaType<std::vector<Symbol>>("std::vector<Symbol>");
    qRegisterMetaType<Style>("Style");
    qRegisterMetaType<json>("json");

    textEdit->installEventFilter(this);

    setCentralWidget(textEdit);

    setupFileActions();
    setupConnectedUsers();
    setupTextActions();

    {
        QToolBar *tb = addToolBar(tr("Logout"));
        tb->setStyleSheet(QString::fromUtf8("QToolButton:hover {background-color:#E6E7E8;border:1px;}"));
        const QIcon eyeIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/eye.png"));
        QAction *a = tb->addAction(eyeIcon, tr("Highlight Characters"), this, &TextEdit::highlightcharacter);

        QWidget *spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        tb->addWidget(spacer);

        const QIcon quitIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/logout.png"));
        a = tb->addAction(quitIcon, tr("&Logout"), this, [=]() {
            requestLogout();
            emit this->logout();
        });
        a->setShortcut(Qt::CTRL + Qt::Key_Q);


    }
    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);

    connect(textEdit->document(), &QTextDocument::modificationChanged, this, &QWidget::setWindowModified);

    setWindowModified(textEdit->document()->isModified());

#ifndef QT_NO_CLIPBOARD

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);

#endif

    textEdit->setFocus();
    //setCurrentFileName(QString());

#ifdef Q_OS_MACOS
    // Use dark text on light background on macOS, also in dark mode.
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    textEdit->setPalette(pal);
#endif
    resetCursors();
    resetText();
}

void TextEdit::clearHighlights() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCursor tempCursor = QTextCursor(cursor);

    tempCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
    tempCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, 1);
    textEdit->setTextCursor(tempCursor);
    textEdit->setTextBackgroundColor(Qt::white);
    textEdit->setTextCursor(cursor);
}

void TextEdit::resetUpdateCursor() {
    client_->updateChangesCursor = true;
}

void TextEdit::requestLogout() {
    client_->symbols.clear();
    json j = json{
            {"operation", "req_logout"},
            {"username",  client_->getUser().toStdString()},
    };
    client_->sendAtServer(j);
}

void TextEdit::closingFile() {
    client_->symbols.clear();
    json j = json{
            {"operation", "close_file"},
            {"filename",  client_->getFileName().toStdString()},
            {"username",  client_->getUser().toStdString()}
    };
    client_->sendAtServer(j);
}

void TextEdit::setupFileActions() {
    QToolBar *tb = addToolBar(tr("File Actions"));
    tb->setStyleSheet(QString::fromUtf8("QToolButton:hover {background-color:#E6E7E8;border:1px;}"));

    const QIcon backIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/left-arrow.png"));
    QAction *a = tb->addAction(backIcon, tr("&Close File"), this, [=]() {
        closingFile();
        emit this->closeFile();
    });

#ifndef QT_NO_PRINTER

    const QIcon exportPdfIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/exportpdf.png"));
    a = tb->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);

#endif

}

void TextEdit::setupEditActions() {
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));


#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menu->addAction(cutIcon, tr("Cu&t"), textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

void TextEdit::setupTextActions() {
    QToolBar *tb = addToolBar(tr("Format Actions"));
    tb->setStyleSheet(QString::fromUtf8("QToolButton:hover {background-color:#E6E7E8;border:1px;}"));

    const QIcon boldIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = tb->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = tb->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("shared-editor", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = tb->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    actionTextUnderline->setCheckable(true);

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = tb->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, QOverload<const QString &>::of(&QComboBox::activated), this, &TextEdit::textSize);
}

void TextEdit::updateConnectedUser(QString user, QString color) {
    QLabel *label = new QLabel(user);
    label->setObjectName(user);
    label->setStyleSheet(QString::fromUtf8("QLabel{\n"
                                           "font-size:16px;\n"
                                           "font-weight:bold;\n"
                                           "color:") + color + QString::fromUtf8(";}"));

    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
}

void TextEdit::setupConnectedUsers() {
    QDockWidget *dock = new QDockWidget(tr("Users Online"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connectedUsers = new QListWidget(dock);
    QLabel *label = new QLabel(client_->getUser());
    label->setStyleSheet(
            QString::fromUtf8("QLabel{font-weight:bold; color:") + client_->getColor() + QString::fromUtf8(";}"));
    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
    dock->setWidget(connectedUsers);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void TextEdit::filePrintPdf() {

#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                                     .arg(QDir::toNativeSeparators(fileName)));
//! [0]
#endif

}

void TextEdit::textBold() {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        actionTextBold->isChecked() ? textEdit->setFontWeight(QFont::Bold) : textEdit->setFontWeight(QFont::Normal);
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.hasSelection())
            return;
        bool bold = actionTextBold->isChecked();
        std::vector<std::string> usernameToChange;
        std::vector<wchar_t> charToChange;
        std::vector<std::vector<int>> crdtToChange;
        int i = cursor.selectionStart();
        int count = 0;
        for (auto style = client_->symbols.begin() + cursor.selectionStart();
             style != client_->symbols.begin() + cursor.selectionEnd(); ++style) {
            QTextCharFormat fmt = style->getTextCharFormat();
            fmt.setFontWeight(bold ? QFont::Bold : QFont::Normal);
            cursor.mergeCharFormat(fmt);
            textEdit->mergeCurrentCharFormat(fmt);

            usernameToChange.push_back(client_->symbols[i].getUsername());
            charToChange.push_back(client_->symbols[i].getCharacter());
            crdtToChange.push_back(client_->symbols[i].getPosizione());
            client_->symbols[i].symbolStyle.setBold(bold);
            i++;
            if (++count == client_->maxBufferSymbol) {
                json j = json{
                        {"operation",        "styleChanged"},
                        {"usernameToChange", usernameToChange},
                        {"charToChange",     charToChange},
                        {"crdtToChange",     crdtToChange},
                        {"bold",             bold}
                };
                client_->sendAtServer(j);
                usernameToChange.clear();
                charToChange.clear();
                crdtToChange.clear();
                count = 0;
            }
        }
        if (!usernameToChange.empty()) {
            json j = json{
                    {"operation",        "styleChanged"},
                    {"usernameToChange", usernameToChange},
                    {"charToChange",     charToChange},
                    {"crdtToChange",     crdtToChange},
                    {"bold",             bold}
            };
            client_->sendAtServer(j);
        }
    }

}

void TextEdit::textUnderline() {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        textEdit->setFontUnderline(actionTextUnderline->isChecked());
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.hasSelection())
            return;
        bool underlined = actionTextUnderline->isChecked();
        std::vector<std::string> usernameToChange;
        std::vector<wchar_t> charToChange;
        std::vector<std::vector<int>> crdtToChange;
        int i = cursor.selectionStart();
        int count = 0;
        for (auto style = client_->symbols.begin() + cursor.selectionStart();
             style != client_->symbols.begin() + cursor.selectionEnd(); ++style) {
            QTextCharFormat fmt = style->getTextCharFormat();
            fmt.setFontUnderline(underlined);
            cursor.mergeCharFormat(fmt);
            textEdit->mergeCurrentCharFormat(fmt);

            usernameToChange.push_back(client_->symbols[i].getUsername());
            charToChange.push_back(client_->symbols[i].getCharacter());
            crdtToChange.push_back(client_->symbols[i].getPosizione());
            client_->symbols[i].symbolStyle.setUnderlined(underlined);
            i++;
            if (++count == client_->maxBufferSymbol) {
                json j = json{
                        {"operation",        "styleChanged"},
                        {"usernameToChange", usernameToChange},
                        {"charToChange",     charToChange},
                        {"crdtToChange",     crdtToChange},
                        {"underlined",       underlined}};
                client_->sendAtServer(j);
                usernameToChange.clear();
                charToChange.clear();
                crdtToChange.clear();
                count = 0;
            }
        }
        if (!usernameToChange.empty()) {
            json j = json{
                    {"operation",        "styleChanged"},
                    {"usernameToChange", usernameToChange},
                    {"charToChange",     charToChange},
                    {"crdtToChange",     crdtToChange},
                    {"underlined",       underlined}};
            client_->sendAtServer(j);
        }
    }
}

void TextEdit::textItalic() {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        textEdit->setFontItalic(actionTextItalic->isChecked());
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.hasSelection())
            return;
        bool italic = actionTextItalic->isChecked();
        std::vector<std::string> usernameToChange;
        std::vector<wchar_t> charToChange;
        std::vector<std::vector<int>> crdtToChange;
        int i = cursor.selectionStart();
        int count = 0;
        for (auto style = client_->symbols.begin() + cursor.selectionStart();
             style != client_->symbols.begin() + cursor.selectionEnd(); ++style) {
            QTextCharFormat fmt = style->getTextCharFormat();
            fmt.setFontItalic(italic);
            cursor.mergeCharFormat(fmt);
            textEdit->mergeCurrentCharFormat(fmt);

            usernameToChange.push_back(client_->symbols[i].getUsername());
            charToChange.push_back(client_->symbols[i].getCharacter());
            crdtToChange.push_back(client_->symbols[i].getPosizione());
            client_->symbols[i].symbolStyle.setItalic(italic);
            i++;
            if (++count == client_->maxBufferSymbol) {
                json j = json{
                        {"operation",        "styleChanged"},
                        {"usernameToChange", usernameToChange},
                        {"charToChange",     charToChange},
                        {"crdtToChange",     crdtToChange},
                        {"italic",           italic}};
                client_->sendAtServer(j);
                usernameToChange.clear();
                charToChange.clear();
                crdtToChange.clear();
                count = 0;
            }
        }
        if (!usernameToChange.empty()) {
            json j = json{
                    {"operation",        "styleChanged"},
                    {"usernameToChange", usernameToChange},
                    {"charToChange",     charToChange},
                    {"crdtToChange",     crdtToChange},
                    {"italic",           italic}};
            client_->sendAtServer(j);
        }
    }
}

void TextEdit::textColor() {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        QColor color = QColorDialog::getColor(textEdit->textColor(), this);
        if (!color.isValid())
            return;
        textEdit->setTextColor(color);
        QPixmap pix(16, 16);
        pix.fill(color);
        actionTextColor->setIcon(pix);

        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.hasSelection())
            return;
        std::vector<std::string> usernameToChange;
        std::vector<wchar_t> charToChange;
        std::vector<std::vector<int>> crdtToChange;
        int i = cursor.selectionStart();
        int count = 0;
        for (auto style = client_->symbols.begin() + cursor.selectionStart();
             style != client_->symbols.begin() + cursor.selectionEnd(); ++style) {
            QTextCharFormat fmt = style->getTextCharFormat();
            fmt.setForeground(color);
            cursor.mergeCharFormat(fmt);
            textEdit->mergeCurrentCharFormat(fmt);

            usernameToChange.push_back(client_->symbols[i].getUsername());
            charToChange.push_back(client_->symbols[i].getCharacter());
            crdtToChange.push_back(client_->symbols[i].getPosizione());
            client_->symbols[i].symbolStyle.setColor(color.name().toStdString());
            i++;
            if (++count == client_->maxBufferSymbol) {
                json j = json{
                        {"operation",        "styleChanged"},
                        {"usernameToChange", usernameToChange},
                        {"charToChange",     charToChange},
                        {"crdtToChange",     crdtToChange},
                        {"color",            color.name().toStdString()}};
                client_->sendAtServer(j);
                usernameToChange.clear();
                charToChange.clear();
                crdtToChange.clear();
                count = 0;
            }
        }
        if (!usernameToChange.empty()) {
            json j = json{
                    {"operation",        "styleChanged"},
                    {"usernameToChange", usernameToChange},
                    {"charToChange",     charToChange},
                    {"crdtToChange",     crdtToChange},
                    {"color",            color.name().toStdString()}};
            client_->sendAtServer(j);
        }
    }
}

void TextEdit::textFamily(const QString &f) {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        textEdit->setFontFamily(f);
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.hasSelection())
            return;
        int i = cursor.selectionStart();
        std::vector<std::string> usernameToChange;
        std::vector<wchar_t> charToChange;
        std::vector<std::vector<int>> crdtToChange;
        int count = 0;
        for (auto style = client_->symbols.begin() + cursor.selectionStart();
             style != client_->symbols.begin() + cursor.selectionEnd(); ++style) {
            QTextCharFormat fmt = style->getTextCharFormat();
            fmt.setFontFamily(f);
            cursor.mergeCharFormat(fmt);
            textEdit->mergeCurrentCharFormat(fmt);

            usernameToChange.push_back(client_->symbols[i].getUsername());
            charToChange.push_back(client_->symbols[i].getCharacter());
            crdtToChange.push_back(client_->symbols[i].getPosizione());
            client_->symbols[i].symbolStyle.setFontFamily(f.toStdString());
            i++;
            if (++count == client_->maxBufferSymbol) {
                json j = json{
                        {"operation",        "styleChanged"},
                        {"usernameToChange", usernameToChange},
                        {"charToChange",     charToChange},
                        {"crdtToChange",     crdtToChange},
                        {"fontFamily",       f.toStdString()}};
                client_->sendAtServer(j);
                usernameToChange.clear();
                charToChange.clear();
                crdtToChange.clear();
                count = 0;
            }
        }
        if (!usernameToChange.empty()) {
            json j = json{
                    {"operation",        "styleChanged"},
                    {"usernameToChange", usernameToChange},
                    {"charToChange",     charToChange},
                    {"crdtToChange",     crdtToChange},
                    {"fontFamily",       f.toStdString()}};
            client_->sendAtServer(j);
        }
    }
}

void TextEdit::textSize(const QString &p) {
    QTextCursor cursor = textEdit->textCursor();
    if (cursor.selectionEnd() - cursor.selectionStart() > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        qreal pointSize = p.toFloat();
        textEdit->setFontPointSize(pointSize);
        if (p.toFloat() <= 0)
            return;

        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        if (!cursor.hasSelection())
            return;
        int i = cursor.selectionStart();
        std::vector<std::string> usernameToChange;
        std::vector<wchar_t> charToChange;
        std::vector<std::vector<int>> crdtToChange;
        int count = 0;
        for (auto style = client_->symbols.begin() + cursor.selectionStart();
             style != client_->symbols.begin() + cursor.selectionEnd(); ++style) {
            QTextCharFormat fmt = style->getTextCharFormat();
            fmt.setFontPointSize(p.toDouble());
            cursor.mergeCharFormat(fmt);
            textEdit->mergeCurrentCharFormat(fmt);

            usernameToChange.push_back(client_->symbols[i].getUsername());
            charToChange.push_back(client_->symbols[i].getCharacter());
            crdtToChange.push_back(client_->symbols[i].getPosizione());
            client_->symbols[i].symbolStyle.setFontSize(p.toInt());
            i++;
            if (++count == client_->maxBufferSymbol) {
                json j = json{
                        {"operation",        "styleChanged"},
                        {"usernameToChange", usernameToChange},
                        {"charToChange",     charToChange},
                        {"crdtToChange",     crdtToChange},
                        {"size",             p.toInt()}};
                client_->sendAtServer(j);
                usernameToChange.clear();
                charToChange.clear();
                crdtToChange.clear();
                count = 0;
            }
        }
        if (!usernameToChange.empty()) {
            json j = json{
                    {"operation",        "styleChanged"},
                    {"usernameToChange", usernameToChange},
                    {"charToChange",     charToChange},
                    {"crdtToChange",     crdtToChange},
                    {"size",             p.toInt()}};
            client_->sendAtServer(j);
        }
    }
}

void TextEdit::updateRemotePosition(QString user, int pos) {
    _cursorsVector[user].setPosition(pos);
    drawGraphicCursor();
}

void TextEdit::cursorPositionChanged() {
    QTextCursor cursor = textEdit->textCursor();
    if (client_->symbols.size() > cursor.position()) {
        actionTextBold->setChecked(client_->symbols[cursor.position()].symbolStyle.isBold());
        actionTextItalic->setChecked(client_->symbols[cursor.position()].symbolStyle.isItalic());
        actionTextUnderline->setChecked(client_->symbols[cursor.position()].symbolStyle.isUnderlined());
        QPixmap pix(16, 16);
        pix.fill(QColor(QString::fromStdString(client_->symbols[cursor.position()].symbolStyle.getColor())));
        actionTextColor->setIcon(pix);
        comboFont->setCurrentFont(
                QString::fromStdString(client_->symbols[cursor.position()].symbolStyle.getFontFamily()));
        comboSize->setCurrentIndex(QFontDatabase::standardSizes().indexOf(client_->symbols[cursor.position()].symbolStyle.getFontSize()));
    }
    if (client_->updateChangesCursor && !cursor.hasSelection()) {
        int pos = cursor.position();

        json j = json{
                {"operation", "update_cursorPosition"},
                {"username",  client_->getUser().toStdString()},
                {"pos",       pos}
        };
        client_->sendAtServer(j);
    }
}

void TextEdit::showSymbolWithId(Symbol symbolToInsert) {

    int pos = client_->generateIndexCRDT(symbolToInsert, 0, -1, -1);
    client_->insertSymbolIndex(symbolToInsert, pos);
    wchar_t c = symbolToInsert.getCharacter();
    QString user = QString::fromStdString(symbolToInsert.getUsername());

    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    format.setFontFamily("Helvetica");

    QTextCursor cur = textEdit->textCursor();

    cur.beginEditBlock();

    int endIndex;
    cur.hasSelection() ? endIndex = cur.selectionEnd() : endIndex = -90;
    int oldPos = pos < cur.position() ? cur.position() + 1 : cur.position();

    if (cur.hasSelection() && pos == endIndex) {

        int startIndex = cur.selectionStart();

        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));

        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::MoveAnchor);
        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::KeepAnchor);
    } else {
        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));
        cur.setPosition(oldPos);
    }
    cur.endEditBlock();

    textEdit->setTextCursor(cur);

    for (auto list:_listParticipantsAndColors) {
        if (list.first != user) {
            if (pos < _cursorsVector[list.first].position)
                _cursorsVector[list.first].setPosition(_cursorsVector[list.first].position + 1);
        } else {
            _cursorsVector[user].setPosition(pos + 1);
        }
    }
    //drawRemoteCursors();
    drawGraphicCursor();

    textEdit->setFocus();

}

void TextEdit::showSymbolWithStyle(Symbol symbolToInsert, bool open) {
    if (open) {
        client_->updateChangesCursor = false;
        timerUpdateCursor->start(5);
    }

    int pos = client_->generateIndexCRDT(symbolToInsert, 0, -1, -1);
    client_->insertSymbolIndex(symbolToInsert, pos);
    wchar_t c = symbolToInsert.getCharacter();
    QString user = QString::fromStdString(symbolToInsert.getUsername());

    QTextCharFormat format;

    bool wasBold = actionTextBold->isChecked();
    bool wasItalic = actionTextItalic->isChecked();
    bool wasUnderlined = actionTextUnderline->isChecked();
    QColor wasColor = textEdit->textColor();
    int wasSixe = textEdit->fontPointSize();
    std::string wasFontFamily = textEdit->fontFamily().toStdString();

    symbolToInsert.getSymbolStyle().isBold() ? format.setFontWeight(QFont::Bold) : format.setFontWeight(QFont::Normal);
    symbolToInsert.getSymbolStyle().isItalic() ? format.setFontItalic(true) : format.setFontItalic(false);
    symbolToInsert.getSymbolStyle().isUnderlined() ? format.setFontUnderline(true) : format.setFontUnderline(false);
    format.setFontFamily(QString::fromStdString(symbolToInsert.getSymbolStyle().getFontFamily()));
    format.setFontPointSize(symbolToInsert.getSymbolStyle().getFontSize());
    format.setForeground(QColor(QString::fromStdString(symbolToInsert.getSymbolStyle().getColor())));

    QTextCursor cur = textEdit->textCursor();
    cur.beginEditBlock();
    int endIndex;
    cur.hasSelection() ? endIndex = cur.selectionEnd() : endIndex = -90;
    int oldPos = pos < cur.position() ? cur.position() + 1 : cur.position();

    if (cur.hasSelection() && pos == endIndex) {

        int startIndex = cur.selectionStart();

        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));

        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::MoveAnchor);
        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::KeepAnchor);
    } else {
        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));
        cur.setPosition(oldPos);
    }
    cur.endEditBlock();

    wasBold ? format.setFontWeight(QFont::Bold) : format.setFontWeight(QFont::Normal);
    textEdit->setFontItalic(wasItalic);
    textEdit->setFontUnderline(wasUnderlined);
    textEdit->setFontFamily(QString::fromStdString(wasFontFamily));
    textEdit->setFontPointSize(wasSixe);
    textEdit->setTextColor(wasColor);

    textEdit->setTextCursor(cur);

    for (auto list:_listParticipantsAndColors) {
        if (list.first != user) {
            if (pos < _cursorsVector[list.first].position)
                _cursorsVector[list.first].setPosition(_cursorsVector[list.first].position + 1);
        } else {
            _cursorsVector[user].setPosition(pos + 1);
        }
    }
    //drawRemoteCursors();
    drawGraphicCursor();

    textEdit->setFocus();

}

void TextEdit::showSymbol(int pos, QChar c) {
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    format.setFontFamily(DEFAULT_FONT_FAMILY);

    QTextCursor cur = textEdit->textCursor();

    cur.beginEditBlock();

    int endIndex;
    cur.hasSelection() ? endIndex = cur.selectionEnd() : endIndex = -90;
    int oldPos = pos < cur.position() ? cur.position() + 1 : cur.position();

    if (cur.hasSelection() && pos == endIndex) {

        int startIndex = cur.selectionStart();

        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));

        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::MoveAnchor);
        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::KeepAnchor);
    } else {
        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));
        cur.setPosition(oldPos);
    }
    cur.endEditBlock();

    textEdit->setTextCursor(cur);
    textEdit->setFocus();
}

void TextEdit::clipboardDataChanged() {}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format) {
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont &f) {
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

bool TextEdit::eventFilter(QObject *obj, QEvent *ev) {
    if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *key_ev = static_cast<QKeyEvent *>(ev);
        int key = key_ev->key();
        /*std::cout << "FILE CRDT: " << std::flush; //print crdt
        for (auto iterPositions = client_->symbols.begin(); iterPositions != client_->symbols.end(); ++iterPositions) {
            if (iterPositions->getCharacter() != 10 && iterPositions->getCharacter() != 13)
                std::wcout << "{" << (int) iterPositions->getCharacter() << "(" << iterPositions->getCharacter()
                           << ") - [" << std::flush;
            else
                std::wcout << "[" << (int) iterPositions->getCharacter() << "(\\n) - [" << std::flush;
            for (int i = 0; i < iterPositions->getPosizione().size(); i++)
                std::cout << std::to_string(iterPositions->getPosizione()[i]) << " " << std::flush;
            std::cout << "] - " << iterPositions->getUsername() << std::flush;
            std::cout << "}" << std::flush;
        }
        std::cout << std::endl;*/
        if (obj == textEdit) {
            if (!key_ev->text().isEmpty()) {
                QTextCursor cursor = textEdit->textCursor();
                int pos, startIndex, endIndex;

                //cancella caratteri se sono selezionati (tranne per ctrl+a e ctrl+c
                if (cursor.hasSelection() && key_ev->text().toStdString().c_str()[0] != 3 &&
                    key_ev->text().toStdString().c_str()[0] != 1) {
                    client_->updateChangesCursor = false;
                    timerUpdateCursor->start(1000);
                    startIndex = cursor.selectionStart();
                    endIndex = cursor.selectionEnd();
                    pos = startIndex;

                    cursor.beginEditBlock();
                    cursor.setPosition(startIndex, QTextCursor::MoveAnchor);
                    cursor.setPosition(endIndex, QTextCursor::KeepAnchor);
                    textEdit->setTextCursor(cursor);
                    cursor.endEditBlock();

                    std::vector<Symbol> symbolsToErase;
                    std::vector<std::string> usernameToErase;
                    std::vector<wchar_t> charToErase;
                    std::vector<std::vector<int>> crdtToErase;

                    int dim = endIndex - startIndex;
                    int count = 0;
                    for (int iter = 0; iter < dim; iter++) {
                        symbolsToErase.push_back(client_->symbols.at(iter + startIndex));
                        usernameToErase.push_back(client_->symbols.at(iter + startIndex).getUsername());
                        charToErase.push_back(client_->symbols.at(iter + startIndex).getCharacter());
                        crdtToErase.push_back(client_->symbols.at(iter + startIndex).getPosizione());
                        if (++count == client_->maxBufferSymbol) {
                            client_->eraseSymbolCRDT(symbolsToErase);
                            iter -= client_->maxBufferSymbol;
                            dim -= client_->maxBufferSymbol;
                            json j = json{
                                    {"operation",       "remove"},
                                    {"usernameToErase", usernameToErase},
                                    {"charToErase",     charToErase},
                                    {"crdtToErase",     crdtToErase}
                            };
                            client_->sendAtServer(j);
                            symbolsToErase.clear();
                            usernameToErase.clear();
                            charToErase.clear();
                            crdtToErase.clear();
                            count = 0;
                        }
                    }
                    if (!symbolsToErase.empty()) {
                        client_->eraseSymbolCRDT(symbolsToErase);
                        json j = json{
                                {"operation",       "remove"},
                                {"usernameToErase", usernameToErase},
                                {"charToErase",     charToErase},
                                {"crdtToErase",     crdtToErase}
                        };
                        client_->sendAtServer(j);
                    }
                    decreentPosition(startIndex, dim);
                    drawGraphicCursor();
                } else {
                    pos = cursor.position();
                }

                if (key != Qt::Key_Backspace &&
                    key != Qt::Key_Delete &&
                    key != Qt::Key_Escape &&
                    key_ev->text().toStdString().c_str()[0] != 22 && //paste
                    key_ev->text().toStdString().c_str()[0] != 24 && //cut
                    key_ev->text().toStdString().c_str()[0] != 3 && //copy
                    key_ev->text().toStdString().c_str()[0] != 26 && //ctrl+z
                    key_ev->text().toStdString().c_str()[0] != 1) { //ctrl+a
                    //caso carattere normale (lettere e spazio)

                    wchar_t c = key_ev->text().toStdWString().c_str()[0];

                    //prendo lo stile
                    bool isBold = textEdit->fontWeight() == QFont::Bold;
                    int size = textEdit->fontPointSize();
                    std::string fontFamily = textEdit->fontFamily().toStdString();
                    if (fontFamily == "")
                        fontFamily = DEFAULT_FONT_FAMILY;
                    if (size == 0)
                        size = DEFAULT_FONT_SIZE;
                    std::string color = textEdit->textColor().name().toStdString();
                    Style style = {isBold, textEdit->fontItalic(), textEdit->fontUnderline(), fontFamily, size, color};
                    //std::vector<int> crdt = client_->insertSymbolNewCRDT(pos, c, client_->getUser().toStdString());
                    std::vector<int> crdt = client_->insertSymbolNewCRDT(pos, c, client_->getUser().toStdString(),
                                                                         style);
                    textEdit->setTextCursor(cursor);
                    //emit updateCursor();

                    std::vector<std::string> usernamev;
                    usernamev.push_back(client_->getUser().toStdString());
                    std::vector<wchar_t> charv;
                    charv.push_back(c);
                    std::vector<std::vector<int>> crdtv;
                    crdtv.push_back(crdt);
                    json j = json{
                            {"operation",        "insertAndStyle"},
                            {"usernameToInsert", usernamev},
                            {"charToInsert",     charv},
                            {"crdtToInsert",     crdtv},
                            {"bold",             isBold},
                            {"italic",           textEdit->fontItalic()},
                            {"underlined",       textEdit->fontUnderline()},
                            {"color",            color},
                            {"fontFamily",       fontFamily},
                            {"size",             size}
                    };
                    client_->sendAtServer(j);
                    //drawRemoteCursors();
                    drawGraphicCursor();
                    incrementPosition(pos, 1);

                    //return QObject::eventFilter(obj, ev);
                }
                    //*********************PASTE*****************************************
                else if (key_ev->text().toStdString().c_str()[0] == 22) {
                    QClipboard *clipboard = QGuiApplication::clipboard();
                    QString pastedText = clipboard->text();
                    textEdit->setTextCursor(cursor);

                    std::vector<std::string> usernameToInsert;
                    std::vector<wchar_t> charToInsert;
                    std::vector<std::vector<int>> crdtToInsert;
                    std::vector<int> crdt;
                    wchar_t c;
                    int startPos = pos;
                    int dim = pastedText.size();
                    int count = 0;
                    for (int iter = 0; iter < dim; iter++) {
                        textEdit->setTextCursor(cursor);
                        usernameToInsert.push_back(client_->getUser().toStdString());
                        c = pastedText.toStdWString().c_str()[iter];
                        charToInsert.push_back(c);
                        crdt = client_->insertSymbolNewCRDT(pos, c, client_->getUser().toStdString());
                        crdtToInsert.push_back(crdt);
                        pos++;
                        if (++count == client_->maxBufferSymbol) {
                            json j = json{
                                    {"operation",        "insert"},
                                    {"usernameToInsert", usernameToInsert},
                                    {"charToInsert",     charToInsert},
                                    {"crdtToInsert",     crdtToInsert}
                            };
                            client_->sendAtServer(j);

                            usernameToInsert.clear();
                            charToInsert.clear();
                            crdtToInsert.clear();
                            count = 0;
                        }
                    }
                    if (!usernameToInsert.empty()) {
                        json j = json{
                                {"operation",        "insert"},
                                {"usernameToInsert", usernameToInsert},
                                {"charToInsert",     charToInsert},
                                {"crdtToInsert",     crdtToInsert}
                        };
                        client_->sendAtServer(j);
                    }
                    drawGraphicCursor();
                    incrementPosition(startPos, dim);
                }
                    //*********************COPY*****************************************
                else if (key_ev->text().toStdString().c_str()[0] == 3) {
                    //do nothing
                }
                    //*********************CUT*****************************************
                else if (key_ev->text().toStdString().c_str()[0] == 24) {
                    //do nothing
                }
                    //*********************CTRL+A*****************************************
                else if (key_ev->text().toStdString().c_str()[0] == 1) {
                    //do nothing
                }
                    //*********************BACKSPACE*************************************
                else if (key == Qt::Key_Backspace) {
                    if (!cursor.hasSelection()) { //gia' cancellato
                        //return QObject::eventFilter(obj, ev);
                        if (pos > 0) {
                            std::vector<Symbol> symbolsToErase;
                            std::vector<std::string> usernameToErase;
                            std::vector<wchar_t> charToErase;
                            std::vector<std::vector<int>> crdtToErase;
                            symbolsToErase.push_back(client_->symbols[pos - 1]);
                            usernameToErase.push_back(client_->symbols[pos - 1].getUsername());
                            charToErase.push_back(client_->symbols[pos - 1].getCharacter());
                            crdtToErase.push_back(client_->symbols[pos - 1].getPosizione());

                            client_->eraseSymbolCRDT(symbolsToErase);

                            json j = json{
                                    {"operation",       "remove"},
                                    {"usernameToErase", usernameToErase},
                                    {"charToErase",     charToErase},
                                    {"crdtToErase",     crdtToErase}
                            };
                            client_->sendAtServer(j);
                            decreentPosition(pos, 1);
                            drawGraphicCursor();
                        }
                    }
                    //return QObject::eventFilter(obj, ev);
                }
                    //**********************CANC****************************/
                else if (key == Qt::Key_Delete) {
                    if (cursor.hasSelection()) //gia' cancellato
                        return QObject::eventFilter(obj, ev);
                    if (pos >= 0 && pos < textEdit->toPlainText().size()) {
                        std::vector<Symbol> symbolsToErase;
                        std::vector<std::string> usernameToErase;
                        std::vector<wchar_t> charToErase;
                        std::vector<std::vector<int>> crdtToErase;
                        symbolsToErase.push_back(client_->symbols[pos]);
                        usernameToErase.push_back(client_->symbols[pos].getUsername());
                        charToErase.push_back(client_->symbols[pos].getCharacter());
                        crdtToErase.push_back(client_->symbols[pos].getPosizione());

                        client_->eraseSymbolCRDT(symbolsToErase);

                        json j = json{
                                {"operation",       "remove"},
                                {"usernameToErase", usernameToErase},
                                {"charToErase",     charToErase},
                                {"crdtToErase",     crdtToErase}
                        };
                        client_->sendAtServer(j);
                    }
                    decreentPosition(pos, 1);
                    drawGraphicCursor();
                    //return QObject::eventFilter(obj, ev);
                }
            }
        }
    }
    return QObject::eventFilter(obj, ev);;
}

void TextEdit::eraseSymbols(std::vector<Symbol> symbolsToErase) {

    client_->updateChangesCursor = false;
    timerUpdateCursor->start(symbolsToErase.size() * 5);

    std::vector<int> erased = client_->eraseSymbolCRDT(symbolsToErase);

    for (int i = 0; i < erased.size(); i++) {
        int toErase = erased[i];
        std::string username = symbolsToErase[i].getUsername();
        QTextCursor cur = textEdit->textCursor();

        cur.beginEditBlock();
        cur.setPosition(toErase - 1);
        int startAlignment = cur.blockFormat().alignment();

        /* erase symbols */
        cur.setPosition(toErase - 1);
        cur.setPosition(toErase, QTextCursor::KeepAnchor);
        cur.removeSelectedText();
        cur.setPosition(toErase - 1, QTextCursor::KeepAnchor);
        cur.removeSelectedText();

        QTextBlockFormat textBlockFormat;
        textBlockFormat = cur.blockFormat();
        textBlockFormat.setAlignment(static_cast<Qt::AlignmentFlag>(startAlignment));
        cur.mergeBlockFormat(textBlockFormat);

        cur.endEditBlock();
        for (auto list:_listParticipantsAndColors) {
            if (list.first.toStdString() != username) {
                if (_cursorsVector[list.first].position > toErase)
                    _cursorsVector[list.first].setPosition(_cursorsVector[list.first].position - 1);
            } else {
                _cursorsVector[QString::fromStdString(username)].setPosition(toErase - 1);
            }
        }
        drawGraphicCursor();
        textEdit->setFocus();
    }
}

QString TextEdit::getFileName() const {
    return fileName;
}

void TextEdit::drawRemoteCursors() {
    QTextCursor cursor = QTextCursor(textEdit->textCursor());
    QTextCursor tempCursor = QTextCursor(cursor);
    tempCursor.clearSelection();
    tempCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
    tempCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, 1);
    textEdit->setTextCursor(tempCursor);
    textEdit->setTextBackgroundColor(QColor(255, 255, 255, 255));
    textEdit->setTextCursor(cursor);
//    textEdit->setTextBackgroundColor(client_->getColor());
    textEdit->setTextBackgroundColor(QColor(255, 255, 255, 255));
    for (std::pair<QString, CustomCursor> cPair : _cursorsVector) {
        tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
        CustomCursor cCursor = cPair.second;
        if (cCursor.hasSelection) {
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor,
                                    (int) cCursor.selectionStart);
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor,
                                    (int) cCursor.selectionEnd - (int) cCursor.selectionStart);
        } else {
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor,
                                    (int) cCursor.position);
            if (cCursor.position == 0) {
                tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, 1);
            } else {
                tempCursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor, 1);
            }
        }
        textEdit->setTextCursor(tempCursor);
        textEdit->setTextBackgroundColor(_listParticipantsAndColors[cPair.first]);
        //_ui->textEdit->setTextCursor(myCursor);
        tempCursor.clearSelection();
        tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    }
    textEdit->setTextCursor(cursor);
    textEdit->setTextBackgroundColor(QColor{255, 255, 255, 255});

}

void TextEdit::highlightcharacter() {
    QTextCursor cursor = textEdit->textCursor();
    QTextCursor tempCursor = QTextCursor(cursor);
    int start, end;
    if (cursor.hasSelection()) {
        start = cursor.selectionStart();
        end = cursor.selectionEnd();
        cursor.clearSelection();
    } else {
        start = 0;
        end = client_->symbols.size();
    }
    if ((end - start) > MAX_ALLOWED_CHARS) {
        this->alert();
    } else {
        int pos = 0;
        client_->updateChangesCursor = false;
        timerUpdateCursor->start(1000);
        for (auto s = client_->symbols.begin() + start; s != client_->symbols.begin() + end; ++s) {
            if (_listParticipantsAndColors.count(QString::fromStdString(s->getUsername())) == 1 ||
                s->getUsername() == client_->getUser().toStdString()) {
                tempCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
                tempCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, pos + start);
                tempCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                textEdit->setTextCursor(tempCursor);
                s->getUsername() == client_->getUser().toStdString() ? textEdit->setTextBackgroundColor(
                        client_->getColor()) : textEdit->setTextBackgroundColor(
                        _listParticipantsAndColors[QString::fromStdString(s->getUsername())]);
            }
            pos++;
        }
        textEdit->setTextCursor(cursor);
        textEdit->setTextBackgroundColor(QColor(255, 255, 255, 255));
        textEdit->setFocus();
        timerHighlight->start(3000);
    }
}

void TextEdit::drawGraphicCursor() {

    for (auto list:_listParticipantsAndColors) {
        QTextCursor cursor = QTextCursor(textEdit->document());
        QTextCursor tempCur = QTextCursor(cursor);
        tempCur.setPosition(_cursorsVector[list.first].position);
        QRect qRect = textEdit->cursorRect(tempCur);
        QPixmap pix(qRect.width() * 2.5, qRect.height());
        pix.fill(list.second);
        _labels[list.first]->setPixmap(pix);
        _labels[list.first]->move(qRect.left(), qRect.top());
        _labels[list.first]->show();
    }
}

void TextEdit::incrementPosition(int pos, int count) {
    for (auto list:_listParticipantsAndColors) {
        if (pos < _cursorsVector[list.first].position) {
            _cursorsVector[list.first].setPosition(_cursorsVector[list.first].position + count);
        }
    }
}

void TextEdit::decreentPosition(int pos, int count) {
    for (auto list:_listParticipantsAndColors) {
        if (pos < _cursorsVector[list.first].position) {
            _cursorsVector[list.first].setPosition(_cursorsVector[list.first].position - count);
        }
    }
}

void TextEdit::updateListParticipants(usersInFile users) {
    for (auto user:_listParticipantsAndColors) {
        _labels[user.first]->move(0, 0);
        _labels[user.first]->hide();
    }
    _listParticipantsAndColors.clear();
    _labels.clear();
    //NON POSSO ELIMINARLO, contiene la posizione di tutti gli utenti
    //_cursorsVector.clear();
    for (auto u:users) {
        if (u.first != client_->getUser().toStdString()) {
            CustomCursor remoteCursor = CustomCursor();
            _cursorsVector.insert(std::pair<QString, CustomCursor>(QString::fromStdString(u.first), remoteCursor));
            _listParticipantsAndColors.insert(
                    std::pair<QString, QColor>(QString::fromStdString(u.first), QString::fromStdString(u.second)));
            QLabel *labelName = new QLabel(textEdit);
            if (_labels.insert(std::pair<QString, QLabel *>(QString::fromStdString(u.first), labelName)).second) {
                //nel coso non c'èera in labels ma c'era in _cursorvector, in ogni caso appena qualcuno entra, la posizione è zero
                // _cursorsVector[QString::fromStdString(u.first)].setPosition(0);
            }
        }
    }


    //drawRemoteCursors();
    drawGraphicCursor();
    updateConnectedUsers(users);

}

void TextEdit::updateConnectedUsers(usersInFile users) {
    connectedUsers->clear();
    QLabel *label = new QLabel(client_->getUser());
    //QPushButton *label = new QPushButton(client_->getUser());
    label->setObjectName(client_->getUser());
    label->setStyleSheet(QString::fromUtf8("QLabel{\n"
                                           "font-size:16px;\n"
                                           "font-weight:bold;\n"
                                           "color:") + client_->getColor() + QString::fromUtf8(";}"));

    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
    for (auto u:_listParticipantsAndColors) {
        if (u.first.toStdString() != client_->getUser().toStdString())
            updateConnectedUser(u.first, u.second.name());
    }
}

void TextEdit::resetText() {
    _currentText = QString(textEdit->toPlainText());
}

void TextEdit::resetCursors() {

    _oldCursor = CustomCursor(QTextCursor(textEdit->textCursor()));
    _newCursor = CustomCursor(QTextCursor(textEdit->textCursor()));
}

void TextEdit::updateCursors(const CustomCursor &cursor) {
    _oldCursor = CustomCursor(_newCursor);
    _newCursor = CustomCursor(cursor);
}

void TextEdit::changeStyle(json js) {
    QTextCursor cursor = textEdit->textCursor();
    cursor.beginEditBlock();

    std::vector<Symbol> symbolsToChange;
    for (int i = 0; i < js.at("usernameToChange").get<std::vector<std::string>>().size(); i++) {
        symbolsToChange.emplace_back(Symbol(js.at("charToChange").get<std::vector<wchar_t>>()[i],
                                            js.at("usernameToChange").get<std::vector<std::string>>()[i],
                                            js.at("crdtToChange").get<std::vector<std::vector<int>>>()[i]));
    }

    int lastFound = 0;
    for (auto iterSymbolsToChange = symbolsToChange.begin();
         iterSymbolsToChange != symbolsToChange.end(); ++iterSymbolsToChange) {
        if ((lastFound - 2) >= 0)
            lastFound -= 2;
        else if ((lastFound - 1) >= 0)
            lastFound--;
        int count = lastFound;
        bool foundSecondPart = false;
        for (auto iterSymbols = client_->symbols.begin() + lastFound; iterSymbols !=
                                                                      client_->symbols.end(); ++iterSymbols) { //cerca prima da dove hai trovato prima in poi
            if (*iterSymbolsToChange == *iterSymbols) {
                QTextCharFormat oldFormat = iterSymbols->getTextCharFormat();
                if (js.contains("bold"))
                    oldFormat.setFontWeight(js.at("bold").get<bool>() ? QFont::Bold : QFont::Normal);
                if (js.contains("underlined"))
                    oldFormat.setFontUnderline(js.at("underlined").get<bool>());
                if (js.contains("italic"))
                    oldFormat.setFontItalic(js.at("italic").get<bool>());
                if (js.contains("color"))
                    oldFormat.setForeground(QColor(QString::fromStdString(js.at("color").get<std::string>())));
                if (js.contains("fontFamily"))
                    oldFormat.setFontFamily(QString::fromStdString(js.at("fontFamily").get<std::string>()));
                if (js.contains("size"))
                    oldFormat.setFontPointSize(js.at("size").get<int>());
                cursor.setPosition(count);
                cursor.setPosition(count + 1, QTextCursor::KeepAnchor);
                cursor.mergeCharFormat(oldFormat);
                client_->symbols[count].symbolStyle.setTextCharFormat(oldFormat);
                lastFound = count;
                foundSecondPart = true;
                break;
            }
            count++;
        }
        if (!foundSecondPart) { //se non hai trovato cerca anche nella prima parte
            for (auto iterSymbols = client_->symbols.begin();
                 iterSymbols != client_->symbols.begin() + lastFound; ++iterSymbols) {
                if (*iterSymbolsToChange == *iterSymbols) {
                    QTextCharFormat oldFormat = iterSymbols->getTextCharFormat();
                    if (js.contains("bold"))
                        oldFormat.setFontWeight(js.at("bold").get<bool>() ? QFont::Bold : QFont::Normal);
                    if (js.contains("underlined"))
                        oldFormat.setFontUnderline(js.at("underlined").get<bool>());
                    if (js.contains("italic"))
                        oldFormat.setFontItalic(js.at("italic").get<bool>());
                    if (js.contains("color"))
                        oldFormat.setForeground(QColor(QString::fromStdString(js.at("color").get<std::string>())));
                    if (js.contains("fontFamily"))
                        oldFormat.setFontFamily(QString::fromStdString(js.at("fontFamily").get<std::string>()));
                    if (js.contains("size"))
                        oldFormat.setFontPointSize(js.at("size").get<int>());
                    cursor.setPosition(count);
                    cursor.setPosition(count + 1, QTextCursor::KeepAnchor);
                    cursor.mergeCharFormat(oldFormat);
                    client_->symbols[count].symbolStyle.setTextCharFormat(oldFormat);
                    lastFound = count;
                    break;
                }
                count++;
            }
        }
    }

    cursor.endEditBlock();

    textEdit->setFocus();
}

void TextEdit::alert() {
    QMessageBox *msgB = new QMessageBox(QMessageBox::Critical, tr("Alert"), tr("Select fewer characters!"),
                                        QMessageBox::NoButton, this, Qt::CustomizeWindowHint);

    msgB->setIconPixmap(QPixmap(rsrcPath + QString::fromUtf8("/triangle.png")));
    msgB->show();
}
