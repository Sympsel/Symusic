#pragma once

#include <QLabel>
#include <QTreeWidget>
#include <QPushButton>
#include <vector>

#include "NavigationButton.h"

class ListWidget final :public QWidget {
    Q_OBJECT
public:
    explicit ListWidget(
        QWidget *parent,
        const QString& path,
        const QString& groupName,
        const std::initializer_list<NavigationButton*>& buttons);
private:
    QIcon _icon;
    QWidget* _groupHead;
    std::vector<NavigationButton*> _buttons;
};