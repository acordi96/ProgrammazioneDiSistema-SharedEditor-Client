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

#include "textedit.h"

#ifdef Q_OS_MACOS
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

TextEdit::TextEdit(Client* c, QWidget *parent)
        : QMainWindow(parent), client_(c)
{
#ifdef Q_OS_MACOS
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    textEdit = new QTextEdit(this);

    connect(this,&TextEdit::updateCursor,this,&TextEdit::drawRemoteCursors);

    connect(client_, &Client::insertSymbol, this, &TextEdit::showSymbol);
    connect(client_, &Client::insertSymbolWithId, this, &TextEdit::showSymbolWithId);
    connect(client_, &Client::updateCursorParticipant, this, &TextEdit::initRemoteCursors);
    connect(client_, &Client::insertParticipant, this, &TextEdit::initListParticipant);
    connect(client_,&Client::eraseSymbols,this, &TextEdit::eraseSymbols);
    connect(client_,&Client::clearEditor,textEdit,&QTextEdit::clear);

    textEdit->installEventFilter(this);

    setCentralWidget(textEdit);

    setupFileActions();
    setupConnectedUsers();

    {
        QMenu *userMenu = menuBar()->addMenu(client_->getUser());
        userMenu->addAction(tr("Logout"),this,[=](){
            requestLogout();
            emit this->logout();
        });
    }
    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);

    connect(textEdit->document(), &QTextDocument::modificationChanged,this, &QWidget::setWindowModified);


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

void TextEdit::closeEvent(QCloseEvent *e)
{
    if (maybeSave())
        e->accept();
    else
        e->ignore();
}

void TextEdit::requestLogout(){
    json j = json{
             {"operation","req_logout"},
             {"username",client_->getUser().toStdString()},
    };
    std::string mess = j.dump().c_str();
    message msg;
    msg.body_length(mess.size());
    std::memcpy(msg.body(),mess.data(),msg.body_length());
    msg.body()[msg.body_length()]='\0';
    msg.encode_header();
    std::cout<<"Messaggio da inviare al server "<< msg.body() << std::endl;
    client_->write(msg);
}

void TextEdit::closingFile(){
    json j =json{
                {"operation","close_file"},
                {"filename",client_->getFileName().toStdString()},
                {"username",client_->getUser().toStdString()}
            };
    std::string mess = j.dump().c_str();
    message msg;
    msg.body_length(mess.size());
    std::memcpy(msg.body(),mess.data(),msg.body_length());
    msg.body()[msg.body_length()]='\0';
    msg.encode_header();
    std::cout<<"Messaggio da inviare al server "<< msg.body() << std::endl;
    client_->write(msg);
}

void TextEdit::setupFileActions()
{



        const QIcon backIcon = QIcon::fromTheme("go-back",QIcon(rsrcPath+"/left-arrow.png"));

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
    QAction *a = menu->addAction(newIcon,  tr("&New"), this, &TextEdit::fileNew);

    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png"));
    a = menu->addAction(openIcon, tr("&Open..."), this, &TextEdit::fileOpen);
    a->setShortcut(QKeySequence::Open);


    menu->addSeparator();

    menu->addSeparator();

#ifndef QT_NO_PRINTER

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    //tb->addAction(a);

    menu->addSeparator();
#endif
    const QIcon quitIcon = QIcon::fromTheme("quit",QIcon(rsrcPath + "/cancel-icon.png"));
    a = menu->addAction(tr("&Quit"), this,[=](){
        requestLogout();
        emit this->closeAll();
    });
    a->setShortcut(Qt::CTRL + Qt::Key_Q);


    const QIcon profileIcon = QIcon::fromTheme("profile",QIcon(rsrcPath + "/user.png"));
    a = menu->addAction(profileIcon,tr("&Userpage"),this,[=](){emit this->closeFile();});

}

