#include "entity/DatabaseManager.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QWidget>
#include "utils/Log.hpp"
#include "entity/SongManager.h"

void DatabaseManager::initDatabase(QWidget* parent) {
    _parent = parent;
    _sqlLiteDB.setDatabaseName("Symusic.db");
    if (!_sqlLiteDB.open()) {
        QMessageBox::critical(_parent, "Symusic", "数据库连接失败");
        LOG_ERROR() << _sqlLiteDB.lastError().text();
        return;
    }
    LOG_DEBUG() << "数据库连接成功";

    QString sql = R"(create table if not exists MusicInfo(
            id TEXT primary key,
            UUID TEXT unique,
            name TEXT,
            artist TEXT not null default '',
            album TEXT not null default '',
            url TEXT not null,
            cover_url TEXT default '',
            duration INTEGER default 0,
            play_count INTEGER default 0,
            tags_bitmap INTEGER default 0,
            belonging_bitmap INTEGER default 0))";

    QSqlQuery query;
    if (!query.exec(sql)) {
        QMessageBox::critical(_parent, "Symusic", "初始化错误");
        LOG_ERROR() << _sqlLiteDB.lastError().text();
        return;
    }
    LOG_DEBUG() << "表初始化成功";
}

void DatabaseManager::loadData(SongManager& songManager) const {
    QSqlQuery query{_sqlLiteDB};
    if (!query.exec(
        "select UUID, name, artist, album, url, cover_url, duration, play_count, tags_bitmap, belonging_bitmap from MusicInfo")) {
        LOG_ERROR() << "加载歌曲数据失败: " << query.lastError().text();
        return;
    }
    while (query.next()) {
        const auto song = Song::createFromDatabase(
            query.value(0).toString(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            QUrl(query.value(4).toString()),
            query.value(5).toString(),
            query.value(6).toLongLong(),
            query.value(7).toInt(),
            query.value(8).toInt(),
            query.value(9).toInt()
        );
        song->ensureCover();

        const int belongStatus = song->getBelongStatus();
        if (belongStatus & static_cast<int>(Song::ExistIn::RECOMMEND_LIST))
            songManager._recommendList.push_back(song);
        if (belongStatus & static_cast<int>(Song::ExistIn::YOU_MAY_LIKE_LIST))
            songManager._youMayLikeList.push_back(song);
        if (belongStatus & static_cast<int>(Song::ExistIn::LIKED_LIST))
            songManager._likedList.push_back(song);
        if (belongStatus & static_cast<int>(Song::ExistIn::DOWNLOAD_LIST))
            songManager._downloadList.push_back(song);
        if (belongStatus & static_cast<int>(Song::ExistIn::HISTORY_LIST))
            songManager._historyList.push_back(song);
        LOG_DEBUG() << "从数据库加载歌曲数据完成";
    }
}

void DatabaseManager::saveData() {
    if (!_sqlLiteDB.isOpen()) {
        LOG_WARN() << "数据库未连接，跳过保存";
        return;
    }
    LOG_DEBUG() << "saveData: 开始收集歌曲数据";
    SongList allMusic = SongManager::getInstance().packUp();
    LOG_DEBUG() << std::format("saveData: 共 {} 首歌曲需要保存", allMusic.size());
    int count = 0;
    for (auto& song : allMusic) {
        if (!song) {
            LOG_WARN() << "saveData: 跳过空歌曲指针";
            continue;
        }
        if (!insertSong(song)) {
            LOG_WARN() << "保存歌曲失败：" << song->getName();
        }
        count++;
    }
    LOG_DEBUG() << std::format("saveData: 保存完成，共处理 {} 首", count);
    _sqlLiteDB.close();
}

bool DatabaseManager::isContainsSong(const QString& uuid) const {
    QSqlQuery query{_sqlLiteDB};
    if (!query.prepare("select 1 from MusicInfo where UUID = :uuid")) {
        LOG_ERROR() << "prepare 失败: " << query.lastError().text();
        return false;
    }
    query.bindValue(":uuid", uuid);
    if (!query.exec()) {
        LOG_ERROR() << "exec 失败: " << query.lastError().text();
        return false;
    }
    return query.next();
}

bool DatabaseManager::insertSong(const SongPtr& song) const {
    QSqlQuery query{_sqlLiteDB};
    query.prepare(
        "insert or replace into MusicInfo "
        "(UUID, name, artist, album, url, cover_url, duration, play_count, tags_bitmap, belonging_bitmap) "
        "values (:uuid, :name, :artist, :album, :url, :cover_url, :duration, :play_count, :tags, :belonging)");
    query.bindValue(":uuid", song->getId());
    query.bindValue(":name", song->getName());
    query.bindValue(":artist", song->getArtist());
    query.bindValue(":album", song->getAlbum());
    query.bindValue(":url", song->getUrl().toString());
    query.bindValue(":cover_url", song->getCoverPath());
    query.bindValue(":duration", song->getDuration());
    query.bindValue(":play_count", song->getPlayCount());
    query.bindValue(":tags", song->getTagsFlag());
    query.bindValue(":belonging", song->getBelongStatus());
    if (!query.exec()) {
        LOG_ERROR() << "保存歌曲失败: " << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::removeSong(const QString& uuid) const {
    QSqlQuery query{_sqlLiteDB};
    query.prepare("delete from MusicInfo where UUID = ?");
    query.bindValue(1, uuid);
    if (!query.exec()) {
        LOG_ERROR() << "删除歌曲失败：" << query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        LOG_WARN() << "删除歌曲失败：歌曲不存在";
        return false;
    }
    return true;
}
