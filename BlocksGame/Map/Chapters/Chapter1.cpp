#include "Chapter1.hpp"

using namespace BlocksGame;

namespace {
    const std::vector<Chapter::MapLevel> levels {
        Chapter::MapLevel {.mLevelIndex = 1, .mPosition = {-24.0f, -15.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 2, .mPosition = {-20.0f, -15.3f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 3, .mPosition = {-16.0f, -15.6f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 4, .mPosition = {-12.0f, -16.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 5, .mPosition = {-8.0f, -16.7f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 6, .mPosition = {-4.2f, -17.7f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 7, .mPosition = {-0.2f, -18.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 8, .mPosition = {3.8f, -18.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 9, .mPosition = {7.8f, -18.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 10, .mPosition = {11.8f, -18.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 11, .mPosition = {15.8f, -18.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 12, .mPosition = {19.8f, -18.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 13, .mPosition = {22.5f, -16.5f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 14, .mPosition = {23.0f, -13.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 15, .mPosition = {22.0f, -10.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 16, .mPosition = {19.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 17, .mPosition = {15.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 18, .mPosition = {11.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 19, .mPosition = {7.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 20, .mPosition = {3.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 21, .mPosition = {-1.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 22, .mPosition = {-5.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 23, .mPosition = {-9.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 24, .mPosition = {-13.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 25, .mPosition = {-17.0f, -9.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 26, .mPosition = {-20.5f, -8.0f, 10.0f}},
        Chapter::MapLevel {.mLevelIndex = 27, .mPosition = {-23.0f, -6.0f, 10.0f}},
    };
    
    const std::vector<CubePathVolume> cubePaths {
        CubePathVolume {
            .mPosition = {-22.0f, -16.5f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {-17.0f, -17.0f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {-12.0f, -17.5f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {-7.0f, -18.0f, 10.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        }
    };
}

Chapter BlocksGame::GetChapter1() {
    return Chapter {levels, cubePaths};
}