void TextEdit::setupEditActions()
{
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

void TextEdit::setupTextActions()
{
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


void TextEdit::updateConnectedUsers(QString user, QString color){
    QLabel * label = new QLabel(user);
    label->setStyleSheet("color:"+color);
    QListWidgetItem * item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
}
void TextEdit::setupConnectedUsers(){
    QDockWidget *dock = new QDockWidget(tr("Connected Users"),this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    connectedUsers = new QListWidget(dock);
    QLabel * label = new QLabel(client_->getUser());
    label->setStyleSheet("font-weight: bold; color: "+client_->getColor());
    QListWidgetItem * item = new QListWidgetItem();
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item, label);
    /*for(auto u : users){
     *  label = new QLabel(username);
     *  label->setStyleSheet(QString::fromUtf8("color:#"));
     *  QListWidgetItem *item = new QListWidgetItem();
     *  connectedUsers->addItem(item);
     *  connectedUsers->setItemWidget(item,label);
     *  }
     * */

/*

    QLabel *label = new QLabel(client_->getUser());
    std::cout<< client_->getColor().toStdString() <<std::endl;
    label->setStyleSheet(QString::fromUtf8("color:#463745"));
    QListWidgetItem *item = new QListWidgetItem();
    connectedUsers->addItems(QStringList()
                            << "angelo96"
                            << "nick1");
    connectedUsers->addItem(item);
    connectedUsers->setItemWidget(item,label);
    */
    dock->setWidget(connectedUsers);
    addDockWidget(Qt::RightDockWidgetArea,dock);
}


bool TextEdit::load(const QString &f)
{
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

bool TextEdit::maybeSave()
{
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

void TextEdit::setCurrentFileName(const QString &fileName)
{
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

void TextEdit::fileNew()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFileName(QString());
    }
}

void TextEdit::fileOpen()
{
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

bool TextEdit::fileSave()
{
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

bool TextEdit::fileSaveAs()
{
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

void TextEdit::filePrint()
{

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


void TextEdit::filePrintPreview()
{

#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &TextEdit::printPreview);
    preview.exec();
#endif

}

void TextEdit::printPreview(QPrinter *printer)
{

#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif

}


void TextEdit::filePrintPdf()
{

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

void TextEdit::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void TextEdit::textStyle(int styleIndex)
{
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

void TextEdit::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TextEdit::cursorPositionChanged()
{
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
    }
}

void TextEdit::showSymbolWithId(int id, int pos, QChar c) {
    QTextCharFormat format;
    format.setFontWeight(QFont::Normal);
    format.setFontFamily("Helvetica");

    QTextCursor cur = textEdit->textCursor();

    cur.beginEditBlock();

    int endIndex;
    cur.hasSelection() ? endIndex = cur.selectionEnd() : endIndex = -90;
    int oldPos = pos < cur.position() ? cur.position()+1 : cur.position();

    if(cur.hasSelection() && pos==endIndex){
        qDebug()<<cur.selectedText();

        int startIndex = cur.selectionStart();

        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));

        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::MoveAnchor);
        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::KeepAnchor);
    }
    else{
        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));
        cur.setPosition(oldPos);
    }
    cur.endEditBlock();

    //textEdit->setText(textEdit->toPlainText().insert(pos,c));

    textEdit->setTextCursor(cur);
    _cursorsVector[id].setPosition(pos+1);
    // textEdit->setTextBackgroundColor(Qt::red);
    drawRemoteCursors();
    //qDebug()<< "Written in pos: "<< pos << endl;
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
    int oldPos = pos < cur.position() ? cur.position()+1 : cur.position();

    if(cur.hasSelection() && pos==endIndex){
        qDebug()<<cur.selectedText();

        int startIndex = cur.selectionStart();

        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));

        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::MoveAnchor);
        cur.setPosition(oldPos == startIndex ? endIndex : startIndex, QTextCursor::KeepAnchor);
    }
    else{
        cur.setPosition(pos);
        cur.setCharFormat(format);
        cur.insertText(static_cast<QString>(c));
        cur.setPosition(oldPos);
    }
    cur.endEditBlock();

    //textEdit->setText(textEdit->toPlainText().insert(pos,c));

    textEdit->setTextCursor(cur);
    //qDebug()<< "Written in pos: "<< pos << endl;
    textEdit->setFocus();
}

void TextEdit::clipboardDataChanged()
{
/*#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif*/
}

void TextEdit::about()
{
    QMessageBox::about(this, tr("About"), tr("This example demonstrates Qt's "
                                             "rich text editing facilities in action, providing an example "
                                             "document for you to experiment with."));
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

void TextEdit::localInsert(){
    QTextCursor cur = textEdit->textCursor();
    int oldPos = cur.position();
    std::pair<int,char> m;
    char c;

    if(cur.hasSelection()){
        //remove char
        return;
    }
    else{
        //insert char
        cur.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor,1);
        c = cur.selectedText().toStdString().c_str()[0];
        qDebug()<< c;
    }

    m = std::make_pair(cur.position(),c);
    json j = json{
        {"operation","insert"},
        {"corpo",m}
    };

    std::string msg = j.dump().c_str();
    size_t size_msg = msg.size();
    message mess;
    mess.body_length(msg.size());
    std::memcpy(mess.body(), msg.data(), mess.body_length());
    mess.body()[mess.body_length()] = '\0';
    mess.encode_header();
    std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
    client_->write(mess);
    textEdit->textCursor().setPosition(oldPos);
}

