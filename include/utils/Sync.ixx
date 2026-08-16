module;

#include <QPushButton>
#include <QLayout>
#include <QStackedWidget>

#include "ui/NavigationButton.h"

export module symusic.uitls.sync;

export class Sync {
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

    static void buttonFixedSize(const QSize size, const std::initializer_list<QPushButton*>& buttons) {
        for (const auto button : buttons) {
            button->setFixedSize(size);
        }
    }

    static void buttonNoFocus(const std::initializer_list<QPushButton*>& buttons) {
        for (const auto button : buttons) {
            button->setFocusPolicy(Qt::NoFocus);
        }
    }

    static void buttonNoFocus(const std::initializer_list<NavigationButton*>& buttons) {
        for (const auto button : buttons) {
            button->setFocusPolicy(Qt::NoFocus);
        }
    }

    static void buttonToLayout(QLayout* layout, const std::initializer_list<QPushButton*>& buttons) {
        for (const auto button : buttons) {
            layout->addWidget(button);
        }
    }

    static void buttonToHLayout(QHBoxLayout* layout,
                                const std::initializer_list<QPushButton*>& buttons,
                                const bool isCentral = true) {
        for (const auto button : buttons) {
            if (isCentral) {
                layout->addWidget(button, 0, Qt::AlignCenter);
            } else {
                layout->addWidget(button);
            }
        }
    }

    static void buttonToVLayout(QVBoxLayout* layout,
                                const std::initializer_list<QPushButton*>& buttons,
                                const bool isCentral = true) {
        for (const auto button : buttons) {
            if (isCentral) {
                layout->addWidget(button, 0, Qt::AlignCenter);
            } else {
                layout->addWidget(button);
            }
        }
    }

    static void widgetToLayout(QLayout* layout, const std::initializer_list<QWidget*>& widgets) {
        for (const auto widget : widgets) {
            layout->addWidget(widget);
        }
    }

    static void widgetToLayout(QHBoxLayout* layout, const std::initializer_list<std::pair<QWidget*, int>>& widgets) {
        for (const auto& [widget, stretch] : widgets) {
            layout->addWidget(widget, stretch);
        }
    }

    static void widgetToLayout(QVBoxLayout* layout, const std::initializer_list<std::pair<QWidget*, int>>& widgets) {
        for (const auto& [widget, stretch] : widgets) {
            layout->addWidget(widget, stretch);
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

    static void clearLayoutVMargins(const std::initializer_list<QLayout*>& layouts) {
        for (const auto& layout : layouts) {
            auto margins = layout->contentsMargins();
            layout->setContentsMargins(margins.left(), 0, margins.right(), 0);
        }
    }

    static void clearLayoutMargins(QLayout* layout) {
        layout->setContentsMargins(0, 0, 0, 0);
    }

    static void clearWidgetMargins(QWidget* widget) {
        widget->setContentsMargins(0, 0, 0, 0);
    }

    static void enabledWidgetStyledBackground(
        const std::initializer_list<QWidget*>& widgets, const bool isOn) {
        for (const auto widget : widgets) {
            widget->setAttribute(Qt::WA_StyledBackground, isOn);
        }
    }

    static void widgetTransparentBackground(const std::initializer_list<QWidget*>& widgets) {
        widgetAppendStyleSheet(widgets, "background-color: transparent;");
    }

    static void appendStyleSheet(QWidget* widget, const QString& stylesheet) {
        widgetAppendStyleSheet({widget}, stylesheet);
    }

    static void widgetAppendStyleSheet(const std::initializer_list<QWidget*>& widgets, const QString& stylesheet) {
        const QString trimmedStyle = stylesheet.trimmed();
        for (const auto widget : widgets) {
            QString existingStyle = widget->styleSheet().trimmed();

            if (existingStyle.contains(trimmedStyle)) {
                continue;
            }

            widget->setStyleSheet(
                existingStyle.isEmpty()
                    ? stylesheet
                    : existingStyle + stylesheet
            );
        }
    }
};
