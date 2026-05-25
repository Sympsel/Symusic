#include "NavigationButton.h"

NavigationButton::NavigationButton(const QString& iconPath, const QString& text, QWidget* parent): QToolButton(parent) {

    this->setIcon(QIcon(iconPath));
    this->setText(text);
    this->setIconSize(QSize(18, 18));
    this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->setMinimumHeight(36);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}
