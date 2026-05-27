#pragma once

#include <QMainWindow>
#include <QStatusBar>
#include <QListWidget>
#include <QLineEdit>
#include <QPainter>
#include <initializer_list>
#include <QPainterPath>

#include "HeadWidget.h"
#include "NavigationButton.h"
#include "Sync.hpp"


class MainWindow final : public QMainWindow {
    Q_OBJECT
   public:
    explicit MainWindow(QWidget *parent = nullptr, bool statusBarVisible = true, bool debugBorder = false);
    ~MainWindow() override;
private:
    QWidget* createBodyWidget(QWidget* parent = nullptr);
    QWidget* createMainStackedWidget(QWidget* parent);
    // QWidget* createFunctionWidget(QWidget* parent = nullptr);
    QWidget* createBodyLeftWidget(QWidget* bodyWidget);
    // 显示调试边框，可以显示子控件的边界
    void setBorder(bool enabled) const;

    static QWidget* createControlWidget(QWidget* parent = nullptr);
    static void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons);
    static void syncButtonContain(std::vector<NavigationButton*>& navigationButtonList, const std::initializer_list<NavigationButton*>& buttons);
    void handleRequestFromHeadButton(const HeadWidget* headWidget);
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    QPoint _dragPos;
    QStackedWidget* _mainStackedWidget;
    std::vector<NavigationButton*> _navigationButtons;
};