bool TextEdit::eventFilter(QObject *obj, QEvent *ev){
    if(ev->type() == QEvent::KeyPress){
        QKeyEvent *key_ev = static_cast<QKeyEvent *>(ev);
        qDebug()<< " You pressed "+ key_ev->text();
        int key = key_ev->key();
        if (obj == textEdit){
            if(!key_ev->text().isEmpty()){
                if( !(key == Qt::Key_Backspace) &&
                    !(key==Qt::Key_Delete) &&
                    !(key==Qt::Key_Escape) &&
                    !(key_ev->text().toStdString().c_str()[0]==22) && //paste
                    !(key_ev->text().toStdString().c_str()[0]==3) && //copy
                    !(key_ev->text().toStdString().c_str()[0]==1)){ //ctrl+a
                    // get data
                    std::pair<int, char> tuple;
                    QTextCursor cursor = textEdit->textCursor();
                    int pos;

                    if(cursor.hasSelection()){
                        pos=cursor.selectionStart();
                        int startIndex = cursor.selectionStart();
                        int endIndex = cursor.selectionEnd();

                        cursor.beginEditBlock();
                        cursor.setPosition(startIndex,QTextCursor::MoveAnchor);
                        cursor.setPosition(endIndex,QTextCursor::KeepAnchor);
                        textEdit->setTextCursor(cursor);
                        cursor.endEditBlock();

                        json j = json{
                                    {"operation","remove"},
                                    {"start", startIndex},
                                    {"end",endIndex}
                                };
                        std::string msg = j.dump().c_str();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(), msg.data(), mess.body_length());
                        mess.body()[mess.body_length()] = '\0';
                        mess.encode_header();
                        std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                        client_->write(mess);
                    }
                    else{
                        pos = cursor.position();
                    }
                    char c = key_ev->text().toStdString().c_str()[0];
                     /* update char format
                     * QCharFormat form;
                     * form.set{quello_che c'è da settare}(valorechedeveaver);
                     * --
                     * --
                     * --
                     * cursor.setCharFormat(form);
                     * textEdit->setTextCursor(cursor);
                     */
                    textEdit->setTextCursor(cursor);
                    emit updateCursor();
                    tuple = std::make_pair(pos,c);

                    json j = json{
                                {"operation","insert"},
                                {"corpo",tuple}
                            };
                    std::string msg = j.dump().c_str();
                    size_t size_msg = msg.size();
                    message mess;
                    mess.body_length(msg.size());
                    std::memcpy(mess.body(), msg.data(), mess.body_length());
                    mess.body()[mess.body_length()] = '\0';
                    mess.encode_header();
                    std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                    client_->write(mess);
                    return QObject::eventFilter(obj,ev);
                }
                //*********************PASTE*****************************************
                else if(key_ev->text().toStdString().c_str()[0]==22){
                    QClipboard *clipboard = QGuiApplication::clipboard();
                    QString pastedText = clipboard->text();
                    std::cout<<"Incollato "<<pastedText.length()<<" caratteri"<<std::endl;

                    std::pair<int, char> tuple;
                    QTextCursor cursor = textEdit->textCursor();
                    int pos;

                    if(cursor.hasSelection()){
                        pos=cursor.selectionStart();
                        int startIndex = cursor.selectionStart();
                        int endIndex = cursor.selectionEnd();

                        cursor.beginEditBlock();
                        cursor.setPosition(startIndex,QTextCursor::MoveAnchor);
                        cursor.setPosition(endIndex,QTextCursor::KeepAnchor);
                        textEdit->setTextCursor(cursor);
                        cursor.endEditBlock();

                        json j = json{
                                {"operation","remove"},
                                {"start", startIndex},
                                {"end",endIndex}
                        };
                        std::string msg = j.dump().c_str();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(), msg.data(), mess.body_length());
                        mess.body()[mess.body_length()] = '\0';
                        mess.encode_header();
                        std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                        client_->write(mess);
                    }
                    else{
                        pos = cursor.position();
                    }
                    for(int i=0; i<pastedText.length(); i++) {
                        textEdit->setTextCursor(cursor);
                        char c = pastedText.toStdString().c_str()[i];
                        tuple = std::make_pair(pos,c);
                        pos++;
                        json j = json{
                                {"operation","insert"},
                                {"corpo",tuple}
                        };
                        std::string msg = j.dump().c_str();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(), msg.data(), mess.body_length());
                        mess.body()[mess.body_length()] = '\0';
                        mess.encode_header();
                        std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                        client_->write(mess);
                    }
                }
                //*********************COPY*****************************************
                else if(key_ev->text().toStdString().c_str()[0]==3){
                    //do nothing
                    std::cout<<"Copiato"<<std::endl;
                }
                //*********************CTRL+A*****************************************
                else if(key_ev->text().toStdString().c_str()[0]==1){
                    //do nothing
                    std::cout<<"Selezionato tutto"<<std::endl;
                }
                //*********************BACKSPACE*************************************
                else if(key == Qt::Key_Backspace){
                    QTextCursor cursor = textEdit->textCursor();
                    int pos = cursor.position();

                    if(cursor.hasSelection()){
                        int startIndex = cursor.selectionStart();
                        int endIndex = cursor.selectionEnd();

                        cursor.setPosition(startIndex);
                        QTextBlockFormat textBlockFormat;
                        int firstCharAlignment = static_cast<int>(cursor.blockFormat().alignment());
                        textBlockFormat.setAlignment(static_cast<Qt::AlignmentFlag>(firstCharAlignment));
                        cursor.mergeBlockFormat(textBlockFormat);
                        textEdit->setAlignment(textBlockFormat.alignment());
                        cursor.setPosition(pos);

                        json j = json{
                                {"operation","remove"},
                                {"start",startIndex},
                                {"end",endIndex}
                        };

                        std::string msg = j.dump().c_str();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(),msg.data(),mess.body_length());
                        mess.encode_header();
                        std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                        client_->write(mess);
                    }
                    else if(pos > 0 ){
                        json j = json{
                                {"operation","remove"},
                                {"start",pos-1},
                                {"end",pos}
                        };

                        std::string msg = j.dump().c_str();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(),msg.data(),mess.body_length());
                        mess.encode_header();
                        std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                        client_->write(mess);
                    }
                    return QObject::eventFilter(obj,ev);
                }
                //**********************CANC****************************/
                else if(key==Qt::Key_Delete){
                    QTextCursor cursor = textEdit->textCursor();
                    int pos = cursor.position();

                    if(cursor.hasSelection()){
                        int startIndex = cursor.selectionStart();
                        int endIndex = cursor.selectionEnd();

                        cursor.setPosition(cursor.selectionStart());
                        QTextBlockFormat textBlockFormat;
                        int firstCharAlignment = static_cast<int>(cursor.blockFormat().alignment());
                        textBlockFormat.setAlignment(static_cast<Qt::AlignmentFlag>(firstCharAlignment));
                        cursor.mergeBlockFormat(textBlockFormat);
                        textEdit->setAlignment(textBlockFormat.alignment());
                        cursor.setPosition(pos);

                        json j = json{
                            {"operation","remove"},
                            {"start",startIndex},
                            {"end",endIndex}
                        };
                        std::string msg = j.dump().c_str();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(),msg.data(),mess.body_length());
                        mess.encode_header();
                        std::cout <<"Messaggio da inviare al server: "<< mess.body() << std::endl;
                        client_->write(mess);
                    }
                    else if(pos>=0 && pos<textEdit->toPlainText().size()){
                        json j = json{
                            {"operation","remove"},
                            {"start",pos},
                            {"end",pos+1}
                        };
                        std::string msg = j.dump();
                        size_t size_msg = msg.size();
                        message mess;
                        mess.body_length(msg.size());
                        std::memcpy(mess.body(),msg.data(),mess.body_length());
                        mess.encode_header();
                        std::cout<<"Messaggio da inviare al server: "<< mess.body()<<std::endl;
                        client_->write(mess);
                    }
                    return QObject::eventFilter(obj,ev);
                }
            }
        }
    }
    return false;
}

