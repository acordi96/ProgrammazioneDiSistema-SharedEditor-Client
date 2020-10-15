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

TextEdit::TextEdit(Client *c, QWidget *parent)
        : QMainWindow(parent), client_(c) {
#ifdef Q_OS_MACOS
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    textEdit = new QTextEdit(this);
    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, &TextEdit::clearHighlights);
    connect(this, &TextEdit::updateCursor, this, &TextEdit::drawGraphicCursor);

    connect(client_, &Client::insertSymbol, this, &TextEdit::showSymbol);
    connect(client_, &Client::insertSymbolWithId, this, &TextEdit::showSymbolWithId);
    connect(client_, &Client::updateUserslist, this, &TextEdit::updateListParticipants);

    connect(client_, &Client::eraseSymbols, this, &TextEdit::eraseSymbols);
    connect(client_, &Client::clearEditor, textEdit, &QTextEdit::clear);
    connect(client_, &Client::updateRemotePosition, this, &TextEdit::updateRemotePosition);
    // controllare che funziona
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &TextEdit::cursorPositionChanged);
    //aggiorno lista

    qRegisterMetaType<usersInFile>("std::map<std::string,std::string>");
    qRegisterMetaType<Symbol>("Symbol");
    qRegisterMetaType<std::vector<Symbol>>("std::vector<Symbol>");

    textEdit->installEventFilter(this);

    setCentralWidget(textEdit);

    setupFileActions();
    setupConnectedUsers();

    /*{
        QMenu *userMenu = menuBar()->addMenu(client_->getUser());
        userMenu->addAction(tr("Logout"),this,[=](){
            requestLogout();
            emit this->logout();
        });
    }*/
    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);

    connect(textEdit->document(), &QTextDocument::modificationChanged, this, &QWidget::setWindowModified);

    setWindowModified(textEdit->document()->isModified());

#ifndef QT_NO_CLIPBOARD

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);

#endif

    textEdit->setFocus();
    setCurrentFileName(QString());

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

void TextEdit::closeEvent(QCloseEvent *e) {
    if (maybeSave())
        e->accept();
    else
        e->ignore();
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

    const QIcon backIcon = QIcon::fromTheme("go-back", QIcon(rsrcPath + "/left-arrow.png"));
    QAction *a = tb->addAction(backIcon, tr("&Close File"), this, [=]() {
        closingFile();
        emit this->closeFile();
    });
/*
        QPushButton *goback = new QPushButton(menuBar());
        goback->setIcon(backIcon);
        goback->setFlat(true);
        QObject::connect(goback,&QPushButton::clicked,this,[=](){
            closingFile();
            emit this->closeFile();
        });
        menuBar()->addSeparator();
    {
            //needed to avoid arrow to overlap with menu
            //look for better solution
            QMenu *space=menuBar()->addMenu(tr("   "));
     }

    QMenu *menu = menuBar()->addMenu(tr("&File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
    a = menu->addAction(newIcon,  tr("&New"), this, &TextEdit::fileNew);

    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png"));
    a = menu->addAction(openIcon, tr("&Open..."), this, &TextEdit::fileOpen);
    a->setShortcut(QKeySequence::Open);


    menu->addSeparator();

    menu->addSeparator();
*/
#ifndef QT_NO_PRINTER

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = tb->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    //tb->addAction(a);

    //menu->addSeparator();
#endif
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(spacer);

    const QIcon quitIcon = QIcon::fromTheme("quit", QIcon(rsrcPath + "/logout.png"));
    a = tb->addAction(quitIcon, tr("&Logout"), this, [=]() {
        requestLogout();
        emit this->logout();
    });
    a->setShortcut(Qt::CTRL + Qt::Key_Q);

//    const QIcon profileIcon = QIcon::fromTheme("profile",QIcon(rsrcPath + "/user.png"));
//    a = tb->addAction(profileIcon,tr("&Userpage"),this,[=](){emit this->closeFile();});

}

void TextEdit::setupEditActions() {
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menu->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();

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
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &TextEdit::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tb->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &TextEdit::textStyle);

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
    QPushButton *label = new QPushButton(user);
    label->setObjectName(user);
    label->setStyleSheet(
            "QPushButton{height:50px;border:1px;text-align:left;font-size:22px;font-weight: bold; color: " + color +
            "}\n"
            "QPushButton:hover{background-color:rgb(243,243,243);border:1px;}");

    QIcon icon;
    icon.addFile(rsrcPath + QString::fromUtf8("/eye.png"), QSize(), QIcon::Normal, QIcon::On);
    label->setIcon(icon);
    label->setIconSize(QSize(16, 16));
    label->setFlat(true);

    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
    connect(label, SIGNAL(clicked()), SLOT(highlightcharacter()));
}

