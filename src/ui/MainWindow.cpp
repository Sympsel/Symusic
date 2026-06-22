#include "ui/MainWindow.h"

#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QStatusBar>
#include <ranges>

#include "ui/CommonPageWidget.h"
#include "ui/HeadWidget.h"
#include "ui/NavigationWidget.h"
#include "ui/PlaySlider.h"
#include "ui/RecommendWidget.h"
#include "ui/SongInfoPage.h"
#include "utils/Create.hpp"
#include "utils/FrameStyleSheet.hpp"
#include "utils/Log.hpp"
#include "utils/Sync.hpp"

MainWindow::MainWindow(QWidget* parent, const bool statusBarVisible, const bool debugBorder)
    : QMainWindow(parent) {
    {
        this->resize(848, 655);
        this->setWindowFlag(Qt::FramelessWindowHint);
        QStatusBar* statusBar = this->statusBar();
        statusBar->addWidget(new QLabel("就绪"));
        statusBar->addPermanentWidget(new QLabel("Alt + 鼠标左键拖拽窗口"));
        statusBar->setVisible(statusBarVisible);
    }

    _mapOfNavigationButtonsToWidget.resize(_pageCount);
    _songInfoPage = nullptr;

    const auto centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    const auto headBodyLayout = new QVBoxLayout(centralWidget);

    const auto headWidget = new HeadWidget(this);
    handleRequestFromHeadButton(headWidget);

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
    auto 推荐 = new NavigationButton("推荐.png", "     推荐");
    auto 电台 = new NavigationButton("电台.png", "     电台");
    auto 漫游 = new NavigationButton("漫游.png", "     漫游");
    const auto onlineMusic = new NavigationWidget(bodyWidget, "在线.png", "在线音乐", {
                                                      推荐, 电台, 漫游
                                                  });
    auto 我喜欢的 = new NavigationButton("喜欢.png", "     我喜欢的");
    auto 本地下载 = new NavigationButton("下载.png", "     本地下载");
    auto 最近播放 = new NavigationButton("最近播放.png", "     最近播放");
    const auto myMusic = new NavigationWidget(bodyWidget, "我的.png", "我的音乐", {
                                                  我喜欢的, 本地下载, 最近播放
                                              });
    Sync::widgetToLayout(leftLayout, {
                             onlineMusic, myMusic
                         });
    const auto buttons = {
        推荐, 电台, 漫游, 我喜欢的, 本地下载, 最近播放
    };
    syncButtonToContain(buttons);
    Sync::buttonNoFocus(buttons);

    leftLayout->addStretch(1);
    return leftWidget;
}

QWidget* MainWindow::createControlWidget(QWidget* parent) {
    const auto controlWidget = new QWidget(parent);
    const auto controlLayout = new QHBoxLayout(controlWidget);
    Sync::clearWidgetMargins(controlWidget);
    _volumeSlider = VolumeSlider::getInstance(this);
    _volumeSlider->setVisible(false);

    // [图片 歌名/歌手]
    QLabel* songCover = Create::squarePixmap(controlWidget, "Sympsel.png", 50);
    const auto leftWidget = new QWidget(controlWidget);
    const auto songInfoWidget = new QWidget(leftWidget);
    const auto leftLayout = new QHBoxLayout(leftWidget);

    const auto songInfoLayout = new QVBoxLayout(songInfoWidget);
    const auto songName = new QLabel("歌曲");
    const auto singer = new QLabel("歌手");

    Sync::widgetToLayout(songInfoLayout, {songName, singer});
    Sync::widgetToLayout(leftLayout, {songCover, songInfoWidget});

    // [随机播放 上一首 暂停/播放 下一首 音量 添加到我喜欢]
    const auto centralWidget = new QWidget(controlWidget);
    const auto centralLayout = new QHBoxLayout(centralWidget);
    const auto playModeButton = Create::buttonOnlyIcon("随机播放.png", centralWidget);
    playModeButton->setToolTip("点击切换到xxx模式");
    playModeButton->setToolTipDuration(3000);
    const auto prevButton = Create::buttonOnlyIcon("上一首.png", centralWidget);
    const auto playButton = Create::buttonOnlyIcon("播放.png", centralWidget);
    const auto nextButton = Create::buttonOnlyIcon("下一首.png", centralWidget);
    const auto volumeButton = Create::buttonOnlyIcon("中等音量.png", centralWidget);
    _volumeSlider->setRelationButton(volumeButton);
    connect(volumeButton, &QPushButton::clicked, this, [this, volumeButton]() {
        const QPoint buttonGlobalPos = volumeButton->mapToGlobal(QPoint(0, 0));
        const QSize sliderSize = _volumeSlider->sizeHint();

        const int x = buttonGlobalPos.x() + (volumeButton->width() - sliderSize.width()) / 2;
        const int y = buttonGlobalPos.y() - sliderSize.height() - 10;

        _volumeSlider->showAtPosition(QPoint(x, y));
    });
    const auto addToButton = Create::buttonOnlyIcon("添加.png", centralWidget);
    addToButton->setToolTip("从本地添加");
    connect(addToButton, &QPushButton::clicked, this, [this]() {
        QFileDialog fileDialog(this);
        fileDialog.setWindowTitle("添加本地音乐");

        // todo 替换为稳定的路径
        QDir dir{QDir::currentPath()};
        dir.cdUp();
        qDebug() << dir;
        fileDialog.setDirectory(dir);
        // 设置一次性可以打开多个
        fileDialog.setFileMode(QFileDialog::ExistingFiles);
        fileDialog.setNameFilter("代码文件(*.h *.cpp *.hpp)");
        if (QDialog::Accepted == fileDialog.exec()) {
            LOG_DEBUG() << "打开";
            const auto urls = fileDialog.selectedUrls();
            SongManager::getInstance().append(SongManager::getInstance().getDownloadList(), urls);
        } else {
            LOG_DEBUG() << "取消";
        }
    });
    addToButton->setToolTipDuration(3000);

    const auto buttons = {
        playModeButton, prevButton, playButton, nextButton, volumeButton, addToButton
    };

    constexpr QSize buttonsSize(30, 30);
    Sync::buttonFixedSize(buttonsSize, buttons);
    Sync::buttonNoFocus(buttons);
    Sync::buttonToHLayout(centralLayout, buttons);

    // [进度]
    const auto rightWidget = new QWidget(controlWidget);
    const auto rightLayout = new QHBoxLayout(rightWidget);
    const auto processLabel = new QLabel("00:00/3:14");
    const auto lyricsButton = new QPushButton(QIcon(":/images/词.png"), "");
    lyricsButton->setFixedSize(buttonsSize);
    syncButtonBackground(buttons);
    syncButtonBackground({lyricsButton});
    rightLayout->addWidget(processLabel);
    rightLayout->addWidget(lyricsButton, 0, Qt::AlignCenter);

    controlLayout->addWidget(leftWidget);
    controlLayout->addStretch(1);
    controlLayout->addWidget(centralWidget);
    controlLayout->addStretch(1);
    controlLayout->addWidget(rightWidget);
    Sync::clearLayoutVMargins({controlLayout, leftLayout, centralLayout, rightLayout});
    return controlWidget;
}

