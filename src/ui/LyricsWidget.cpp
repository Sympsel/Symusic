module symusic.ui.lyrics_widget;


void LyricsWidget::updateLyricsDisplay() {
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
        showWaitingMessage();
        return;
    }

    updateLyricsLabels(currentIndex);
}
