#pragma once

#include "ListWidget.h"
#include "Sync.hpp"

class RecommendWidget : public QWidget {
private:
    static void syncButtonStyle(const std::initializer_list<QPushButton*>& buttons);

public:
    explicit RecommendWidget(QWidget* parent);

    static QWidget* createWidgetItem(const QString& name);
};