QWidget* MainWindow::createMainStackedWidget(QWidget* parent) {
    const auto mainWidget = new QWidget(parent);
    const auto bodyRightLayout = new QVBoxLayout(mainWidget);
    bodyRightLayout->setContentsMargins(4, 0, 4, 0);

    _mainStackedWidget = new QStackedWidget(mainWidget);

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

        const auto 推荐_页 = new RecommendWidget(_mainStackedWidget);
        const auto 电台_页 = createPage("电台", _mainStackedWidget);
        const auto 漫游_页 = createPage("漫游页面", _mainStackedWidget);
        const auto 我喜欢的_页 = new CommonPageWidget(
            "我喜欢的",
            "Sympsel.png",
            "这里是你爱听的",
            _mainStackedWidget);
        我喜欢的_页->initData(SongManager::getInstance().getLikedList());
        connect(我喜欢的_页, &CommonPageWidget::songItemDoubleClicked, this,
                [this, 我喜欢的_页](const SongPtr& song) {
                    if (_songInfoPage != nullptr) {
                        _songInfoPage->close();
                        _songInfoPage = nullptr;
                    }
                    _songInfoPage = new SongInfoPage(song);

                    // 监听喜欢状态变化，刷新列表
                    connect(_songInfoPage, &QWidget::destroyed, this, [this, 我喜欢的_页]() {
                        我喜欢的_页->reloadData(SongManager::getInstance().getLikedList());
                        LOG_DEBUG() << "喜欢状态改变，已刷新喜欢列表";
                        _songInfoPage = nullptr;
                    });
                    _songInfoPage->setAttribute(Qt::WA_DeleteOnClose);
                    _songInfoPage->show();
                });
        const auto 本地下载_页 = createPage("本地下载页面", _mainStackedWidget);
        const auto 最近播放_页 = createPage("最近播放页面", _mainStackedWidget);

        const std::initializer_list<QWidget*>& pages = {
            推荐_页, 电台_页, 漫游_页, 我喜欢的_页, 本地下载_页, 最近播放_页
        };
        Sync::widgetToStackedWidget(_mainStackedWidget, pages);
        syncWidgetToContain(pages);
    }

    // 进度条
    const auto sliderWidget = PlaySlider::getInstance(mainWidget);

    // 控制按钮区
    QWidget* controlWidget = createControlWidget(mainWidget);

    Sync::widgetToLayout(bodyRightLayout, {_mainStackedWidget, sliderWidget, controlWidget});
    Sync::clearLayoutMargins(bodyRightLayout);
    bodyRightLayout->setSpacing(0);
    return mainWidget;
}

