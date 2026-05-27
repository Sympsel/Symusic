#include "mainwindow.h"

#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

#include "ListWidget.h"
#include "Log.hpp"
#include "Sync.hpp"
#include "Create.hpp"
#include "HeadWidget.h"

MainWindow::MainWindow(QWidget* parent, const bool statusBarVisible, const bool debugBorder)
    : QMainWindow(parent) {
    {
        this->resize(800, 600);
        this->setWindowFlag(Qt::FramelessWindowHint);
        QStatusBar* statusBar = this->statusBar();
        statusBar->addWidget(new QLabel("就绪"));
        statusBar->addPermanentWidget(new QLabel("Alt + 鼠标左键拖拽窗口"));
        statusBar->setVisible(statusBarVisible);
    }

    const auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    const auto headBodyLayout = new QVBoxLayout(centralWidget);

    const auto headWidget = new HeadWidget(this);
    connect(headWidget, &HeadWidget::closeRequested, this, [this]() {
        LOG_INFO() << "程序正常退出";
        this->close();
    });

    QWidget* bodyWidget = createBodyWidget();
    QFrame* line = Create::line(QFrame::HLine);

    // 4. 添加到布局
    Sync::widgetToLayout(headBodyLayout, {
                             headWidget, line
                         });
    headBodyLayout->addWidget(bodyWidget, 1);

    setBorder(debugBorder);
}

QWidget* MainWindow::createBodyLeftWidget(QWidget* bodyWidget) {
    const auto leftWidget = new QWidget(bodyWidget);
    leftWidget->setMaximumWidth(150);
    const auto leftLayout = new QVBoxLayout(leftWidget);

    // 这里文本缩进写死是因为按钮宽度固定
    auto 推荐 = new NavigationButton(":/images/推荐.png", "     推荐");
    auto 电台 = new NavigationButton(":/images/电台.png", "     电台");
    auto 漫游 = new NavigationButton(":/images/漫游.png", "     漫游");
    const auto onlineMusic = new ListWidget(bodyWidget, ":/images/在线.png", "在线音乐", {
                                                推荐, 电台, 漫游
                                            });
    auto 我喜欢的 = new NavigationButton(":/images/喜欢.png", "     我喜欢的");
    auto 本地下载 = new NavigationButton(":/images/下载.png", "     本地下载");
    auto 最近播放 = new NavigationButton(":/images/最近播放.png", "     最近播放");
    const auto myMusic = new ListWidget(bodyWidget, ":/images/我的.png", "我的音乐", {
                                            我喜欢的, 本地下载, 最近播放
                                        });
    Sync::widgetToLayout(leftLayout, {
                             onlineMusic, myMusic
                         });
    syncButtonContain(_navigationButtons, {
                          推荐, 电台, 漫游, 我喜欢的, 本地下载, 最近播放
                      });

    leftLayout->addStretch(1);
    return leftWidget;
}

