#pragma once

#include <QWidget>

#include "entity/Color.hpp"
#include "Log.hpp"

class FrameStyleSheet {
public:
    static void setBorder(QWidget* widget, const bool enabled = false) {
        const Color& color = ColorTheme::getInstance().getColor();
        if (enabled) {
            if (widget) {
                widget->setStyleSheet(QString(
                    "background-color: rgb(%1);"
                    "border: 2px solid rgb(%2);"
                ).arg(color.background, color.border));
            }
        } else {
            if (widget) {
                widget->setObjectName("Widget");
                widget->setStyleSheet(QString(
                    "#Widget {"
                    "   background-color: rgb(%1);"
                    "   border: 2px solid rgb(%2);"
                    "}"
                ).arg(color.background, color.border));
            }
        }
    }
};
