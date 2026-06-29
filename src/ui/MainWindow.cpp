#include "ui/MainWindow.h"

#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QStatusBar>
#include <ranges>

#include "entity/PlayManager.hpp"
#include "entity/StatusManager.hpp"
#include "ui/HeadWidget.h"
#include "ui/LyricsWidget.h"
#include "ui/MarqueeLabel.h"
#include "ui/NavigationWidget.h"
#include "ui/PlaySlider.h"
#include "ui/RecommendWidget.h"
#include "ui/SongInfoPage.h"
#include "utils/Create.hpp"
#include "utils/FrameStyleSheet.hpp"
#include "utils/Log.hpp"
#include "utils/Sync.hpp"

MainWindow::MainWindow(QWidget* parent, const bool statusBarVisible, const bool debugBorder)
    : QMainWindow(parent)
      , _volumeSlider()
      , _songInfoPage()
      , _lyricsWidget()
      , _playModeButton()
      , _prevButton()
      , _playButton()
      , _nextButton()
      , _volumeButton()
      , _addToButton()
      , _lyricsButton()
      , _songCover()
      , _songNameLabel()
      , _singerLabel()
      , _processLabel() {
    {
        this->resize(848, 655);
        this->setWindowFlag(Qt::FramelessWindowHint);
        QStatusBar* statusBar = this->statusBar();
        statusBar->addWidget(new QLabel("就绪"));
        statusBar->addPermanentWidget(new QLabel("Alt + 鼠标左键拖拽窗口"));
        statusBar->setVisible(statusBarVisible);

        const auto& statusManager = StatusManager::getInstance();
        connect(&statusManager, &StatusManager::statusChanged,
                this, [statusBar](const QString& msg, const int timeout) {
                    statusBar->showMessage(msg, timeout);
                });
        connect(&statusManager, &StatusManager::statusCleared,
                this, [statusBar]() {
                    statusBar->clearMessage();
                });
    }

    _mapOfButtonsToWidget.resize(_navPageCount);

    const auto centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    const auto headBodyLayout = new QVBoxLayout(centralWidget);

    const auto headWidget = new HeadWidget(this);
    handleRequestFromHeadButton(headWidget);

    QWidget* bodyWidget = createBodyWidget();
    QFrame* line = Create::line(QFrame::HLine);

    Sync::widgetToLayout(headBodyLayout, {
                             headWidget, line
                         });
    headBodyLayout->addWidget(bodyWidget, 1);

    initLyricsWidget();

    // 设置样式
    setupStyles();
    setBorder(debugBorder);
    // 连接信号
    setupConnections();
}

MainWindow::~MainWindow() {
    LOG_DEBUG() << std::format("程序退出，退出时宽高为 [{}, {}]", this->width(), this->height());
}

// ==================== 样式设置 ====================
void MainWindow::setupStyles() {
    // 保存按钮原始样式
    std::vector<QString> originalStyleSheets;
    for (const auto& navigationButton : _mapOfButtonsToWidget | std::views::keys) {
        originalStyleSheets.emplace_back(navigationButton->styleSheet());
    }

    // 设置默认选中状态
    if (!_mapOfButtonsToWidget.empty()) {
        _mapOfButtonsToWidget[0].first->setSelected(true, originalStyleSheets[0]);
    }

    // 设置控制按钮样式
    constexpr QSize buttonsSize(30, 30);
    Sync::buttonFixedSize(buttonsSize, {
                              _playModeButton, _prevButton, _playButton, _nextButton, _volumeButton, _addToButton
                          });
    Sync::buttonNoFocus({
        _playModeButton, _prevButton, _playButton, _nextButton, _volumeButton, _addToButton
    });
    syncButtonBackground({
        _playModeButton, _prevButton, _playButton, _nextButton, _volumeButton, _addToButton
    });
}