QWidget* MainWindow::createControlWidget(QWidget* parent) {
    const auto controlWidget = new QWidget(parent);
    const auto controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(4, 0, 4, 0);

    // [图片 歌名/歌手]
    QLabel* songCover = Create::squarePixmap(controlWidget, ":/images/Sympsel.png", 40);
    const auto leftWidget = new QWidget(controlWidget);
    const auto leftInnerWidget = new QWidget(leftWidget);
    const auto songInfoLayout = new QHBoxLayout(leftWidget);

    const auto songInfoInnerLayout = new QVBoxLayout(leftInnerWidget);
    const auto songName = new QLabel("歌曲");
    const auto singer = new QLabel("歌手");
    songInfoInnerLayout->addWidget(songName);
    songInfoInnerLayout->addWidget(singer);

    songInfoLayout->addWidget(songCover);
    songInfoLayout->addWidget(leftInnerWidget);

    // [随机播放 上一首 暂停/播放 下一首 音量 添加到我喜欢]
    const auto centralWidget = new QWidget(controlWidget);
    const auto centralLayout = new QHBoxLayout(centralWidget);
    const auto playModeButton = new QPushButton(QIcon(":/images/随机播放.png"), "", centralWidget);
    const auto prevButton = new QPushButton(QIcon(":/images/上一首.png"), "", centralWidget);
    const auto playButton = new QPushButton(QIcon(":/images/播放.png"), "", centralWidget);
    const auto nextButton = new QPushButton(QIcon(":/images/下一首.png"), "", centralWidget);
    const auto volumeButton = new QPushButton(QIcon(":/images/音量.png"), "", centralWidget);
    const auto addToButton = new QPushButton(QIcon(":/images/添加.png"), "", centralWidget);

    // QSlider* volumeSlider = new QSlider(Qt::Horizontal);
    // volumeSlider->setRange(0, 100);
    // volumeSlider->setValue(50);
    // volumeSlider->setFixedWidth(100);
    const auto buttons = {
        playModeButton, prevButton, playButton, nextButton, volumeButton, addToButton
    };
    Sync::buttonSize(QSize(30, 30), buttons);
    Sync::buttonToLayout(centralLayout, buttons);

    // [进度]
    const auto rightWidget = new QWidget(controlWidget);
    const auto rightLayout = new QHBoxLayout(rightWidget);
    const auto processLabel = new QLabel("00:00/3:14");
    const auto lyricsButton = new QPushButton(QIcon(":/images/词.png"), "", rightWidget);
    lyricsButton->setFixedSize(QSize(30, 30));
    syncButtonBackground(buttons);
    syncButtonBackground({lyricsButton});
    rightLayout->addWidget(processLabel);
    rightLayout->addWidget(lyricsButton);

    controlLayout->addWidget(leftWidget);
    controlLayout->addStretch(1);
    controlLayout->addWidget(centralWidget);
    controlLayout->addStretch(1);
    controlLayout->addWidget(rightWidget);
    return controlWidget;
}

QWidget* MainWindow::createBodyRightWidget(QWidget* parent) {
    const auto bodyRightWidget = new QWidget(parent);
    const auto bodyRightLayout = new QVBoxLayout(bodyRightWidget);
    bodyRightLayout->setContentsMargins(4, 0, 4, 0);

    _mainStackedWidget = new QStackedWidget(bodyRightWidget);

    {
        // todo: 替换这段代码
        // 为每个页面创建独立的按钮和布局
        auto createPage = [](const QString& text, QWidget* stack) -> QWidget* {
            const auto page = new QWidget(stack);
            const auto layout = new QVBoxLayout(page);
            const auto btn = new QPushButton(text, page);
            layout->addWidget(btn, 0, Qt::AlignCenter);
            layout->addStretch(1);
            return page;
        };

        QWidget* 推荐_页 = createPage("推荐页面", _mainStackedWidget);
        QWidget* 电台_页 = createPage("电台页面", _mainStackedWidget);
        QWidget* 漫游_页 = createPage("漫游页面", _mainStackedWidget);
        QWidget* 我喜欢的_页 = createPage("我喜欢的页面", _mainStackedWidget);
        QWidget* 本地下载_页 = createPage("本地下载页面", _mainStackedWidget);
        QWidget* 最近播放_页 = createPage("最近播放页面", _mainStackedWidget);

        Sync::widgetToStackedWidget(_mainStackedWidget, {
                                        推荐_页, 电台_页, 漫游_页, 我喜欢的_页, 本地下载_页, 最近播放_页
                                    });
    }

    const auto slider = new QSlider(Qt::Horizontal, bodyRightWidget);
    slider->setRange(0, 100);
    // 刻度显示在下方
    slider->setTickPosition(QSlider::TicksBelow);

    // 控制按钮区
    QWidget* controlWidget = createControlWidget(bodyRightWidget);

    Sync::widgetToLayout(bodyRightLayout, {_mainStackedWidget, slider, controlWidget});

    return bodyRightWidget;
}

