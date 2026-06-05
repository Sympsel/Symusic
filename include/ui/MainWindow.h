#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QPainter>
#include <initializer_list>
#include <QPainterPath>

#include "HeadWidget.h"
#include "SongInfoPage.h"
#include "ui/NavigationButton.h"
#include "utils/Sync.hpp"


class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr, bool statusBarVisible = true, bool debugBorder = false);

    ~MainWindow() override;

private:
    QWidget* createBodyWidget(QWidget* parent = nullptr);

    QWidget* createMainStackedWidget(QWidget* parent);

    QWidget* createBodyLeftWidget(QWidget* bodyWidget);
    // 显示调试边框，可以显示子控件的边界
    void setBorder(bool enabled = false) const;

    static QWidget* createControlWidget(QWidget* parent = nullptr);

    static void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons);

    void syncButtonToContain(const std::initializer_list<NavigationButton*>& buttons);

    void syncWidgetToContain(const std::initializer_list<QWidget*>& widgets);

    void handleRequestFromHeadButton(const HeadWidget* headWidget);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

private:
    QPoint _dragPos;
    QStackedWidget* _mainStackedWidget;
    std::vector<std::pair<NavigationButton*, QWidget*>> _mapOfNavigationButtonsToWidget;
    SongInfoPage* _songInfoPage;
    int _pageCount = 6;
};
