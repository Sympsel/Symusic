module;

#include <QTreeWidget>
#include <vector>

#include "NavigationButton.h"

export module symusic.ui.navigation_widget;

export class NavigationWidget final :public QWidget {
    Q_OBJECT
public:
    explicit NavigationWidget(
        QWidget *parent,
        const QString& iconPath,
        const QString& groupName,
        const std::initializer_list<NavigationButton*>& buttons);
private:
    QIcon _icon;
    QWidget* _groupHead;
    std::vector<NavigationButton*> _buttons;
};