QWidget* MainWindow::createBodyWidget(QWidget* parent) {
    const auto bodyWidget = new QWidget(parent);

    bodyWidget->setMinimumHeight(100);
    QWidget* leftWidget = createBodyLeftWidget(bodyWidget);

    QFrame* line = Create::line(QFrame::VLine);

    QWidget* rightWidget = createBodyRightWidget(bodyWidget);

    const auto bodyLayout = new QHBoxLayout(bodyWidget);
    Sync::widgetToLayout(bodyLayout, {leftWidget, line, rightWidget});

    // 将按钮与页面连接
    const size_t size = static_cast<size_t>(_mainStackedWidget->count());
    if (size != _navigationButtons.size()) {
        LOG_FATAL() << "页面数(" << _mainStackedWidget->count()
            << ")与按钮数(" << _navigationButtons.size() << ")不匹配";
        exit(EXIT_FAILURE);
    }

    // 保存每个按钮的原始样式表
    std::vector<QString> originalStyleSheets;
    for (size_t i = 0; i < _navigationButtons.size(); ++i) {
        originalStyleSheets.emplace_back(_navigationButtons[i]->styleSheet());
        LOG_DEBUG() << "按钮 " << i << " 的原始样式长度: " << originalStyleSheets[i].length();
    }

    // 点击按钮切换页面
    for (size_t i = 0; i < size; ++i) {
        connect(_navigationButtons[i], &QPushButton::clicked, this, [this, i]() {
            _mainStackedWidget->setCurrentIndex(static_cast<int>(i));
        });
    }

    const Color& color = ColorTheme::getInstance().getColor();
    // 监听页面切换信号，自动更新按钮高亮
    connect(_mainStackedWidget, &QStackedWidget::currentChanged, this,
            [this, originalStyleSheets, color](const int index) {
                for (size_t i = 0; i < _navigationButtons.size(); ++i) {
                    const bool selected = (static_cast<int>(i) == index);
                    _navigationButtons[i]->setSelected(selected, originalStyleSheets[i], color);
                }
                LOG_DEBUG() << "已切换到第 " << index << " 页";
            });

    // 默认选中第一个页面
    if (!_navigationButtons.empty()) {
        _navigationButtons[0]->setSelected(true, originalStyleSheets[0], color);
    }

    return bodyWidget;
}

QWidget* MainWindow::createFunctionWidget(QWidget* parent) {
    const auto functionWidget = new QWidget(parent);

    const auto layout = new QHBoxLayout(functionWidget);

    const auto settingsButton = new QPushButton(QIcon(":/images/设置.png"), "", functionWidget);
    const auto minimizeButton = new QPushButton(QIcon(":/images/最小化.png"), "", functionWidget);
    const auto maximizeButton = new QPushButton(QIcon(":/images/最大化.png"), "", functionWidget);
    const auto closeButton = new QPushButton(QIcon(":/images/关闭.png"), "", functionWidget);
    connect(closeButton, &QPushButton::clicked, this, [this]() {
        LOG_INFO() << "程序正常退出";
        this->close();
    });
    const auto buttons = {settingsButton, minimizeButton, maximizeButton, closeButton};
    this->syncButtonBackground(buttons);
    Sync::buttonSize(QSize(30, 30), buttons);

    layout->addStretch(1);
    Sync::buttonToLayout(layout, buttons);

    return functionWidget;
}

void MainWindow::syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) {
    const Color& color = ColorTheme::getInstance().getColor();
    Sync::buttonBackground(buttons, color.background, color.hoverOn, color.pressed);
}

void MainWindow::syncButtonContain(std::vector<NavigationButton*>& navigationButtonList,
                                   const std::initializer_list<NavigationButton*>& buttons) {
    for (const auto& button : buttons) {
        navigationButtonList.emplace_back(button);
    }
}

void MainWindow::setBorder(const bool enabled = false) const {
    const Color& color = ColorTheme::getInstance().getColor();
    QWidget* currentWidget = this->centralWidget();
    if (enabled) {
        if (currentWidget) {
            LOG_DEBUG() << "启用调试边框";
            currentWidget->setStyleSheet(QString(
                "background-color: rgb(%1);"
                "border: 2px solid rgb(%2);"
            ).arg(color.background, color.border));
        }
    } else {
        LOG_DEBUG() << "启用常规边框";
        if (currentWidget) {
            currentWidget->setObjectName("MainCentralWidget");
            currentWidget->setStyleSheet(QString(
                "#MainCentralWidget {"
                "   background-color: rgb(%1);"
                "   border: 2px solid rgb(%2);"
                "}"
            ).arg(color.background, color.border));
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if ((event->buttons() & Qt::LeftButton) &&
        // 检测当前修饰键状态中，Alt 键对应的位是否为 1，下同
        (event->modifiers() & Qt::AltModifier)) {
        move(event->globalPosition().toPoint() - _dragPos);
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if ((event->button() == Qt::LeftButton) &&
        (event->modifiers() & Qt::AltModifier)) {
        _dragPos = event->globalPosition().toPoint() - geometry().topLeft();
        return;
    }
    QWidget::mousePressEvent(event);
}

MainWindow::~MainWindow() = default;
