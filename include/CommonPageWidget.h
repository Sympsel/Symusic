#pragma once

#include "ListItem.h"
#include <QListWidget>
#include <QWidget>
#include <Color.hpp>

#include "Log.hpp"
#include "SongManager.h"
#include "Sync.hpp"

class CommonPageWidget : public QWidget {
    Q_OBJECT

private:
    [[nodiscard]] QWidget* createHeadWidget(const QString& coverPath, const QString& description) const;

    QWidget* createMiddleWidget();

public:
    explicit CommonPageWidget(QString pageName, const QString& coverPath, const QString& description = "",
                              QWidget* parent = nullptr);

    void initData(const SongManager::SongList& songList) const;

    void reloadData(const SongManager::SongList& songList) const;

private:
    QString _pageName;
    QPushButton* _playAllButton;
    QListWidget* _playlist;
};