// ==================== 信号连接 ====================
void MainWindow::setupConnections() {
    // 保存按钮原始样式（用于页面切换）
    std::vector<QString> originalStyleSheets;
    for (const auto& navigationButton : _mapOfButtonsToWidget | std::views::keys) {
        originalStyleSheets.emplace_back(navigationButton->styleSheet());
    }

    // 点击按钮切换页面
    for (size_t i = 0; i < _mapOfButtonsToWidget.size(); ++i) {
        connect(_mapOfButtonsToWidget[i].first, &QPushButton::clicked, this, [this, i]() {
            _mainStackedWidget->setCurrentIndex(static_cast<int>(i));
        });
    }

    // 监听页面切换信号
    connect(_mainStackedWidget, &QStackedWidget::currentChanged, this,
            [this, originalStyleSheets](const int index) {
                if (index == _navPageCount) {
                    // 歌词页在 _navPageCount 的位置
                    StatusManager::getInstance().showMessage(
                        std::format("切换到: {}", "歌词页面"), 2000
                    );
                    return;
                }
                for (size_t i = 0; i < _mapOfButtonsToWidget.size(); ++i) {
                    const bool selected = (static_cast<int>(i) == index);
                    _mapOfButtonsToWidget[i].first->setSelected(selected, originalStyleSheets[i]);
                }

                if (const auto currentPage = _mainStackedWidget->widget(index)) {
                    if (const auto commonPage = qobject_cast<CommonPageWidget*>(currentPage)) {
                        commonPage->reloadData();
                        StatusManager::getInstance().showMessage(
                            std::format("切换到: {}，单击播放，双击打开歌曲详情页", commonPage->getPageName().toStdString()),
                            2000
                        );
                    }
                }
            });

    // PlayManager 相关信号连接
    auto& playManager = PlayManager::getInstance();

    // 封面更新
    connect(&playManager, &PlayManager::songPlayed, this, [this, &playManager]() {
        const auto song = playManager.getCurrPlay();
        if (song) {
            if (const QPixmap cover = song->getCover(); !cover.isNull()) {
                _songCover->setPixmap(cover.scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            } else {
                _songCover->setPixmap(
                    QPixmap(prefix::itemImages + "Sympsel.png").scaled(50, 50, Qt::KeepAspectRatioByExpanding,
                                                                       Qt::SmoothTransformation));
            }
        }
    });

    // 歌曲信息更新
    connect(&playManager, &PlayManager::songPlayed, this, [this]() {
        const auto song = PlayManager::getInstance().getCurrPlay();
        _songNameLabel->setMarqueeText("歌曲：" + song->getName());
        const QString artist = song->getArtist();
        _singerLabel->setMarqueeText("歌手：" + (artist.isEmpty() ? "未知" : artist));
    });

    // 播放按钮状态
    connect(&playManager, &PlayManager::songPlayed, this, [this]() {
        _playButton->setIcon(QIcon(prefix::normalImages + "停止.png"));
    });
    connect(&playManager, &PlayManager::playStateChanged, this, [this]() {
        _playButton->setIcon(QIcon(prefix::normalImages + "播放.png"));
    });

    // 进度更新
    connect(&playManager, &PlayManager::positionChanged, this, [this]() {
        _processLabel->setText(PlayManager::getInstance().getFormattedProgress());
    });

    // 播放模式按钮
    connect(_playModeButton, &QPushButton::clicked, this, [this]() {
        auto& playManager = PlayManager::getInstance();
        switch (playManager.getPlayMode()) {
        case PlayMode::ORDERED:
            playManager.setPlayMode(PlayMode::RANDOMED);
            _playModeButton->setIcon(QIcon(prefix::normalImages + "随机播放.png"));
            _playModeButton->setToolTip("点击切换到单曲循环");
            StatusManager::getInstance().showMessage("已切换到随机播放模式", 1000);
            break;
        case PlayMode::RANDOMED:
            playManager.setPlayMode(PlayMode::SINGLE_LOOPING);
            _playModeButton->setIcon(QIcon(prefix::normalImages + "单曲循环.png"));
            _playModeButton->setToolTip("点击切换到顺序播放");
            StatusManager::getInstance().showMessage("已切换到单曲循环模式", 1000);
            break;
        case PlayMode::SINGLE_LOOPING:
            playManager.setPlayMode(PlayMode::ORDERED);
            _playModeButton->setIcon(QIcon(prefix::normalImages + "列表播放.png"));
            _playModeButton->setToolTip("点击切换到随机播放");
            StatusManager::getInstance().showMessage("已切换到顺序播放模式", 1000);
            break;
        }
    });

    // 上一首
    connect(_prevButton, &QPushButton::clicked, this, []() {
        StatusManager::getInstance().showMessage("播放上一首", 1000);
        PlayManager::getInstance().prevPlay();
    });

    // 播放/暂停
    connect(_playButton, &QPushButton::clicked, this, [this]() {
        auto& playManager = PlayManager::getInstance();
        auto& statusManager = StatusManager::getInstance();
        switch (playManager.getPlayStatus()) {
        case PlayStatus::STOPPED:
            statusManager.showMessage("请先选则要播放的歌曲");
            break;
        case PlayStatus::PLAYING:
            playManager.pause();
            statusManager.showMessage("已暂停", 1000);
            _playButton->setIcon(QIcon(prefix::normalImages + "播放.png"));
            break;
        case PlayStatus::PAUSED:
            playManager.start();
            statusManager.showMessage("继续播放", 1000);
            _playButton->setIcon(QIcon(prefix::normalImages + "停止.png"));
            break;
        case PlayStatus::ERROR:
            LOG_ERROR() << "播放错误";
            break;
        }
    });

    // 下一首
    connect(_nextButton, &QPushButton::clicked, this, []() {
        StatusManager::getInstance().showMessage("播放下一首", 1000);
        PlayManager::getInstance().nextPlay();
    });

    // 音量按钮
    connect(_volumeButton, &QPushButton::clicked, this, [this]() {
        const QPoint buttonGlobalPos = _volumeButton->mapToGlobal(QPoint(0, 0));
        const QSize sliderSize = _volumeSlider->sizeHint();
        const int x = buttonGlobalPos.x() + (_volumeButton->width() - sliderSize.width()) / 2;
        const int y = buttonGlobalPos.y() - sliderSize.height() - 10;
        _volumeSlider->showAtPosition(QPoint(x, y));
    });

    // 添加本地音乐
    connect(_addToButton, &QPushButton::clicked, this, [this]() {
        QFileDialog fileDialog(this);
        fileDialog.setWindowTitle("添加本地音乐");

        QDir dir{QDir::currentPath()};
        dir.cdUp();
        fileDialog.setDirectory(dir);
        fileDialog.setFileMode(QFileDialog::ExistingFiles);
        fileDialog.setNameFilter("音乐文件(*.mp3 *.wav)");
        if (QDialog::Accepted == fileDialog.exec()) {
            const auto urls = fileDialog.selectedUrls();
            auto& songManager = SongManager::getInstance();
            songManager.append(songManager.getDownloadList(), urls);
            if (const auto currentPage = _mainStackedWidget->currentWidget()) {
                if (const auto downloadPage = qobject_cast<CommonPageWidget*>(currentPage)) {
                    if (downloadPage->getPageName() == "本地下载") {
                        downloadPage->reloadData(songManager.getDownloadList());
                        downloadPage->updateCover();
                    }
                }
            }
        } else {
            LOG_DEBUG() << "用户取消添加";
        }
    });
}

QWidget* MainWindow::createControlWidget(QWidget* parent) {
    const auto controlWidget = new QWidget(parent);
    const auto controlLayout = new QHBoxLayout(controlWidget);
    Sync::clearWidgetMargins(controlWidget);
    _volumeSlider = VolumeSlider::getInstance(this);
    _volumeSlider->setVisible(false);

    // [封面 歌名/歌手]
    _songCover = Create::squarePixmap(controlWidget, "Sympsel.png", 50);

    const auto leftWidget = new QWidget(controlWidget);
    const auto songInfoWidget = new QWidget(leftWidget);
    const auto leftLayout = new QHBoxLayout(leftWidget);

    const auto songInfoLayout = new QVBoxLayout(songInfoWidget);
    _songNameLabel = new MarqueeLabel();
    _songNameLabel->setFixedWidth(150);
    _songNameLabel->setText("歌曲");
    _singerLabel = new MarqueeLabel();
    _singerLabel->setFixedWidth(150);
    _singerLabel->setText("歌手");

    Sync::widgetToLayout(songInfoLayout, {_songNameLabel, _singerLabel});
    Sync::widgetToLayout(leftLayout, {_songCover, songInfoWidget});

    // [随机播放 上一首 暂停/播放 下一首 音量 添加到我喜欢]
    const auto centralWidget = new QWidget(controlWidget);
    const auto centralLayout = new QHBoxLayout(centralWidget);
    _playModeButton = Create::buttonOnlyIcon("列表播放.png", centralWidget);
    _playModeButton->setToolTipDuration(3000);
    _playModeButton->setToolTip("切换到随机播放模式");

    _prevButton = Create::buttonOnlyIcon("上一首.png", centralWidget);
    _playButton = Create::buttonOnlyIcon("播放.png", centralWidget);
    _nextButton = Create::buttonOnlyIcon("下一首.png", centralWidget);
    _volumeButton = Create::buttonOnlyIcon("中等音量.png", centralWidget);
    _volumeSlider->setRelationButton(_volumeButton);
    _addToButton = Create::buttonOnlyIcon("添加.png", centralWidget);
    _addToButton->setToolTip("从本地添加");
    _addToButton->setToolTipDuration(3000);

    Sync::buttonToHLayout(centralLayout, {
                              _playModeButton, _prevButton, _playButton, _nextButton, _volumeButton, _addToButton
                          });

    // [进度]
    const auto rightWidget = new QWidget(controlWidget);
    const auto rightLayout = new QHBoxLayout(rightWidget);
    _processLabel = new QLabel("00:00/00:00");
    _lyricsButton = new QPushButton(QIcon(prefix::normalImages + "词.png"), "");
    _lyricsButton->setFixedSize(QSize(30, 30));
    syncButtonBackground({_lyricsButton});
    rightLayout->addWidget(_processLabel);
    rightLayout->addWidget(_lyricsButton, 0, Qt::AlignCenter);

    controlLayout->addWidget(leftWidget);
    controlLayout->addStretch(1);
    controlLayout->addWidget(centralWidget);
    controlLayout->addStretch(1);
    controlLayout->addWidget(rightWidget);
    Sync::clearLayoutVMargins({controlLayout, leftLayout, centralLayout, rightLayout});
    return controlWidget;
}

void MainWindow::initLyricsWidget() {
    _lyricsWidget = new LyricsWidget(_mainStackedWidget);
    _mainStackedWidget->addWidget(_lyricsWidget);

    connect(_lyricsButton, &QPushButton::clicked, this, [this]() {
        _mainStackedWidget->setCurrentIndex(_navPageCount);
        // todo
    });
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
    if (size != _mapOfButtonsToWidget.size()) {
        LOG_FATAL() << std::format("程序出错：页面数 {} 和 导航按钮数 {} 不匹配",
                                   _mainStackedWidget->count(), _mapOfButtonsToWidget.size());
        exit(EXIT_FAILURE);
    }

    // 保存每个按钮的原始样式表
    std::vector<QString> originalStyleSheets;
    for (const auto& _navigationButton : _mapOfButtonsToWidget | std::views::keys) {
        originalStyleSheets.emplace_back(_navigationButton->styleSheet());
    }

    // 点击按钮切换页面
    for (size_t i = 0; i < size; ++i) {
        connect(_mapOfButtonsToWidget[i].first, &QPushButton::clicked, this, [this, i]() {
            _mainStackedWidget->setCurrentIndex(static_cast<int>(i));
        });
    }

    // 监听页面切换信号
    connect(_mainStackedWidget, &QStackedWidget::currentChanged, this,
            [this, originalStyleSheets](const int index) {
                for (size_t i = 0; i < _mapOfButtonsToWidget.size(); ++i) {
                    const bool selected = (static_cast<int>(i) == index);
                    if (i != _mapOfButtonsToWidget.size())
                        _mapOfButtonsToWidget[i].first->setSelected(selected, originalStyleSheets[i]);
                }

                if (const auto currentPage = _mainStackedWidget->widget(index)) {
                    if (const auto commonPage = qobject_cast<CommonPageWidget*>(currentPage)) {
                        commonPage->reloadData();
                        StatusManager::getInstance().showMessage(
                            std::format("切换到: {}，单击播放，双击打开歌曲详情页", commonPage->getPageName().toStdString()),
                            2000
                        );
                    }
                }
            });

    // 默认选中第一个页面
    if (_mapOfButtonsToWidget.size() != 1) {
        qobject_cast<NavigationButton*>(_mapOfButtonsToWidget[0].first)->setSelected(true, originalStyleSheets[0]);
    }

    return bodyWidget;
}

QWidget* MainWindow::createMainStackedWidget(QWidget* parent) {
    const auto mainWidget = new QWidget(parent);
    const auto bodyRightLayout = new QVBoxLayout(mainWidget);
    bodyRightLayout->setContentsMargins(4, 0, 4, 0);

    _mainStackedWidget = new QStackedWidget(mainWidget);
    // todo 动态扩高
    _mainStackedWidget->setMinimumHeight(420);


    {
        // todo: 替换这段代码
        // 为每个页面创建独立的按钮和布局
        auto tempPage = [](const QString& text, QWidget* stack) -> QWidget* {
            const auto page = new QWidget(stack);
            const auto layout = new QVBoxLayout(page);
            const auto btn = new QPushButton(text, page);
            layout->addWidget(btn, 0, Qt::AlignCenter);
            layout->addStretch(1);
            return page;
        };

        const auto 推荐_页 = new RecommendWidget(_mainStackedWidget);
        const auto 电台_页 = tempPage("电台", _mainStackedWidget);
        const auto 漫游_页 = tempPage("漫游页面", _mainStackedWidget);

        const auto 我喜欢的_页 = new CommonPageWidget(
            "我喜欢的",
            "这里是你爱听的",
            _mainStackedWidget);
        我喜欢的_页->setSpecialCallback([]() {
            StatusManager::getInstance().showMessage(
                std::format("页面 {} 正常加载", "我喜欢的"), 2000
            );
            LOG_INFO() << std::format("页面 {} 正常加载", "我喜欢的");
        });
        auto& songManager = SongManager::getInstance();
        我喜欢的_页->setReloadCallback([&songManager](CommonPageWidget* page) {
            page->reloadData(songManager.getLikedList());
        });
        我喜欢的_页->initData(songManager.getLikedList());
        connect(我喜欢的_页, &CommonPageWidget::songItemDoubleClicked, this,
                [this, 我喜欢的_页](const SongPtr& song) {
                    handleRequestFromListWidgetItem(我喜欢的_页, song);
                });

        const auto 本地下载_页 = new CommonPageWidget(
            "本地下载",
            "这里是你已下载或从本地添加的歌曲",
            _mainStackedWidget);
        本地下载_页->setReloadCallback([&songManager](CommonPageWidget* page) {
            page->reloadData(songManager.getDownloadList());
        });
        本地下载_页->setSpecialCallback([]() {
            StatusManager::getInstance().showMessage(
                std::format("页面 {} 正常加载", "本地下载"), 2000
            );
            LOG_INFO() << std::format("页面 {} 正常加载", "本地下载");
        });
        本地下载_页->initData(songManager.getDownloadList());
        connect(本地下载_页, &CommonPageWidget::songItemDoubleClicked, this,
                [this, 本地下载_页](const SongPtr& song) {
                    handleRequestFromListWidgetItem(本地下载_页, song);
                });

        const auto 最近播放_页 = new CommonPageWidget(
            "最近播放",
            "这里是你曾经听过的",
            _mainStackedWidget);
        最近播放_页->setSpecialCallback([]() {
            StatusManager::getInstance().showMessage(
                std::format("页面 {} 正常加载", "最近播放"), 2000
            );
            LOG_INFO() << std::format("页面 {} 正常加载", "最近播放");
        });
        最近播放_页->setReloadCallback([&songManager](CommonPageWidget* page) {
            page->reloadData(songManager.getHistoryList());
        });
        最近播放_页->initData(songManager.getHistoryList());
        connect(最近播放_页, &CommonPageWidget::songItemDoubleClicked, this,
                [this, 最近播放_页](const SongPtr& song) {
                    handleRequestFromListWidgetItem(最近播放_页, song);
                });

        const std::initializer_list<QWidget*>& pages = {
            推荐_页, 电台_页, 漫游_页, 我喜欢的_页, 本地下载_页, 最近播放_页
        };
        syncCommonWidgetConnect({
            我喜欢的_页, 本地下载_页, 最近播放_页
        });
        Sync::widgetToStackedWidget(_mainStackedWidget, pages);
        syncWidgetToContainer(pages);
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
    syncButtonToContainer(buttons);
    Sync::buttonNoFocus(buttons);

    leftLayout->addStretch(1);
    return leftWidget;
}

void MainWindow::setBorder(const bool enabled) const {
    LOG_DEBUG() << std::format("启用{}边框", enabled ? "调试" : "常规");
    FrameStyleSheet::setBorder(this->centralWidget(), enabled);
}

void MainWindow::handleRequestFromListWidgetItem(CommonPageWidget* commonPageWidget, const SongPtr& song) {
    if (_songInfoPage) {
        _songInfoPage->close();
        _songInfoPage = nullptr;
    }
    SongList* list = commonPageWidget->getSongList();
    if (!list) {
        LOG_ERROR() << "未注册播放列表";
        return;
    }
    _songInfoPage = new SongInfoPage({song, *list});
    connect(_songInfoPage, &QWidget::destroyed, this, [this]() {
        _songInfoPage = nullptr;
    });
    _songInfoPage->setAttribute(Qt::WA_DeleteOnClose);
    _songInfoPage->show();
}


// ============= Sync ==============
void MainWindow::syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) {
    const auto& color = ColorTheme::getInstance();
    const auto& [hover, pressed] = color.getControlButtonColor();
    Sync::buttonBackground(buttons, color.getGlobalBGColor(), hover, pressed);
}

void MainWindow::syncButtonToContainer(
    const std::initializer_list<NavigationButton*>& buttons) {
    // 有一个页面是留给歌词页的
    if (static_cast<int>(buttons.size()) == _navPageCount) {
        int i = 0;
        for (const auto& button : buttons) {
            _mapOfButtonsToWidget[i++].first = button;
        }
    } else {
        LOG_ERROR() << "程序出错，页面数与按钮不匹配";
    }
}

void MainWindow::syncWidgetToContainer(
    const std::initializer_list<QWidget*>& widgets) {
    if (static_cast<int>(widgets.size()) == _navPageCount) {
        int i = 0;
        for (const auto& widget : widgets) {
            _mapOfButtonsToWidget[i++].second = widget;
        }
    }
}

void MainWindow::syncCommonWidgetConnect(const std::initializer_list<CommonPageWidget*>& commonPages) {
    for (const auto& page : commonPages) {
        connect(page, &CommonPageWidget::needUpdate, this, [page]() {
            page->reloadData();
        });
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

// ============== 事件 ===============
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
