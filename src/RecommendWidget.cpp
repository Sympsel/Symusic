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
        _contain["今日推荐"].list.emplace_back(
            new PlaylistItem(
                ":/images/items/" + id + ".png",
                QString("推荐-%1").arg(i + 1, 3, 10, '0'),
                // 这里先不挂载到对象树上，等待布局管理器自己处理
                nullptr
            )
        );
    }
    for (size_t i{idxs.size() / 2}; i < idxs.size(); ++i) {
        auto id = QString(std::to_string(idxs[i]).c_str());
        _contain["猜你喜欢"].list.emplace_back(
            new PlaylistItem(
                ":/images/items/" + id + ".png",
                QString("推荐-%1").arg(i + 1, 3, 10, '0'),
                nullptr
            )
        );
    }
}

RecommendWidget::Items RecommendWidget::displayList(const QString& name) const {
    const auto& alist = _contain.at(name);
    if (alist.begin >= static_cast<int>(alist.list.size())) {
        LOG_ERROR() << "越界访问";
        throw std::runtime_error("越界访问");
    }
    Items playlist_items{
        alist.list.begin() + alist.begin,
        std::min(
            alist.list.begin() + alist.begin + _rowSize,
            alist.list.end()
        )
    };
    return playlist_items;
}

RecommendWidget::RecommendWidget(QWidget* parent) : QWidget(parent) {
    // 初始化防抖定时器
    _resizeTimer = new QTimer(this);
    _resizeTimer->setSingleShot(true);
    connect(_resizeTimer, &QTimer::timeout, this, [this]() {
        // 定时器触发时，窗口已经稳定
        updateRowSize();
    });
    const auto mainLayout = new QVBoxLayout(this);
    Sync::clearLayoutMargins(mainLayout);

    const auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);
    const auto scrollContent = new QWidget();
    const auto contentLayout = new QVBoxLayout(scrollContent);

    // 初始化推荐列表
    initPlaylist();
    auto recommendWidget = createPlateWidget("今日推荐");
    auto youMayLikeWidget = createPlateWidget("猜你喜欢");

    Sync::widgetToLayout(contentLayout, {
                             recommendWidget, youMayLikeWidget
                         });
    contentLayout->addStretch(1);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

void RecommendWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    // 启动防抖定时器，延迟启动防止更新错误
    _resizeTimer->start(100);
}

void RecommendWidget::updateRowSize() {
    const int widgetWidth = this->width();
    constexpr int itemLen = 120;
    constexpr int spaceLen = 8;
    // 减去左右箭头按钮和边距的总宽度
    const int availableWidth = widgetWidth - 120;

    if (const int newRowSize = availableWidth / (itemLen + spaceLen);
        newRowSize > 0 && newRowSize != _rowSize) {
        _rowSize = newRowSize;

        // 调整列表的begin，确保不会越界，防止窗口放大不会往前扩容
        for (auto& [key, alist] : _contain) {
            if (const int totalSize = static_cast<int>(alist.list.size());
                alist.begin + _rowSize > totalSize) {
                alist.begin = std::max(0, totalSize - _rowSize);
            }
        }

        updateWidgetLayout("今日推荐");
        updateWidgetLayout("猜你喜欢");
    }
}

void RecommendWidget::updateButtonVisibility(const QString& name) {
    if (_contain.find(name) != _contain.end()) {
        const auto& alist = _contain.at(name);

        if (const bool couldGoLeft = alist.begin != 0) {
            alist.leftButton->setEnabled(couldGoLeft);
            alist.leftButton->setIcon(QIcon(":/images/向左.png"));
        } else {
            alist.leftButton->setIcon(QIcon());
        }

        if (const bool couldGoRight = alist.begin + _rowSize < alist.list.size()) {
            alist.rightButton->setEnabled(couldGoRight);
            alist.rightButton->setIcon(QIcon(":/images/向右.png"));
        } else {
            alist.rightButton->setIcon(QIcon());
        }
    }
}

