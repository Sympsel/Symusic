#pragma once

#include <QPushButton>
#include <QWidget>

#include "entity/PathManager.hpp"
#include "entity/Color.hpp"

class HeadWidget final : public QWidget {
    Q_OBJECT
private:
    static QPushButton* createControlButton(const QString& iconFileNameWithoutPath, QWidget* parent = nullptr) {
    return new QPushButton(QIcon(prefix::normalImages + iconFileNameWithoutPath), "", parent);
    }
public:
    explicit HeadWidget(QWidget* parent = nullptr);
    QWidget* createFunctionWidget(QWidget* parent);
    void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) const;
signals:
    // 向父窗口发送请求
    void closeRequested();
    void minimizeRequested();
    void maximizeRequested();

private:
    Color _color;
};