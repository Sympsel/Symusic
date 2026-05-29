#pragma once

#include <QResizeEvent>

#include "ListWidget.h"
#include "Log.hpp"
#include "PlaylistItem.h"

class RecommendWidget : public QWidget {
    Q_OBJECT

public:
    using Items = std::vector<PlaylistItem*>;

    struct Alist {
        QWidget* widget;
        Items list;
        int begin = 0;
    };

private:
    static void syncButtonStyle(const std::initializer_list<QPushButton*>& buttons, int width, int height);

    void initPlaylist();

    [[nodiscard]] Items displayList(const QString& name) const;

    QWidget* createWidgetItem(const QString& name);

public:
    explicit RecommendWidget(QWidget* parent);

    void setRowSize(const int rowSize) {
        _rowSize = rowSize;
    }

    ~RecommendWidget() override;;

    bool eventFilter(QObject* watched, QEvent* event) override;


    [[nodiscard]] int getRowSize() const {
        for (const auto& [key, value] : _contain) {
            // 取其中一个界面用于计算宽度
            return value.widget->width();
        }
        return 0;
    }

private:
    std::unordered_map<QString, Alist> _contain;
    // std::vector<Alist> _contain;
    // int _todyRecommendBegin;
    // int _youMayLikeBegin;
    int _rowSize = 4;
};
