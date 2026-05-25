#pragma once

#include <QMainWindow>
#include <QStatusBar>
#include <QIcon>
#include <QPushButton>
#include <QPushButton>
#include <QStackedWidget>
#include <QProgressBar>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <initializer_list>
#include <QPainterPath>

#include "Color.hpp"
#include "Sync.hpp"


class MainWindow final : public QMainWindow {
    Q_OBJECT
   public:
    explicit MainWindow(QWidget *parent = nullptr, bool statusBarVisible = true, bool debugBorder = false);
    ~MainWindow() override;
private:
    QWidget* createControlWidget(QWidget* parent = nullptr) const;
    QWidget* createBodyWidget(QWidget* parent = nullptr) const;
    QWidget* createBodyRightWidget(QWidget* parent) const;
    QWidget* createFunctionWidget(QWidget* parent = nullptr);
    void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons) const;
    // 显示调试边框，可以显示子控件的边界
    void setBorder(bool enabled);

    static QWidget* createBodyLeftWidget(QWidget* bodyWidget);
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    Color _color;
    QPoint _dragPos;
};
