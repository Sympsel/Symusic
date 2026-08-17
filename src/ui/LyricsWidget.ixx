module;

#include <QLabel>
#include <QVBoxLayout>
#include <QString>

export module symusic.ui.lyrics_widget;
import symusic.utils.lyrics_parser;
import symusic.utils.sync;
import symusic.entity.play_manager;
import symusic.entity.song_manager;
import symusic.component.lyrics_item;

export class LyricsWidget final : public QWidget {
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

    void setHighlight(QLabel* label, const bool yes) const {
        if (yes) {
            label->setStyleSheet(
                QString(
                    "QLabel {"
                    "   background-color: rgb(%1);"
                    "   color: white;"
                    "   font-size: 16px;"
                    "}"
                ).arg(_hoverColor.isEmpty() ? "40,40,40" : _hoverColor)
            );
        } else {
            label->setStyleSheet(
                "QLabel {"
                "   background-color: transparent;"
                "   color: white;"
                "   font-size: 14px;"
                "}"
            );
        }
    }

    int findCurrentLyricIndex(LL currentPosition) const {
        if (_lyricItems.empty()) {
            return -1;
        }

        int index = -1;
        for (size_t i = 0; i < _lyricItems.size(); ++i) {
            if (_lyricItems[i].pos <= currentPosition) {
                index = static_cast<int>(i);
            } else {
                break;
            }
        }
        return index;
    }

    void updateLyricsDisplay();

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

    void updateLyricsLabels(int currentIndex) {
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

        const int startIndex = std::max(0, currentIndex - 3);
        const int endIndex = std::min(static_cast<int>(_lyricItems.size()) - 1, currentIndex + 3);

        for (int i = startIndex; i <= endIndex; ++i) {
            const QString lyricText = _lyricItems[i].text.empty() ? "..." : QString(_lyricItems[i].text.c_str());
            auto* label = new QLabel(lyricText, container);
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
                const QString lrcPath = currPlay->getLrcPath();
                loadLyricsFile(lrcPath);
            }
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
};
