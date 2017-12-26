#include "Chapter.hpp"

#include <assert.h>

#include "Chapter1.hpp"

using namespace BlocksGame;

Chapter::Chapter(const std::vector<MapLevel>& levels, const std::vector<CubePathVolume>& cubePaths) :
    mLevels {levels},
    mCubePaths {cubePaths} {}

Chapter BlocksGame::GetChapter(int chapterIndex) {
    switch (chapterIndex) {
        case 1:
            return GetChapter1();
        default:
            assert(!"Unknown chapter!");
    }
}
