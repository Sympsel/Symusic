#pragma once

#include <QResizeEvent>
#include <QTimer>

#include "ListWidget.h"
#include "Log.hpp"
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

    // resize 事件处理
    void resizeEvent(QResizeEvent* event) override;

    void updateRowSize();

    void updateButtonVisibility(const QString& name);

public:
    explicit RecommendWidget(QWidget* parent);

    void setRowSize(const int rowSize) {
        _rowSize = rowSize;
    }

    ~RecommendWidget() override;;

    [[nodiscard]] int getRowSize() const {
        for (const auto& [key, value] : _contain) {
            // 取其中一个界面用于计算宽度
            return value.widget->width();
        }
        return 0;
    }

private:
    std::unordered_map<QString, Alist> _contain;
    int _rowSize = 4;
    // 防抖定时器
    QTimer* _resizeTimer = nullptr;
};
