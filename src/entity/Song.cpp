#include "entity/Song.h"

Song::TagList Song::getTags(const int flag) {
    TagList tags;
    if (flag & VIP) {
        tags.emplace_back("VIP");
    }
    if (flag & SQ) {
        tags.emplace_back("SQ");
    }
    return tags;
}