QWidget* MainWindow::createBodyWidget(QWidget* parent) {
    const auto bodyWidget = new QWidget(parent);

    bodyWidget->setMinimumHeight(100);

    const auto leftWidget = createBodyLeftWidget(bodyWidget);
    const auto line = Create::line(QFrame::VLine);
    const auto rightWidget = createMainStackedWidget(bodyWidget);

    const auto bodyLayout = new QHBoxLayout(bodyWidget);
    Sync::widgetToLayout(bodyLayout, {leftWidget, line, rightWidget});
    // 将按钮与页面连接
    const auto size = static_cast<size_t>(_mainStackedWidget->count());
    if (size != _mapOfNavigationButtonsToWidget.size()) {
        LOG_FATAL() << std::format("程序出错：页面数 {} 和 导航按钮数 {} 不匹配",
                                   _mainStackedWidget->count(), _mapOfNavigationButtonsToWidget.size());
        exit(EXIT_FAILURE);
    }

    // 保存每个按钮的原始样式表
    std::vector<QString> originalStyleSheets;
    for (const auto& _navigationButton : _mapOfNavigationButtonsToWidget | std::views::keys) {
        originalStyleSheets.emplace_back(_navigationButton->styleSheet());
    }

    // 点击按钮切换页面
    for (size_t i = 0; i < size; ++i) {
        connect(_mapOfNavigationButtonsToWidget[i].first, &QPushButton::clicked, this, [this, i]() {
            _mainStackedWidget->setCurrentIndex(static_cast<int>(i));
        });
    }

    const Color& color = ColorTheme::getInstance().getColor();
    // 监听页面切换信号
    connect(_mainStackedWidget, &QStackedWidget::currentChanged, this,
            [this, originalStyleSheets, color](const int index) {
                for (size_t i = 0; i < _mapOfNavigationButtonsToWidget.size(); ++i) {
                    const bool selected = (static_cast<int>(i) == index);
                    _mapOfNavigationButtonsToWidget[i].first->setSelected(selected, originalStyleSheets[i], color);
                }

                if (const auto currentPage = _mainStackedWidget->widget(index)) {
                    if (const auto likedPage = qobject_cast<CommonPageWidget*>(currentPage)) {
                        if (likedPage->getPageName() == "我喜欢的") {
                            likedPage->reloadData(SongManager::getInstance().getLikedList());
                        }
                    }
                }
            });

    // 默认选中第一个页面
    if (!_mapOfNavigationButtonsToWidget.empty()) {
        _mapOfNavigationButtonsToWidget[0].first->setSelected(true, originalStyleSheets[0], color);
    }

    return bodyWidget;
}

void MainWindow::syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) {
    const Color& color = ColorTheme::getInstance().getColor();
    Sync::buttonBackground(buttons, color.background, color.hoverOn, color.pressed);
}

void MainWindow::syncButtonToContain(
    const std::initializer_list<NavigationButton*>& buttons) {
    if (static_cast<int>(buttons.size()) == _pageCount) {
        int i = 0;
        for (const auto& button : buttons) {
            _mapOfNavigationButtonsToWidget[i++].first = button;
        }
    }
}

void MainWindow::syncWidgetToContain(
    const std::initializer_list<QWidget*>& widgets) {
    if (static_cast<int>(widgets.size()) == _pageCount) {
        int i = 0;
        for (const auto& widget : widgets) {
            _mapOfNavigationButtonsToWidget[i++].second = widget;
        }
    }
}

void MainWindow::handleRequestFromHeadButton(const HeadWidget* headWidget) {
    connect(headWidget, &HeadWidget::maximizeRequested, this, [this]() {
        if (this->isMaximized()) {
            LOG_INFO() << "窗口恢复正常大小";
            this->showNormal();
        } else {
            LOG_INFO() << "窗口最大化";
            this->showMaximized();
        }
    });

    connect(headWidget, &HeadWidget::minimizeRequested, this, [this]() {
        LOG_INFO() << "窗口最小化";
        this->showMinimized();
    });

    connect(headWidget, &HeadWidget::closeRequested, this, [this]() {
        LOG_INFO() << "程序正常退出";
        this->close();
    });
}

void MainWindow::setBorder(const bool enabled) const {
    LOG_DEBUG() << std::format("启用{}边框", enabled ? "调试" : "常规");
    FrameStyleSheet::setBorder(this->centralWidget(), enabled);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if ((event->buttons() & Qt::LeftButton) &&
        // 检测当前修饰键状态中，Alt 键对应的位是否为 1，下同
        (event->modifiers() & Qt::AltModifier)) {
        move(event->globalPosition().toPoint() - _dragPos);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if ((event->button() == Qt::LeftButton) &&
        (event->modifiers() & Qt::AltModifier)) {
        _dragPos = event->globalPosition().toPoint() - geometry().topLeft();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // 先关闭歌曲信息页
    if (_songInfoPage) {
        _songInfoPage->blockSignals(true);
        _songInfoPage->close();
        _songInfoPage = nullptr;
    }

    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow() {
    LOG_DEBUG() << std::format("程序退出，退出时宽高为 [{}, {}]", this->width(), this->height());
}
