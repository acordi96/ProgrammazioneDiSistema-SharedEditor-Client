
#include <iostream>
#include "Headers/customcursor.h"

CustomCursor::CustomCursor():position(0),hasSelection(false),reverseSelection(false),selectionStart(0),selectionEnd(0){
}

CustomCursor::CustomCursor(const QTextCursor &qTextCursor):
position(qTextCursor.position()),
hasSelection(qTextCursor.hasSelection()),
reverseSelection(position==qTextCursor.selectionStart()),
selectionStart(qTextCursor.selectionStart()),
selectionEnd(qTextCursor.selectionEnd()){
}
/*
CustomCursor::CustomCursor(const QJsonObject &json) :
position(json["position"].toString().toUInt()),
hasSelection(json["has_selection"].toBool(false)),
reverseSelection(json["reverse_selection"].toBool(false)),
selectionStart(json["selection_start"].toString().toUInt()),
selectionEnd(json["selection_end"].toString().toUInt()){
}

QJsonObject CustomCursor::toQJsonObject() const{
    QJsonObject json;
    json.insert("position", QString::number(position));
    json.insert("has_selection", hasSelection);
    json.insert("reverse_selection", reverseSelection);
    json.insert("selection_start", QString::number(selectionStart));
    json.insert("selection_end", QString::number(selectionEnd));
    return json;
}
*/
QTextCursor CustomCursor::toQTextCursor(const QTextCursor &base) const {
    QTextCursor qtc = QTextCursor(base);
    qtc.clearSelection();
    if(hasSelection){
        qtc.setPosition((int)(reverseSelection?selectionEnd:selectionStart), QTextCursor::MoveMode::MoveAnchor);
        qtc.setPosition((int)(reverseSelection?selectionStart:selectionEnd), QTextCursor::MoveMode::KeepAnchor);
    }else{
        qtc.setPosition((int)position, QTextCursor::MoveMode::MoveAnchor);
    }
    return qtc;
}

CustomCursor::~CustomCursor() {
    //std::cout << "~CustomCursor()" << std::endl;
}
