#include "Chapter1.hpp"

using namespace BlocksGame;

namespace {
    const std::vector<Chapter::MapLevel> levels {
        Chapter::MapLevel {
            .mLevelIndex = 1,
            .mPosition = {-24.0f, -15.0f, 10.0f}
        },
        Chapter::MapLevel {
            .mLevelIndex = 2,
            .mPosition = {-20.0f, -15.3f, 10.0f}
        }
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
