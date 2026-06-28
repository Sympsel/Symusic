#pragma once

#include <QWidget>

#include "entity/Common.hpp"

class FrameStyleSheet {
public:
    static void setBorder(QWidget* widget, const bool enabled = false) {
        const auto& color = ColorTheme::getInstance();
        if (enabled) {
            if (widget) {
                widget->setStyleSheet(QString(
                    "background-color: rgb(%1);"
                    "border: 2px solid rgb(%2);"
                ).arg(color.getGlobalBGColor(), color.getBaseColor().bd));
            }
        } else {
            if (widget) {
                widget->setObjectName("Widget");
                widget->setStyleSheet(QString(
                    "#Widget {"
                    "   background-color: rgb(%1);"
                    "   border: 2px solid rgb(%2);"
                    "}"
                ).arg(color.getGlobalBGColor(), color.getBaseColor().bd));
            }
        }
    }
};