void TextEdit::eraseSymbols(int start, int end){
    QTextCursor cur = textEdit->textCursor();

    cur.beginEditBlock();

    cur.setPosition(start);
    int startAlignment = cur.blockFormat().alignment();

    /* erase symbols */
    cur.setPosition(end);
    cur.setPosition(start+1,QTextCursor::KeepAnchor);
    cur.removeSelectedText();
    cur.setPosition(start,QTextCursor::KeepAnchor);
    cur.removeSelectedText();

    QTextBlockFormat textBlockFormat;
    textBlockFormat = cur.blockFormat();
    textBlockFormat.setAlignment(static_cast<Qt::AlignmentFlag>(startAlignment));
    cur.mergeBlockFormat(textBlockFormat);

    cur.endEditBlock();

    qDebug() << "deleted char ranges " << endl;
    textEdit->setFocus();
}
void TextEdit::draw2 (unsigned int position){
    QTextCursor cursor= QTextCursor(textEdit->textCursor());
    cursor.setPosition(position);
    std::cout<<" Preso cursor"<<cursor.position()<<std::endl;
    QTextCursor tempCursor = QTextCursor(cursor);
    tempCursor.setPosition(position);
    std::cout<<" Preso tempCursor"<<tempCursor.position()<<std::endl;
    tempCursor.clearSelection();
    std::cout<<" Preso tempCursor clear"<<tempCursor.position()<<std::endl;

    tempCursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor,1);
    std::cout<<" Preso tempCursor move1"<<tempCursor.position()<< tempCursor.selectionStart()<<std::endl;

    tempCursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor,1);
    std::cout<<" Preso tempCursor move2"<<tempCursor.position()<<tempCursor.selectionEnd()<<std::endl;

    textEdit->setTextCursor(tempCursor);
    textEdit->setTextBackgroundColor(QColor(255,255,255,255));
    textEdit->setTextCursor(cursor);
    //textEdit->setTextBackgroundColor(client_->getColor());
    textEdit->setTextBackgroundColor(Qt::red);

}

