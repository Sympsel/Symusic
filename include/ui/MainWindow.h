#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <initializer_list>
#include <QPainterPath>

#include "HeadWidget.h"
#include "LyricsWidget.h"
#include "SongInfoPage.h"
#include "ui/NavigationButton.h"
#include "ui/CommonPageWidget.h"
#include "utils/Sync.hpp"
#include "VolumeSlider.h"


class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr, bool statusBarVisible = true, bool debugBorder = false);

    ~MainWindow() override;

private:
    // UI 组件创建
    QWidget* createBodyWidget(QWidget* parent = nullptr);
    QWidget* createMainStackedWidget(QWidget* parent);
    QWidget* createBodyLeftWidget(QWidget* bodyWidget);
    QWidget* createControlWidget(QWidget* parent = nullptr);
    void initLyricsWidget();

    // 样式设置
    void setupStyles();
    void setBorder(bool enabled = false) const;

    // 信号连接
    void setupConnections();

    // 业务逻辑
    void handleRequestFromHeadButton(const HeadWidget* headWidget);
    void handleRequestFromListWidgetItem(CommonPageWidget* commonPageWidget, const SongPtr& song);
    void handleTransPlayStatus() const;

    // 数据库
    void initSqlLite();

    // 同步辅助方法
    static void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons);
    void syncButtonToContainer(const std::initializer_list<NavigationButton*>& buttons);
    void syncWidgetToContainer(const std::initializer_list<QWidget*>& widgets);
    void syncCommonWidgetConnect(const std::initializer_list<CommonPageWidget*>& commonPages);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    // 窗口拖拽
    QPoint _dragPos;

    // UI 组件
    QStackedWidget* _mainStackedWidget{};
    VolumeSlider* _volumeSlider;
    SongInfoPage* _songInfoPage;
    LyricsWidget* _lyricsWidget;

    // 导航相关
    std::vector<std::pair<NavigationButton*, QWidget*>> _mapOfButtonsToWidget;
    int _navPageCount = 6;

    // 控制区组件
    QPushButton* _playModeButton;
    QPushButton* _prevButton;
    QPushButton* _playButton;
    QPushButton* _nextButton;
    QPushButton* _volumeButton;
    QPushButton* _addToButton;
    QPushButton* _lyricsButton;
    QLabel* _songCover;
    MarqueeLabel* _songNameLabel;
    MarqueeLabel* _singerLabel;
    QLabel* _processLabel;
};
