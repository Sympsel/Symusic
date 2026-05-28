#include "RecommendWidget.h"

#include <QScrollArea>

void RecommendWidget::syncButtonStyle(const std::initializer_list<QPushButton*>& buttons) {
    const auto& color = ColorTheme::getInstance().getColor();
    QString width = "30", height = "80";
    for (const auto button : buttons) {
        button->setObjectName("navArrowButton");

        QString style = QString(
            "QPushButton#navArrowButton {"
            "    padding: 0px !important;"
            "    margin: 0px !important;"
            "    border: none !important;"
            "    border-radius: 2px;"
            "    background-color: transparent;"
            "    min-width: %3px !important;"
            "    max-width: %3px !important;"
            "    min-height: %4px !important;"
            "    max-height: %4px !important;"
            "}"
            "QPushButton#navArrowButton:hover {"
            "    background-color: rgb(%1);"
            "}"
            "QPushButton#navArrowButton:pressed {"
            "    background-color: rgb(%2);"
            "}"
        ).arg(color.arrowButtonHover, color.arrowButtonPressed, width, height);

        button->setStyleSheet(style);
    }
}

RecommendWidget::RecommendWidget(QWidget* parent): QWidget(parent) {
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);
    auto scrollContent = new QWidget();
    auto contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    auto recommendWidget = createWidgetItem("今日推荐");
    auto youMayLikeWidget = createWidgetItem("猜你喜欢");

    Sync::widgetToLayout(contentLayout, {
                             recommendWidget, youMayLikeWidget
                         });
    contentLayout->addStretch(1);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

QWidget* RecommendWidget::createWidgetItem(const QString& name) {
    const auto vWidget = new QWidget();
    const auto vWidgetLayout = new QVBoxLayout(vWidget);
    vWidgetLayout->setContentsMargins(0, 0, 0, 0);
    vWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto label = new QLabel(name);
    label->setFixedHeight(30);
    label->setStyleSheet("font-size: 25px;");

    auto leftButton = new QPushButton(QIcon(":/images/向左.png"), "");
    // auto centralWidget = new QWidget();
    auto rightButton = new QPushButton(QIcon(":/images/向右.png"), "");
    syncButtonStyle({leftButton, rightButton});
    auto widgetH = new QWidget();
    widgetH->setContentsMargins(0, 0, 0, 0);

    const auto hWidgetLayout = new QHBoxLayout(widgetH);
    hWidgetLayout->addWidget(leftButton);
    // todo replace it
    hWidgetLayout->addStretch(1);
    hWidgetLayout->addWidget(rightButton);

    Sync::widgetToLayout(vWidgetLayout, {label, widgetH});
    return vWidget;
}
