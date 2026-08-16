module;

#include <QSet>
#include <QString>

export module symusic.common;
export import symusic.common.color;
export import symusic.common.path;
export import symusic.utils.log;

export using LL = long long;

export using SongSupportedList = QSet<QString>;

export struct SupportSongType {
private:
    /**
     *
     * audio/mpeg - MP3
     * audio/flac - 无损
     * audio/wav
     */
    explicit SupportSongType() : _supportedList{"audio/mpeg", "audio/flac", "audio/wav"} {
    }

public:
    SupportSongType(const SupportSongType&) = delete;
    SupportSongType(SupportSongType&&) = delete;
    SupportSongType& operator=(const SupportSongType&) = delete;
    SupportSongType& operator=(SupportSongType&&) = delete;

    [[nodiscard]] static SupportSongType& getInstance() {
        static SupportSongType instance;
        return instance;
    }

    [[nodiscard]] const SongSupportedList& getList() {
        return _supportedList;
    }

private:
    SongSupportedList _supportedList;
};



export enum class PlayMode {
    ORDERED,
    RANDOMED,
    SINGLE_LOOPING
};

