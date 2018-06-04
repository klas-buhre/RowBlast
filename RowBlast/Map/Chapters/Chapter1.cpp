#include "Chapter1.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapLevel> levels {
        MapLevel {.mLevelIndex = 1, .mPosition = {-24.0f, -3.0f, 9.5f}},
        MapLevel {.mLevelIndex = 2, .mPosition = {-20.0f, -3.3f, 9.5f}},
        MapLevel {.mLevelIndex = 3, .mPosition = {-16.0f, -3.6f, 8.5f}},
        MapLevel {.mLevelIndex = 4, .mPosition = {-12.0f, -4.0f, 8.5f}},
        MapLevel {.mLevelIndex = 5, .mPosition = {-10.0f, -4.7f, 10.0f}},
        MapLevel {.mLevelIndex = 6, .mPosition = {-6.2f, -5.4f, 10.0f}},
        MapLevel {.mLevelIndex = 7, .mPosition = {-3.2f, -4.5, 8.5f}},
        MapLevel {.mLevelIndex = 8, .mPosition = {-3.0f, -1.0f, 8.5f}},
        MapLevel {.mLevelIndex = 9, .mPosition = {-5.5f, 1.0f, 9.5f}},
        MapLevel {.mLevelIndex = 10, .mPosition = {-9.2f, 1.5f, 9.5f}},
        MapLevel {.mLevelIndex = 11, .mPosition = {-13.0f, 1.0f, 9.5f}},
        MapLevel {.mLevelIndex = 12, .mPosition = {-17.0f, 1.0f, 9.5f}},
        MapLevel {.mLevelIndex = 13, .mPosition = {-20.f, 0.0f, 9.5f}},
        MapLevel {.mLevelIndex = 14, .mPosition = {-22.5f, 2.0f, 9.5f}},
        MapLevel {.mLevelIndex = 15, .mPosition = {-21.0f, 5.0f, 9.5f}},
        MapLevel {.mLevelIndex = 16, .mPosition = {-17.5f, 4.5f, 9.5f}},
        MapLevel {.mLevelIndex = 17, .mPosition = {-13.5f, 4.5f, 9.5f}},
        MapLevel {.mLevelIndex = 18, .mPosition = {-10.0f, 5.0f, 9.5f}},
        MapLevel {.mLevelIndex = 19, .mPosition = {-6.5f, 5.0f, 9.5}},
        MapLevel {.mLevelIndex = 20, .mPosition = {-3.5f, 4.5f, 9.5f}},
        MapLevel {.mLevelIndex = 21, .mPosition = {-1.0f, 2.0f, 10.0f}},
        MapLevel {.mLevelIndex = 22, .mPosition = {1.0f, -1.0f, 10.0f}},
        MapLevel {.mLevelIndex = 23, .mPosition = {3.0f, -3.0f, 10.0f}},
        MapLevel {.mLevelIndex = 24, .mPosition = {7.0f, -3.5, 10.0f}},
        MapLevel {.mLevelIndex = 25, .mPosition = {11.0f, -3.5f, 10.0f}},
        MapLevel {.mLevelIndex = 26, .mPosition = {15.5f, -3.5f, 10.0f}},
        MapLevel {.mLevelIndex = 27, .mPosition = {19.5f, -3.5f, 10.0f}},
        MapLevel {.mLevelIndex = 28, .mPosition = {22.0f, -2.0f, 10.0f}},
        MapLevel {.mLevelIndex = 29, .mPosition = {23.5f, 1.0f, 10.0f}},
        MapLevel {.mLevelIndex = 30, .mPosition = {22.0f, 4.0f, 10.0f}},
        MapLevel {.mLevelIndex = 31, .mPosition = {19.5f, 5.0f, 10.0f}},
        MapLevel {.mLevelIndex = 32, .mPosition = {17.0f, 4.0f, 10.0f}},
        MapLevel {.mLevelIndex = 33, .mPosition = {15.5f, 1.0f, 10.0f}},
        MapLevel {.mLevelIndex = 34, .mPosition = {12.0f, 0.0f, 10.0f}},
        MapLevel {.mLevelIndex = 35, .mPosition = {8.0f, 0.0f, 10.0f}},
        MapLevel {.mLevelIndex = 36, .mPosition = {4.0f, 0.0f, 10.0f}},
        MapLevel {.mLevelIndex = 37, .mPosition = {2.0f, 2.0f, 10.0f}},
        MapLevel {.mLevelIndex = 38, .mPosition = {4.0f, 4.0f, 10.0f}}
    };
    
    const std::vector<BlockPathVolume> blockPaths {
        BlockPathVolume {.mPosition = {-22.0f, 0.5f, -15.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-22.0f, -5.0f, 12.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-16.0f, -5.0f, 12.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-7.0f, -5.0f, 8.0f}, .mSize = {0.0f, 0.0f, 0.0f}, .mIsLPiece = true},
        BlockPathVolume {.mPosition = {-1.0f, 5.0f, 8.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {0.0f, -4.0f, 13.0f}, .mSize = {0.0f, 0.0f, 0.0f}, .mIsLPiece = true},
        BlockPathVolume {.mPosition = {5.0f, -5.0f, 11.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-9.0f, 0.5f, 5.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-13.0f, 5.0f, -15.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {3.0f, 0.5f, 8.0f}, .mSize = {0.0f, 0.0f, 0.0f}}
    };
}

Chapter RowBlast::GetChapter1() {
    return Chapter {levels, blockPaths};
}
