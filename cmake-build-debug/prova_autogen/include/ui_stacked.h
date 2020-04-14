/********************************************************************************
** Form generated from reading UI file 'stacked.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STACKED_H
#define UI_STACKED_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_stacked
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *loginpage;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_4;
    QFormLayout *formLayout_2;
    QLabel *label;
    QLineEdit *user_log_line;
    QLabel *label_2;
    QLineEdit *psw_log_line;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *reglogButton;
    QPushButton *loginButton;
    QWidget *registration;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_6;
    QWidget *widget;
    QLabel *icon_label;
    QGraphicsView *icon;
    QFormLayout *formLayout_5;
    QLabel *user_label;
    QLineEdit *user_line_form;
    QLabel *psw_lab_form;
    QLineEdit *psw_line_form;
    QLabel *confirm_psw_lab;
    QLineEdit *confirm_psw_line;
    QLabel *email_label;
    QLineEdit *email_line;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *form_cancButton;
    QPushButton *form_regButton;
    QWidget *editor;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_2;
    QWidget *widget_3;
    QLabel *label_4;
    QLabel *label_5;
    QPushButton *pushButton_6;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout;
    QPushButton *fileButton;
    QPushButton *pushButton_3;
    QPushButton *pushButton_5;
    QLabel *label_3;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *stacked)
    {
        if (stacked->objectName().isEmpty())
            stacked->setObjectName(QString::fromUtf8("stacked"));
        stacked->resize(701, 570);
        centralwidget = new QWidget(stacked);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, 0, 701, 531));
        stackedWidget->setAcceptDrops(false);
        loginpage = new QWidget();
        loginpage->setObjectName(QString::fromUtf8("loginpage"));
        verticalLayoutWidget = new QWidget(loginpage);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(190, 90, 321, 381));
        verticalLayout_4 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFormAlignment(Qt::AlignCenter);
        formLayout_2->setContentsMargins(50, -1, 30, -1);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        user_log_line = new QLineEdit(verticalLayoutWidget);
        user_log_line->setObjectName(QString::fromUtf8("user_log_line"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, user_log_line);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        psw_log_line = new QLineEdit(verticalLayoutWidget);
        psw_log_line->setObjectName(QString::fromUtf8("psw_log_line"));
        psw_log_line->setEchoMode(QLineEdit::Password);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, psw_log_line);


        verticalLayout_4->addLayout(formLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(50, -1, 50, -1);
        reglogButton = new QPushButton(verticalLayoutWidget);
        reglogButton->setObjectName(QString::fromUtf8("reglogButton"));

        horizontalLayout_3->addWidget(reglogButton);

        loginButton = new QPushButton(verticalLayoutWidget);
        loginButton->setObjectName(QString::fromUtf8("loginButton"));

        horizontalLayout_3->addWidget(loginButton, 0, Qt::AlignVCenter);


        verticalLayout_4->addLayout(horizontalLayout_3);

        stackedWidget->addWidget(loginpage);
        registration = new QWidget();
        registration->setObjectName(QString::fromUtf8("registration"));
        verticalLayoutWidget_2 = new QWidget(registration);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(140, 20, 381, 501));
        verticalLayout_6 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(verticalLayoutWidget_2);
        widget->setObjectName(QString::fromUtf8("widget"));
        icon_label = new QLabel(widget);
        icon_label->setObjectName(QString::fromUtf8("icon_label"));
        icon_label->setGeometry(QRect(60, 60, 67, 17));
        icon = new QGraphicsView(widget);
        icon->setObjectName(QString::fromUtf8("icon"));
        icon->setGeometry(QRect(190, 20, 101, 91));

        verticalLayout_6->addWidget(widget);

        formLayout_5 = new QFormLayout();
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        formLayout_5->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        formLayout_5->setFormAlignment(Qt::AlignCenter);
        user_label = new QLabel(verticalLayoutWidget_2);
        user_label->setObjectName(QString::fromUtf8("user_label"));

        formLayout_5->setWidget(1, QFormLayout::LabelRole, user_label);

        user_line_form = new QLineEdit(verticalLayoutWidget_2);
        user_line_form->setObjectName(QString::fromUtf8("user_line_form"));

        formLayout_5->setWidget(1, QFormLayout::FieldRole, user_line_form);

        psw_lab_form = new QLabel(verticalLayoutWidget_2);
        psw_lab_form->setObjectName(QString::fromUtf8("psw_lab_form"));

        formLayout_5->setWidget(2, QFormLayout::LabelRole, psw_lab_form);

        psw_line_form = new QLineEdit(verticalLayoutWidget_2);
        psw_line_form->setObjectName(QString::fromUtf8("psw_line_form"));
        psw_line_form->setEchoMode(QLineEdit::Password);

        formLayout_5->setWidget(2, QFormLayout::FieldRole, psw_line_form);

        confirm_psw_lab = new QLabel(verticalLayoutWidget_2);
        confirm_psw_lab->setObjectName(QString::fromUtf8("confirm_psw_lab"));

        formLayout_5->setWidget(3, QFormLayout::LabelRole, confirm_psw_lab);

        confirm_psw_line = new QLineEdit(verticalLayoutWidget_2);
        confirm_psw_line->setObjectName(QString::fromUtf8("confirm_psw_line"));
        confirm_psw_line->setEchoMode(QLineEdit::Password);

        formLayout_5->setWidget(3, QFormLayout::FieldRole, confirm_psw_line);

        email_label = new QLabel(verticalLayoutWidget_2);
        email_label->setObjectName(QString::fromUtf8("email_label"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, email_label);

        email_line = new QLineEdit(verticalLayoutWidget_2);
        email_line->setObjectName(QString::fromUtf8("email_line"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, email_line);


        verticalLayout_6->addLayout(formLayout_5);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(20);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(50, -1, 50, -1);
        form_cancButton = new QPushButton(verticalLayoutWidget_2);
        form_cancButton->setObjectName(QString::fromUtf8("form_cancButton"));

        horizontalLayout_7->addWidget(form_cancButton);

        form_regButton = new QPushButton(verticalLayoutWidget_2);
        form_regButton->setObjectName(QString::fromUtf8("form_regButton"));

        horizontalLayout_7->addWidget(form_regButton);


        verticalLayout_6->addLayout(horizontalLayout_7);

        stackedWidget->addWidget(registration);
        editor = new QWidget();
        editor->setObjectName(QString::fromUtf8("editor"));
        horizontalLayoutWidget = new QWidget(editor);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(180, 110, 341, 321));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widget_2 = new QWidget(horizontalLayoutWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_3 = new QWidget(widget_2);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setGeometry(QRect(130, 0, 211, 319));
        widget_3->setMouseTracking(false);
        widget_3->setStyleSheet(QString::fromUtf8("background-color: rgb(252, 233, 79);"));
        label_4 = new QLabel(widget_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(40, 100, 111, 31));
        label_4->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(widget_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(60, 50, 67, 17));
        label_5->setAlignment(Qt::AlignCenter);
        pushButton_6 = new QPushButton(widget_3);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(40, 210, 111, 25));
        pushButton_6->setMouseTracking(true);
        pushButton_6->setFocusPolicy(Qt::StrongFocus);
        pushButton_6->setStyleSheet(QString::fromUtf8("QPushButton:hover{\n"
"	color: rgb(32, 74, 135);\n"
"}\n"
""));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/Science-Plus2-Math-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_6->setIcon(icon1);
        pushButton_6->setCheckable(false);
        pushButton_6->setChecked(false);
        pushButton_6->setAutoDefault(false);
        pushButton_6->setFlat(true);
        scrollArea = new QScrollArea(widget_2);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(0, 0, 131, 321));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 129, 319));
        verticalLayoutWidget_3 = new QWidget(scrollAreaWidgetContents);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(0, 30, 131, 291));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        fileButton = new QPushButton(verticalLayoutWidget_3);
        fileButton->setObjectName(QString::fromUtf8("fileButton"));
        fileButton->setAutoFillBackground(false);
        fileButton->setStyleSheet(QString::fromUtf8("QPushButton { \n"
"border: 0px;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(32, 74, 135);\n"
"}"));
        fileButton->setCheckable(false);
        fileButton->setChecked(false);
        fileButton->setAutoDefault(false);
        fileButton->setFlat(false);

        verticalLayout->addWidget(fileButton);

        pushButton_3 = new QPushButton(verticalLayoutWidget_3);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"border: 0px;\n"
"}\n"
"QPushButton:hover {\n"
"background-color: rgb(32, 74, 135);\n"
"}"));
        pushButton_3->setFlat(false);

        verticalLayout->addWidget(pushButton_3);

        pushButton_5 = new QPushButton(verticalLayoutWidget_3);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"border: 0px;\n"
"}\n"
"QPushButton:hover {\n"
"background-color: rgb(32, 74, 135);\n"
"}"));
        pushButton_5->setFlat(false);

        verticalLayout->addWidget(pushButton_5);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 10, 101, 17));
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(widget_2);

        stackedWidget->addWidget(editor);
        stacked->setCentralWidget(centralwidget);
        menubar = new QMenuBar(stacked);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 701, 22));
        menubar->setNativeMenuBar(true);
        stacked->setMenuBar(menubar);
        statusbar = new QStatusBar(stacked);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        stacked->setStatusBar(statusbar);
        QWidget::setTabOrder(icon, email_line);
        QWidget::setTabOrder(email_line, user_line_form);
        QWidget::setTabOrder(user_line_form, psw_line_form);
        QWidget::setTabOrder(psw_line_form, confirm_psw_line);
        QWidget::setTabOrder(confirm_psw_line, form_cancButton);
        QWidget::setTabOrder(form_cancButton, form_regButton);
        QWidget::setTabOrder(form_regButton, scrollArea);
        QWidget::setTabOrder(scrollArea, loginButton);
        QWidget::setTabOrder(loginButton, reglogButton);
        QWidget::setTabOrder(reglogButton, fileButton);
        QWidget::setTabOrder(fileButton, user_log_line);
        QWidget::setTabOrder(user_log_line, pushButton_3);
        QWidget::setTabOrder(pushButton_3, pushButton_5);
        QWidget::setTabOrder(pushButton_5, psw_log_line);
        QWidget::setTabOrder(psw_log_line, pushButton_6);

        retranslateUi(stacked);

        stackedWidget->setCurrentIndex(0);
        pushButton_6->setDefault(false);
        fileButton->setDefault(false);


        QMetaObject::connectSlotsByName(stacked);
    } // setupUi

    void retranslateUi(QMainWindow *stacked)
    {
        stacked->setWindowTitle(QApplication::translate("stacked", "MainWindow", nullptr));
        label->setText(QApplication::translate("stacked", "Username : ", nullptr));
        label_2->setText(QApplication::translate("stacked", "Password :", nullptr));
        psw_log_line->setText(QString());
        reglogButton->setText(QApplication::translate("stacked", "Register", nullptr));
        loginButton->setText(QApplication::translate("stacked", "Login", nullptr));
        icon_label->setText(QApplication::translate("stacked", "Icon", nullptr));
        user_label->setText(QApplication::translate("stacked", "Username :", nullptr));
        psw_lab_form->setText(QApplication::translate("stacked", "Password :", nullptr));
        confirm_psw_lab->setText(QApplication::translate("stacked", "Confirm passowrd: ", nullptr));
        email_label->setText(QApplication::translate("stacked", "Email : ", nullptr));
        form_cancButton->setText(QApplication::translate("stacked", "Cancel", nullptr));
        form_regButton->setText(QApplication::translate("stacked", "Register", nullptr));
        label_4->setText(QApplication::translate("stacked", "connected user", nullptr));
        label_5->setText(QApplication::translate("stacked", "Icon", nullptr));
        pushButton_6->setText(QApplication::translate("stacked", "New file", nullptr));
        fileButton->setText(QApplication::translate("stacked", "File 1", nullptr));
        pushButton_3->setText(QApplication::translate("stacked", "PushButton", nullptr));
        pushButton_5->setText(QApplication::translate("stacked", "PushButton", nullptr));
        label_3->setText(QApplication::translate("stacked", "Recent files :", nullptr));
    } // retranslateUi

};

namespace Ui {
    class stacked: public Ui_stacked {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STACKED_H
