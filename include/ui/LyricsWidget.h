#pragma once

#include <mpegfile.h>
#include <QFile>
#include <QLabel>
#include <QVBoxLayout>
#include "entity/Common.hpp"
#include "entity/PlayManager.hpp"
#include "entity/SongManager.h"
#include "utils/LyricsParser.hpp"
#include "utils/Sync.hpp"
#include <QGraphicsDropShadowEffect>

class LyricsWidget final : public QWidget {
    Q_OBJECT

private:
    static void setAttribute(QLabel* label) {
        label->setAlignment(Qt::AlignCenter);
        label->setFixedHeight(60);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    static void syncAttributes(const std::initializer_list<QLabel*>& labels) {
        for (const auto& label : labels) {
            label->setAlignment(Qt::AlignCenter);
            label->setFixedHeight(60);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }
    }

    // static std::string getEmbeddedLyrics(const QString& filePath) {
    //     TagLib::MPEG::File file(filePath.toStdString().c_str());
    //     // ID3v2Tag
    //     if (!file.isValid() || !file.ID3v2Tag()) {
    //         return {};
    //     }
    //     auto frames = file.ID3v2Tag()->frameListMap()["USLT"];
    //     if (!frames.isEmpty()) {
    //         if (auto* uslt = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frames.front())) {
    //             return uslt->text().to8Bit(true);
    //         }
    //     }
    //     return "";
    // }

    void setHighlight(QLabel* label, const bool yes) const {
        if (yes) {
            label->setStyleSheet(
                QString(
                    "QLabel {"
                    "   background-color: rgba(%1, 80);"
                    "   color: white;"
                    "   font-size: 18px;"
                    "   font-weight: bold;"
                    "   border-left: 3px solid rgb(%1);"
                    "   padding-left: 10px;"
                    "   border-radius: 4px;"
                    "}"
                ).arg(_hoverColor.isEmpty() ? "40,40,40" : _hoverColor)
            );
            auto* glow = new QGraphicsDropShadowEffect(label);
            glow->setBlurRadius(20);
            glow->setColor(QColor(255, 255, 255, 120));
            glow->setOffset(0, 0);
            label->setGraphicsEffect(glow);
        } else {
            label->setStyleSheet(
                "QLabel {"
                "   background-color: transparent;"
                "   color: rgba(255, 255, 255, 140);"
                "   font-size: 14px;"
                "   border-left: none;"
                "   padding-left: 10px;"
                "}"
            );
            label->setGraphicsEffect(nullptr);
        }
    }

    ssize_t findCurrentLyricIndex(LL currentPosition) const {
        if (_lyricItems.empty()) {
            return -1;
        }
        ssize_t left = 0, right = _lyricItems.size() - 1;
        while (left <= right) {
            ssize_t mid = left + (right - left) / 2;
            if (_lyricItems[mid].pos <= currentPosition) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return right;
    }

    void updateLyricsDisplay() {
        const LL currPos = PlayManager::getInstance().getPosition();
        const auto& currPlay = PlayManager::getInstance().getCurrPlay();

        if (!currPlay) {
            showNoSongMessage();
            return;
        }

        if (_lyricItems.empty()) {
            showNoLyricsMessage(currPlay);
            return;
        }

        const int currentIndex = findCurrentLyricIndex(currPos);

        if (currentIndex == -1) {
            if (_lastIndex != -1) {
                _lastIndex = -1;
                showWaitingMessage();
            }
            return;
        }

        if (currentIndex != _lastIndex) {
            _lastIndex = currentIndex;
            updateLyricsLabels(currentIndex);
        }
    }

    void showNoSongMessage() {
        clearLayout();
        const auto newLayout = new QVBoxLayout(this);
        const auto label = new QLabel("请先播放一首歌曲", this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: white; font-size: 16px;");
        newLayout->addWidget(label);
    }

    void showNoLyricsMessage(const SongPtr& song) {
        clearLayout();

        const auto container = new QWidget(this);
        const auto containerLayout = new QVBoxLayout(container);
        Sync::clearLayoutMargins(containerLayout);
        containerLayout->setSpacing(0);

        const auto mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch(1);
        mainLayout->addWidget(container);
        mainLayout->addStretch(1);
        Sync::clearLayoutMargins(mainLayout);

        const auto label1 = new QLabel("纯音乐 - 无歌词", container);
        const auto label2 = new QLabel(QString("歌手：%1").arg(song->getArtist()), container);
        const auto label3 = new QLabel(QString("专辑：%1").arg(song->getAlbum()), container);
        setHighlight(label1, true);
        setHighlight(label2, false);
        setHighlight(label3, false);
        syncAttributes({label1, label2, label3});
        Sync::widgetToLayout(containerLayout, {label1, label2, label3});

        constexpr int totalHeight = 3 * 60;
        container->setFixedHeight(totalHeight);
    }

    void showWaitingMessage() {
        clearLayout();

        const auto container = new QWidget(this);
        const auto containerLayout = new QVBoxLayout(container);
        Sync::clearLayoutMargins(containerLayout);
        containerLayout->setSpacing(0);

        const auto mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch(1);
        mainLayout->addWidget(container);
        mainLayout->addStretch(1);
        Sync::clearLayoutMargins(mainLayout);

        const auto label = new QLabel("等待歌词...", container);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedHeight(60);
        label->setStyleSheet("color: white; font-size: 14px; background-color: transparent;");
        containerLayout->addWidget(label);

        constexpr int totalHeight = 60;
        container->setFixedHeight(totalHeight);
    }

    void updateLyricsLabels(ssize_t currentIndex) {
        clearLayout();

        const auto container = new QWidget(this);
        const auto containerLayout = new QVBoxLayout(container);
        Sync::clearLayoutMargins(containerLayout);
        containerLayout->setSpacing(0);

        const auto mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch(1);
        mainLayout->addWidget(container);
        mainLayout->addStretch(1);
        Sync::clearLayoutMargins(mainLayout);

        const ssize_t startIndex = std::max(0ll, currentIndex - 3);
        const ssize_t endIndex = std::min(static_cast<ssize_t>(_lyricItems.size()) - 1, currentIndex + 3);

        for (ssize_t i = startIndex; i <= endIndex; ++i) {
            const std::string lyricText = _lyricItems[i].text.empty() ? "..." : _lyricItems[i].text;
            auto* label = new QLabel(QString::fromStdString(lyricText), container);
            setAttribute(label);

            const bool isCenter = (i == currentIndex);
            setHighlight(label, isCenter);

            containerLayout->addWidget(label);
        }

        constexpr int totalHeight = 7 * 60;
        container->setFixedHeight(totalHeight);
    }

    void clearLayout() {
        if (auto* layout = this->layout()) {
            while (layout->count() > 0) {
                if (const auto item = layout->takeAt(0)) {
                    if (auto* widget = item->widget()) {
                        widget->deleteLater();
                    }
                    delete item;
                }
            }
            delete layout;
        }
    }

    // void loadLyricsForSong(const SongPtr& song) {
    //     // 尝试读取内嵌歌词
    //     // 尝试读取同目录下的同名 lrc 文件
    //     std::string embedded = getEmbeddedLyrics(song->getFilePath());
    //     if (!embedded.empty()) {
    //         _lyricItems = LyricsParser::parseString(embedded);
    //         return;
    //     }
    //     QString lrcPath = song->getLrcPathFromName();
    //     if (QFile::exists(lrcPath)) {
    //         _lyricItems = LyricsParser::parse(lrcPath);
    //         return;
    //     }
    //     // 无歌词
    //     _lyricItems.clear();
    // }

    void loadLyricsFile(const QString& lrcPath) {
        _lrcFile = lrcPath;
        _lyricItems = LyricsParser::parse(lrcPath);
    }

public:
    explicit LyricsWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 监听歌曲播放事件，加载歌词文件
        connect(&PlayManager::getInstance(), &PlayManager::songPlayed, this, [this]() {
            const auto& currPlay = PlayManager::getInstance().getCurrPlay();
            if (currPlay) {
                // 1. 尝试从 MP3 内嵌歌词读取
                _lyricItems = LyricsParser::extractLyricsFromMP3(currPlay->getUrl().toLocalFile());

                // 2. 内嵌歌词为空，尝试同名 .lrc 文件
                if (_lyricItems.empty()) {
                    const QString lrcPath = currPlay->getLrcPath();
                    LOG_DEBUG() << lrcPath;
                    if (QFile::exists(lrcPath)) {
                        _lyricItems = LyricsParser::parse(lrcPath);
                    }
                }
                // const QString lrcPath = currPlay->getLrcPath();
                // loadLyricsFile(lrcPath);
                // loadLyricsFile("D:/shared/Code/qtCode/SymMusic_2/utils/1.lrc");
            }
            _lastIndex = -1;
            updateLyricsDisplay();
        });

        // 监听播放进度变化，动态更新歌词
        connect(&PlayManager::getInstance(), &PlayManager::positionChanged, this, [this]() {
            updateLyricsDisplay();
        });
    }

private:
    static constexpr int _count = 7;
    QVector<QLabel*> _labels;
    QString _hoverColor = ColorTheme::getInstance().getPlayItemColor().hover;
    QString _lrcFile;
    std::vector<LyricItem> _lyricItems;
    ssize_t _lastIndex = -1;
};