QString TextEdit::getFileName() const
{
    return fileName;
}
void TextEdit::drawRemoteCursors(){
    QTextCursor cursor= QTextCursor(textEdit->textCursor());
    QTextCursor tempCursor = QTextCursor(cursor);
    tempCursor.clearSelection();
    tempCursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor,1);
    tempCursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor,1);
    textEdit->setTextCursor(tempCursor);
    textEdit->setTextBackgroundColor(QColor(255,255,255,255));
    textEdit->setTextCursor(cursor);
//    textEdit->setTextBackgroundColor(client_->getColor());
    textEdit->setTextBackgroundColor(QColor(255,255,255,255));
    for(std::pair<unsigned int, CustomCursor> cPair : _cursorsVector){
        tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
        CustomCursor cCursor = cPair.second;
        if(cCursor.hasSelection){
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, (int)cCursor.selectionStart);
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, (int)cCursor.selectionEnd-(int)cCursor.selectionStart);
        }else{
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, (int)cCursor.position);
            if(cCursor.position==0){
                tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, 1);
            }else{
                tempCursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor, 1);
            }
        }
        textEdit->setTextCursor(tempCursor);
        textEdit->setTextBackgroundColor(_cursorColors[cPair.first]);
        //_ui->textEdit->setTextCursor(myCursor);
        tempCursor.clearSelection();
        tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    }
    textEdit->setTextCursor(cursor);
    textEdit->setTextBackgroundColor(QColor{255,255,255, 255});

}


void TextEdit::initListParticipant(int participantId, QString username) {
    if(_listParticipant.insert(std::pair<int, QString>(participantId, username)).second){
        std::cout << "Inserisco user con id " << participantId << " ,user " << username.toStdString()<< std::endl;
    }
}


void TextEdit::initRemoteCursors(int participantId, QString color) {
    std::cout << "Id " << participantId << " == " << client_->getUser().toStdString() << std::endl;
    if(_listParticipant[participantId] != client_->getUser()) {
        CustomCursor remoteCursor = CustomCursor();
        _cursorsVector.insert(std::pair<int, CustomCursor>(participantId, remoteCursor));
        if (_cursorColors.insert(std::pair<int, QColor>(participantId, color)).second) {
            updateConnectedUsers(_listParticipant[participantId], color);
        }
    }

}

void TextEdit::resetText(){
      _currentText = QString(textEdit->toPlainText());
}

void TextEdit::resetCursors(){

    _oldCursor = CustomCursor(QTextCursor(textEdit->textCursor()));
    _newCursor = CustomCursor(QTextCursor(textEdit->textCursor()));
}

void TextEdit::updateCursors(){
    CustomCursor cursor = CustomCursor(QTextCursor(textEdit->textCursor()));
    updateCursors(cursor);
}
void TextEdit::updateCursors(const CustomCursor &cursor){
    _oldCursor = CustomCursor(_newCursor);
    _newCursor = CustomCursor(cursor);
}
