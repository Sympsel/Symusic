#include "RecommendWidget.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <string>
#include <random>

#include "Sync.hpp"

void RecommendWidget::syncButtonStyle(const std::initializer_list<QPushButton*>& buttons,
                                      const int width,
                                      const int height) {
    const auto& color = ColorTheme::getInstance().getColor();
    QString widthStr = std::to_string(width).c_str();
    QString heightStr = std::to_string(height).c_str();
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
        ).arg(color.arrowButtonHover, color.arrowButtonPressed, widthStr, heightStr);

        button->setStyleSheet(style);
    }
}

void RecommendWidget::initPlaylist() {
    // 初始化推荐页面展示图片
    constexpr int maxIdx = 35;
    std::vector<int> idxs;
    for (int i = 1; i <= maxIdx; ++i) {
        idxs.emplace_back(i);
    }
    // 提供随机种子
    std::random_device rd;
    // 创建一个随机数发生器
    std::mt19937 g(rd());
    // 洗牌算法
    std::shuffle(idxs.begin(), idxs.end(), g);

    for (size_t i{}; i < idxs.size() / 2; ++i) {
        auto id = QString(std::to_string(idxs[i]).c_str());
        _todayRecommendList.emplace_back(
            new PlaylistItem(
                ":/images/items/" + id + ".png",
                id,
                this
            )
        );
    }
    for (size_t i{idxs.size() / 2}; i < idxs.size(); ++i) {
        auto id = QString(std::to_string(idxs[i]).c_str());
        _youMayLikeList.emplace_back(
            new PlaylistItem(
                ":/images/items/" + id + ".png",
                id,
                this
            )
        );
    }
}

RecommendWidget::Items RecommendWidget::displayList(const Items& items, const int begin) const {
    if (begin >= static_cast<int>(items.size())) {
        LOG_ERROR() << "越界访问";
        throw std::runtime_error("越界访问");
    }
    return Items{items.begin() + begin, items.begin() + begin + _rowSize};
}

RecommendWidget::RecommendWidget(QWidget* parent) : QWidget(parent) {
    // 初始化推荐列表
    initPlaylist();
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    const auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);
    const auto scrollContent = new QWidget();
    const auto contentLayout = new QVBoxLayout(scrollContent);
    // const auto halfSize = static_cast<unsigned long>(_playlist.size() / 2);
    // const std::vector<PlaylistItem*> recommendBank(_playlist.begin(), _playlist.begin() + halfSize);
    // const std::vector<PlaylistItem*> youMayLikeBank(_playlist.begin() + halfSize + 1, _playlist.end());

    auto recommendWidget = createWidgetItem("今日推荐", displayList(_todayRecommendList, 0));
    auto youMayLikeWidget = createWidgetItem("猜你喜欢", displayList(_youMayLikeList, 0));

    Sync::widgetToLayout(contentLayout, {
                             recommendWidget, youMayLikeWidget
                         });
    contentLayout->addStretch(1);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

QWidget* RecommendWidget::createWidgetItem(const QString& name, const Items& items) {
    const auto vWidget = new QWidget();
    const auto vWidgetLayout = new QVBoxLayout(vWidget);
    vWidgetLayout->setContentsMargins(0, 0, 0, 0);
    vWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto label = new QLabel(name);
    label->setFixedHeight(30);
    label->setStyleSheet("font-size: 25px;");

    auto leftButton = new QPushButton(QIcon(":/images/向左.png"), "");
    auto centralWidget = new QWidget();
    const auto centralHLayout = new QHBoxLayout(centralWidget);
    // centralHLayout->setContentsMargins(10, 0, 10, 0);
    Sync::clearLayoutVMargins({centralHLayout});
    centralHLayout->addStretch(1);

    //connect(centralWidget, &QWidget::resize, centralWidget, [centralWidget]() {
    //    qDebug() << centralWidget->width();
    //});
    // centralWidget
    // todo 动态行扩容
    for (const auto& item : items) {
        centralHLayout->addWidget(item);
    }
    centralHLayout->addStretch(1);
    auto rightButton = new QPushButton(QIcon(":/images/向右.png"), "");
    syncButtonStyle({leftButton, rightButton}, 30, 120);
    auto widgetH = new QWidget();
    widgetH->setContentsMargins(0, 0, 0, 0);

    const auto hWidgetLayout = new QHBoxLayout(widgetH);
    Sync::widgetToLayout(hWidgetLayout, {leftButton, centralWidget, rightButton});
    //hWidgetLayout->addWidget(leftButton);
    //hWidgetLayout->addStretch(1);
    //hWidgetLayout->addWidget(rightButton);

    Sync::widgetToLayout(vWidgetLayout, {label, widgetH});
    return vWidget;
}

RecommendWidget::~RecommendWidget() {
    for (const auto item : _todayRecommendList) {
        delete item;
    }
    _todayRecommendList.clear();
    for (const auto item : _youMayLikeList) {
        delete item;
    }
    _youMayLikeList.clear();
}