QWidget* RecommendWidget::createPlateWidget(const QString& name) {
    const auto items = displayList(name);
    const auto vWidget = new QWidget();
    const auto vWidgetLayout = new QVBoxLayout(vWidget);
    Sync::clearLayoutMargins(vWidgetLayout);
    vWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto label = new QLabel(name);
    label->setFixedHeight(30);
    label->setStyleSheet("font-size: 25px;");

    auto& leftButton = _contain.at(name).leftButton;
    leftButton = new QPushButton(QIcon(":/images/向左.png"), "");
    auto centralWidget = new QWidget();
    const auto centralHLayout = new QHBoxLayout(centralWidget);
    Sync::clearLayoutVMargins({centralHLayout});

    centralHLayout->addStretch(1);
    for (const auto& item : items) {
        // 添加到布局的同时设置父控件
        centralHLayout->addWidget(item);
    }
    centralHLayout->addStretch(1);

    auto& rightButton = _contain.at(name).rightButton;
    rightButton = new QPushButton(QIcon(":/images/向右.png"), "");
    syncButtonStyle({leftButton, rightButton}, 30, 120);
    auto widgetH = new QWidget();
    Sync::clearWidgetMargins(widgetH);

    const auto hWidgetLayout = new QHBoxLayout(widgetH);
    Sync::widgetToLayout(hWidgetLayout, {leftButton, centralWidget, rightButton});

    Sync::widgetToLayout(vWidgetLayout, {label, widgetH});

    if (_contain.find(name) != _contain.end()) {
        auto& alist = _contain.at(name);
        alist.widget = centralWidget;

        // 连接左右按钮信号
        connect(leftButton, &QPushButton::clicked, this, [this, name, &alist]() {
            if (int& begin = alist.begin; begin > 0) {
                --begin;
                LOG_DEBUG() << std::string(name.toUtf8()) << " begin: " << begin;
                updateWidgetLayout(name);
            }
        });
        connect(rightButton, &QPushButton::clicked, this, [this, name, &alist]() {
            if (int& begin = alist.begin;
                begin + _rowSize < static_cast<int>(alist.list.size())) {
                ++begin;
                LOG_DEBUG() << std::string(name.toUtf8()) << " begin: " << begin;
                updateWidgetLayout(name);
            }
        });

        updateButtonVisibility(name);
    }
    return vWidget;
}

void RecommendWidget::updateWidgetLayout(const QString& name) {
    if (const auto it = _contain.find(name); it != _contain.end() && it->second.widget != nullptr) {
        const auto& alist = it->second;
        auto* centralWidget = alist.widget;

        if (const auto layout = qobject_cast<QHBoxLayout*>(centralWidget->layout())) {
            const int currItemCount = layout->count() - 2;
            const auto newItems = displayList(name);
            const int newItemCount = static_cast<int>(newItems.size());

            if (newItemCount == currItemCount) {
                // 检查begin是否改变，即是否触发了滚动
                bool needUpdate = false;
                for (int i{}; i < currItemCount; ++i) {
                    if (const auto layoutItem = layout->itemAt(1 + i)) {
                        if (const auto currWidget = layoutItem->widget()) {
                            if (currWidget != newItems[i]) {
                                needUpdate = true;
                                break;
                            }
                        }
                    }
                }
                if (!needUpdate) {
                    updateButtonVisibility(name);
                    return;
                }
            }

            // 清空当前布局中除弹簧外所有项目
            while (layout->count() > 2) {
                if (const auto layoutItem = layout->takeAt(1)) {
                    if (auto* widget = layoutItem->widget()) {
                        widget->setParent(this);
                    }
                    delete layoutItem;
                }
            }

            // 重新添加新项目
            for (int i{}; i < newItemCount; ++i) {
                layout->insertWidget(1 + i, newItems[i]);
            }
            layout->invalidate();
            layout->activate();
            centralWidget->update();

            updateButtonVisibility(name);
        }
    }
}

RecommendWidget::~RecommendWidget() {
    for (auto& [_, alist] : _contain) {
        for (const auto& item : alist.list) {
            delete item;
        }
    }
    _contain.clear();
}