void TextEdit::setupConnectedUsers() {
    QDockWidget *dock = new QDockWidget(tr("Connected Users"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connectedUsers = new QListWidget(dock);
    QLabel *label = new QLabel(client_->getUser());
    label->setStyleSheet("font-weight: bold; color: " + client_->getColor());
    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
    dock->setWidget(connectedUsers);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, dock);


}

bool TextEdit::load(const QString &f) {
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) {
        textEdit->setHtml(str);
    } else {
        str = QString::fromLocal8Bit(data);
        textEdit->setPlainText(str);
    }

    setCurrentFileName(f);
    return true;
}

bool TextEdit::maybeSave() {
    if (!textEdit->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret =
            QMessageBox::warning(this, QCoreApplication::applicationName(),
                                 tr("The document has been modified.\n"
                                    "Do you want to save your changes?"),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void TextEdit::setCurrentFileName(const QString &fileName) {
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void TextEdit::fileNew() {
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFileName(QString());
    }
}

void TextEdit::fileOpen() {
    QFileDialog fileDialog(this, tr("Open File..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(QStringList() << "text/html" << "text/plain");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().first();
    if (load(fn))
        statusBar()->showMessage(tr("Opened \"%1\"").arg(QDir::toNativeSeparators(fn)));
    else
        statusBar()->showMessage(tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(fn)));
}

bool TextEdit::fileSave() {
    if (fileName.isEmpty())
        return fileSaveAs();
    if (fileName.startsWith(QStringLiteral(":/")))
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(textEdit->document());
    if (success) {
        textEdit->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName)));
    } else {
        statusBar()->showMessage(tr("Could not write to file \"%1\"")
                                         .arg(QDir::toNativeSeparators(fileName)));
    }
    return success;
}

bool TextEdit::fileSaveAs() {
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("odt");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}

void TextEdit::filePrint() {

#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
#endif
}

void TextEdit::filePrintPreview() {

#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &TextEdit::printPreview);
    preview.exec();
#endif

}

void TextEdit::printPreview(QPrinter *printer) {

#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif

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
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline() {
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic() {
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f) {
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p) {
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void TextEdit::textStyle(int styleIndex) {
    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    switch (styleIndex) {
        case 1:
            style = QTextListFormat::ListDisc;
            break;
        case 2:
            style = QTextListFormat::ListCircle;
            break;
        case 3:
            style = QTextListFormat::ListSquare;
            break;
        case 4:
            style = QTextListFormat::ListDecimal;
            break;
        case 5:
            style = QTextListFormat::ListLowerAlpha;
            break;
        case 6:
            style = QTextListFormat::ListUpperAlpha;
            break;
        case 7:
            style = QTextListFormat::ListLowerRoman;
            break;
        case 8:
            style = QTextListFormat::ListUpperRoman;
            break;
        default:
            break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 9 ? styleIndex - 9 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

void TextEdit::textColor() {
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::textAlign(QAction *a) {
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format) {
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::updateRemotePosition(QString user, int pos) {
    _cursorsVector[user].setPosition(pos);
    drawGraphicCursor();
}

void TextEdit::cursorPositionChanged() {

    QTextCursor cursor = textEdit->textCursor();
    int pos = cursor.position();

    json j = json{
            {"operation", "update_cursorPosition"},
            {"username",  client_->getUser().toStdString()},
            {"pos",       pos}
    };
    //client_->sendAtServer(j);

    /*
    alignmentChanged(textEdit->alignment());
    QTextList *list = textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
            case QTextListFormat::ListDisc:
                comboStyle->setCurrentIndex(1);
                break;
            case QTextListFormat::ListCircle:
                comboStyle->setCurrentIndex(2);
                break;
            case QTextListFormat::ListSquare:
                comboStyle->setCurrentIndex(3);
                break;
            case QTextListFormat::ListDecimal:
                comboStyle->setCurrentIndex(4);
                break;
            case QTextListFormat::ListLowerAlpha:
                comboStyle->setCurrentIndex(5);
                break;
            case QTextListFormat::ListUpperAlpha:
                comboStyle->setCurrentIndex(6);
                break;
            case QTextListFormat::ListLowerRoman:
                comboStyle->setCurrentIndex(7);
                break;
            case QTextListFormat::ListUpperRoman:
                comboStyle->setCurrentIndex(8);
                break;
            default:
                comboStyle->setCurrentIndex(-1);
                break;
        }
    } else {
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 8 : 0);
    }*/
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

void TextEdit::showSymbol(int pos, QChar c) {
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
    textEdit->setFocus();
}

void TextEdit::clipboardDataChanged() {}

void TextEdit::about() {
    QMessageBox::about(this, tr("About"), tr("This example demonstrates Qt's "
                                             "rich text editing facilities in action, providing an example "
                                             "document for you to experiment with."));
}

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

void TextEdit::colorChanged(const QColor &c) {
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a) {
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

bool TextEdit::eventFilter(QObject *obj, QEvent *ev) {
    if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *key_ev = static_cast<QKeyEvent *>(ev);
        int key = key_ev->key();
        std::cout << "FILE CRDT: " << std::flush; //print crdt
        for (auto iterPositions = client_->symbols.begin(); iterPositions != client_->symbols.end(); ++iterPositions) {
            if (iterPositions->getCharacter() != 10 && iterPositions->getCharacter() != 13)
                std::wcout << "[" << (int) iterPositions->getCharacter() << "(" << iterPositions->getCharacter()
                           << ") - " << std::flush;
            else
                std::wcout << "[" << (int) iterPositions->getCharacter() << "(\\n) - " << std::flush;
            for (int i = 0; i < iterPositions->getPosizione().size(); i++)
                std::cout << std::to_string(iterPositions->getPosizione()[i]) << std::flush;
            std::cout << " - " << iterPositions->getUsername() << std::flush;
            std::cout << "]" << std::flush;
        }
        std::cout << std::endl;
        if (obj == textEdit) {
            if (!key_ev->text().isEmpty()) {

                QTextCursor cursor = textEdit->textCursor();
                int pos, startIndex, endIndex;

                //cancella caratteri se sono selezionati (tranne per ctrl+a e ctrl+c
                if (cursor.hasSelection() && key_ev->text().toStdString().c_str()[0] != 3 &&
                    key_ev->text().toStdString().c_str()[0] != 1) {
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

                    int k = 0;
                    int dim = endIndex - startIndex;
                    int of = dim / client_->maxBufferSymbol;
                    while ((k + 1) * client_->maxBufferSymbol <= dim) {
                        symbolsToErase.clear();
                        usernameToErase.clear();
                        charToErase.clear();
                        crdtToErase.clear();
                        for (int i = 0; i < client_->maxBufferSymbol; i++) {
                            symbolsToErase.push_back(client_->symbols.at(i + startIndex));
                            usernameToErase.push_back(client_->symbols.at(i + startIndex).getUsername());
                            charToErase.push_back(client_->symbols.at(i + startIndex).getCharacter());
                            crdtToErase.push_back(client_->symbols.at(i + startIndex).getPosizione());
                        }
                        client_->eraseSymbolCRDT(symbolsToErase);

                        json j = json{
                                {"operation",       "remove"},
                                {"usernameToErase", usernameToErase},
                                {"charToErase",     charToErase},
                                {"crdtToErase",     crdtToErase}
                        };
                        client_->sendAtServer(j);
                        k++;
                    }

                    if ((dim % client_->maxBufferSymbol) > 0) {
                        symbolsToErase.clear();
                        usernameToErase.clear();
                        charToErase.clear();
                        crdtToErase.clear();
                        for (int i = 0; i < (dim % client_->maxBufferSymbol); i++) {
                            symbolsToErase.push_back(client_->symbols.at(i + startIndex));
                            usernameToErase.push_back(client_->symbols.at(i + startIndex).getUsername());
                            charToErase.push_back(client_->symbols.at(i + startIndex).getCharacter());
                            crdtToErase.push_back(client_->symbols.at(i + startIndex).getPosizione());
                        }
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
                    std::vector<int> crdt = client_->insertSymbolNewCRDT(pos, c, client_->getUser().toStdString());
                    textEdit->setTextCursor(cursor);
                    //emit updateCursor();

                    std::vector<std::string> usernamev;
                    usernamev.push_back(client_->getUser().toStdString());
                    std::vector<wchar_t> charv;
                    charv.push_back(c);
                    std::vector<std::vector<int>> crdtv;
                    crdtv.push_back(crdt);
                    json j = json{
                            {"operation",        "insert"},
                            {"usernameToInsert", usernamev},
                            {"charToInsert",     charv},
                            {"crdtToInsert",     crdtv}
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

                    std::vector<std::string> usernameToInsert;
                    std::vector<wchar_t> charToInsert;
                    std::vector<std::vector<int>> crdtToInsert;
                    std::vector<int> crdt;
                    wchar_t c;
                    int startPos = pos;
                    int dim = pastedText.size();
                    int of = dim / client_->maxBufferSymbol;
                    int i = 0;

                    while ((i + 1) * client_->maxBufferSymbol <= dim) {
                        usernameToInsert.clear();
                        charToInsert.clear();
                        crdtToInsert.clear();

                        for (int k = 0; k < client_->maxBufferSymbol; k++) {
                            textEdit->setTextCursor(cursor);
                            usernameToInsert.push_back(client_->getUser().toStdString());
                            c = pastedText.toStdWString().c_str()[(i * client_->maxBufferSymbol) + k];
                            charToInsert.push_back(c);
                            crdt = client_->insertSymbolNewCRDT(pos, c, client_->getUser().toStdString());
                            crdtToInsert.push_back(crdt);
                            pos++;
                        }
                        json j = json{
                                {"operation",        "insert"},
                                {"usernameToInsert", usernameToInsert},
                                {"charToInsert",     charToInsert},
                                {"crdtToInsert",     crdtToInsert}
                        };
                        client_->sendAtServer(j);
                        i++;
                    }


                    if ((dim % client_->maxBufferSymbol) > 0) {
                        usernameToInsert.clear();
                        charToInsert.clear();
                        crdtToInsert.clear();

                        for (int i = 0; i < (dim % client_->maxBufferSymbol); i++) {
                            textEdit->setTextCursor(cursor);
                            usernameToInsert.push_back(client_->getUser().toStdString());
                            c = pastedText.toStdWString().c_str()[(of * client_->maxBufferSymbol) + i];
                            charToInsert.push_back(c);
                            crdt = client_->insertSymbolNewCRDT(pos, c, client_->getUser().toStdString());
                            crdtToInsert.push_back(crdt);
                            pos++;
                        }
                        json j = json{
                                {"operation",        "insert"},
                                {"usernameToInsert", usernameToInsert},
                                {"charToInsert",     charToInsert},
                                {"crdtToInsert",     crdtToInsert}
                        };
                        client_->sendAtServer(j);
                    }
                    //TO DO: probabilente aggiornare cursori anche qui
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
    QObject *sender = QObject::sender();
    QString name = sender->objectName();
    QTextCursor cursor = textEdit->textCursor();
    QTextCursor tempCursor = QTextCursor(cursor);
    int pos = 0;
    for (auto s: client_->symbols) {
        if (s.getUsername() == name.toStdString()) {
            tempCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
            tempCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, pos);
            tempCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
            textEdit->setTextCursor(tempCursor);
            if (name == client_->getUser())
                textEdit->setTextBackgroundColor(client_->getColor());
            else
                textEdit->setTextBackgroundColor(_listParticipantsAndColors[name]);

        }
        pos++;
    }

    textEdit->setTextCursor(cursor);
    textEdit->setTextBackgroundColor(QColor(255, 255, 255, 255));
    textEdit->setFocus();

    timer->start(3500);

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
    //QLabel * label = new QLabel(client_->getUser());
    QPushButton *label = new QPushButton(client_->getUser());
    label->setObjectName(client_->getUser());
    label->setStyleSheet(
            "QPushButton{border:1px;text-align:left;font-size:22px;font-weight: bold; color: " + client_->getColor() +
            "}\n"
            "QPushButton:hover{border:1px;background-color:rgb(243,243,243)}");

    QIcon icon;
    icon.addFile(rsrcPath + QString::fromUtf8("/eye.png"), QSize(), QIcon::Normal, QIcon::On);
    label->setIcon(icon);
    label->setIconSize(QSize(16, 16));
    label->setFlat(true);

    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
    connect(label, SIGNAL(clicked()), SLOT(highlightcharacter()));
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

/*
void TextEdit::updateCursors() {
    CustomCursor cursor = CustomCursor(QTextCursor(textEdit->textCursor()));
    updateCursors(cursor);
}
*/
void TextEdit::updateCursors(const CustomCursor &cursor) {
    _oldCursor = CustomCursor(_newCursor);
    _newCursor = CustomCursor(cursor);
}
