//
// Created by Sam on 07/ott/2020.
//

#ifndef PROGETTO_PROVA_DOCUMENTSHARED_H
#define PROGETTO_PROVA_DOCUMENTSHARED_H

#include <QTextEdit>
#include <QTimer>

class documentShared : public QTextEdit
{
Q_OBJECT

public:
    documentShared(QWidget* parent) : QTextEdit(parent) {}
    void insertFromMimeData(const QMimeData * source) override;
    void setAlignmentsVector(QVector<std::pair<int,int>> alignmentVector);
    void setEditorColor(QString color);
    void addRemoteCursor(QString username, std::pair<QString, int> colorAndPos);
    void setBtnColorChecked(bool val);
    QVector<std::pair<int,int>> getAlignmentsVector();
    QString getEditorColor();
    QHash<QString, std::pair<QString,int>> getRemoteCursors();
    bool isBtnColorChecked();
    QTimer timer;

private:
    QVector<std::pair<int,int>> alignments;
    QString editorColor;
    bool isColorBtnChecked = false;
    bool showHorizontalRect = false;
    QHash<QString, std::pair<QString, int>> remoteCursors;
    int calculateOffsetHeight(int curFontSize);
    int calculateOffsetY(int curFontSize);
    int calculateWidthForUsername(QString& username);
    int getAlignmentValues(QVector<std::pair<int,int>>& alignments);
    QString hexToHtmlRGB(QString hexARGB);
    QString replaceColorWithEditorColor(QString text);
    QStringList getRegexListFromHTML(QString text, QRegularExpression rx);

protected:
    void paintEvent(QPaintEvent * event);

signals:
    void updateAlignmentButton();

public slots:
    void removeRemoteCursor(std::string username);
    void changeRemoteCursor(QString username, QString color, int pos);
    void hideHorizontalRect();
};

#endif //PROGETTO_PROVA_DOCUMENTSHARED_H
