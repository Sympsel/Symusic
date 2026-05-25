#pragma once

#include <QToolButton>

class NavigationButton final : public QToolButton {
public:
    explicit NavigationButton(const QString& iconPath, const QString& text, QWidget* parent = nullptr);
};