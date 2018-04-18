#include "Chapter.hpp"

#include <assert.h>

#include "Chapter1.hpp"

using namespace RowBlast;

Chapter::Chapter(const std::vector<MapLevel>& levels, const std::vector<BlockPathVolume>& blockPaths) :
    mLevels {levels},
    mBlockPaths {blockPaths} {}

Chapter RowBlast::GetChapter(int chapterIndex) {
    switch (chapterIndex) {
        case 1:
            return GetChapter1();
        default:
            assert(!"Unknown chapter!");
    }
}
