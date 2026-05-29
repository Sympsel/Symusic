#pragma once

#include "ListWidget.h"
#include "Log.hpp"
#include "PlaylistItem.h"

class RecommendWidget : public QWidget {
    using Items = std::vector<PlaylistItem*>;

private:
    static void syncButtonStyle(const std::initializer_list<QPushButton*>& buttons, int width, int height);

    void initPlaylist();

    [[nodiscard]] Items displayList(const Items& items, int begin) const;

    static QWidget* createWidgetItem(const QString& name, const Items& items);


public:
    explicit RecommendWidget(QWidget* parent);

    void setRowSize(const int rowSize) {
        _rowSize = rowSize;
    }

    ~RecommendWidget() override;;

private:
    Items _todayRecommendList;
    int _todyRecommendBegin;
    Items _youMayLikeList;
    int _youMayLikeBegin;
    int _rowSize = 4;
};
