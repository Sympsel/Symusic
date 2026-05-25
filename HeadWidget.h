#pragma once

#include <qpushbutton.h>

#include <QWidget>

#include "Color.hpp"

class HeadWidget final : public QWidget {
    Q_OBJECT
public:
    explicit HeadWidget(QWidget* parent = nullptr);
    QWidget* createFunctionWidget(QWidget* parent);
    void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) const;
signals:
    // 向父窗口发送关闭请求
    void closeRequested();

private:
    Color _color;
};