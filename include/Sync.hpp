#pragma once

#include <QPushButton>
#include <QLayout>
#include <QStackedWidget>

class Sync {
public:
    static void buttonBackground(const std::initializer_list<QPushButton*>& buttons,
                                 const QString& backgroundColor, const QString& hoverColor,
                                 const QString& pressedColor) {
        for (const auto button : buttons) {
            button->setStyleSheet(QString(
                "QPushButton {"
                "    background-color: rgb(%1);"
                "}"
                "QPushButton:hover {"
                "    background-color: rgb(%2);"
                "}"
                "QPushButton:pressed {"
                "    background-color: rgb(%3);"
                "}"
            ).arg(backgroundColor, hoverColor, pressedColor));
        }
    }

    static void buttonSize(const QSize size, const std::initializer_list<QPushButton*>& buttons) {
        for (const auto button : buttons) {
            button->setFixedSize(size);
        }
    }

    static void buttonToLayout(QLayout* layout, const std::initializer_list<QPushButton*>& buttons) {
        for (const auto button : buttons) {
            layout->addWidget(button);
        }
    }

    static void widgetToLayout(QLayout* layout, const std::initializer_list<QWidget*>& widgets) {
        for (const auto widget : widgets) {
            layout->addWidget(widget);
        }
    }

    static void widgetToStackedWidget(QStackedWidget* stackedWidget, const std::initializer_list<QWidget*>& widgets) {
        for (const auto widget : widgets) {
            stackedWidget->addWidget(widget);
            if (widget->parentWidget() == nullptr) {
                widget->setParent(stackedWidget);
            }
        }
    }

    static void widgetParent(QWidget* parent, const std::initializer_list<QWidget*>& widgets) {
        for (const auto widget : widgets) {
            widget->setParent(parent);
        }
    }

    static void widgetContain(std::vector<QWidget*> contain, const std::initializer_list<QWidget*>& widgets) {
        for (const auto& widget : widgets) {
            contain.emplace_back(widget);
        }
    }
};
