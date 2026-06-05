#pragma once

#include <QResizeEvent>
#include <QTimer>

#include "utils/Log.hpp"
#include "NavigationWidget.h"
#include "PlaylistItem.h"

class RecommendWidget : public QWidget {
    Q_OBJECT

public:
    using Items = std::vector<PlaylistItem*>;

    struct Alist {
        QPushButton* leftButton;
        QPushButton* rightButton;

        QWidget* widget;
        Items list;
        int begin = 0;
    };

private:
    static void syncButtonStyle(const std::initializer_list<QPushButton*>& buttons, int width, int height);

    void initPlaylist();

    [[nodiscard]] Items displayList(const QString& name) const;

    QWidget* createPlateWidget(const QString& name);

    void updateWidgetLayout(const QString& name);


    void updateRowSize();

    void updateButtonVisibility(const QString& name) const;

protected:
    // resize 事件处理
    void resizeEvent(QResizeEvent* event) override;

public:
    explicit RecommendWidget(QWidget* parent);

    void setRowSize(const int rowSize) {
        _rowSize = rowSize;
    }

    ~RecommendWidget() override;;

 private:
    std::unordered_map<QString, Alist> _contain;
    int _rowSize = 4;
    // 防抖定时器
    QTimer* _resizeTimer = nullptr;
};
