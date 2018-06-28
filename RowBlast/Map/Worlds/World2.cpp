#include "World2.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapPlace> places {
        MapPlace {Portal {1, 19, {-24.0f, 3.0f, 10.0f}}},
        MapPlace {MapLevel {19, {-21.5f, 1.0f, 10.0f}}},
        MapPlace {MapLevel {20, {-19.5f, -1.5f, 10.0f}}},
        MapPlace {MapLevel {21, {-17.0f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {22, {-13.5f, -4.5f, 10.0f}}},
        MapPlace {MapLevel {23, {-10.0f, -4.0f, 10.0f}}},
        MapPlace {MapLevel {24, {-8.5f, -2.0f, 10.0f}}},
        MapPlace {MapLevel {25, {-10.0f, 0.0f, 10.0f}}},
        MapPlace {MapLevel {26, {-13.5f, 0.5f, 10.0f}}},
        MapPlace {MapLevel {27, {-15.0f, 2.5f, 10.0f}}},
        MapPlace {MapLevel {28, {-13.0f, 4.0f, 10.0f}}},
        MapPlace {MapLevel {29, {-9.5f, 3.5f, 10.0f}}},
        MapPlace {MapLevel {30, {-6.0f, 2.5f, 10.0f}}},
        MapPlace {MapLevel {31, {-3.5f, 0.5f, 10.0f}}},
        MapPlace {MapLevel {32, {-1.5f, -2.0f, 10.0f}}},
        MapPlace {MapLevel {33, {1.5f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {34, {4.5f, -4.0f, 10.0f}}},
        MapPlace {MapLevel {35, {8.0f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {36, {11.0f, -2.5f, 10.0f}}},
        MapPlace {MapLevel {37, {14.0f, -1.0f, 10.0f}}},
        MapPlace {MapLevel {38, {16.5f, 1.0f, 10.0f}}},
        MapPlace {MapLevel {39, {19.5f, 1.5f, 10.0f}}},
        MapPlace {Portal {3, 40, {23.0f, 1.5f, 10.0f}}}
    };

    const std::vector<BlockPathVolume> blockPaths {
        BlockPathVolume {.mPosition = {-22.0f, 0.5f, -15.0f}, .mSize = {0.0f, 0.0f, 0.0f}, .mPieceType = FloatingPieceType::ShortI},
        BlockPathVolume {.mPosition = {-22.0f, -5.0f, 12.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-16.0f, -5.0f, 12.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-7.0f, -5.0f, 8.0f}, .mSize = {0.0f, 0.0f, 0.0f}, .mPieceType = FloatingPieceType::L},
        BlockPathVolume {.mPosition = {-1.0f, 5.0f, 8.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {0.0f, -4.0f, 13.0f}, .mSize = {0.0f, 0.0f, 0.0f}, .mPieceType = FloatingPieceType::L},
        BlockPathVolume {.mPosition = {5.0f, -5.0f, 11.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-9.0f, 0.5f, 5.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {-13.0f, 5.0f, -15.0f}, .mSize = {0.0f, 0.0f, 0.0f}},
        BlockPathVolume {.mPosition = {3.0f, 0.5f, 8.0f}, .mSize = {0.0f, 0.0f, 0.0f}}
    };
    
    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {
            .mPosition = {0.0f, -4.55f, 14.9f},
            .mSize = {70.0f, 0.0f, 0.0f},
            .mCloudSize = {7.0f, 7.0f},
            .mNumClouds = 2,
            .mVelocity = 0.75f
        },
        CloudPathVolume {
            .mPosition = {0.0f, 4.8f, 14.9f},
            .mSize = {70.0f, 0.0f, 0.0f},
            .mCloudSize = {10.0f, 10.0f},
            .mNumClouds = 2,
            .mVelocity = 0.75f
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {120.0f, 0.0f, 0.0f},
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -60.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -30.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 40.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f}
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -100.0f},
            .mSize = {250.0f, 180.0f, 20.0f},
            .mCloudSize = {60.0f, 60.0f},
            .mCloudSizeRandPart = 60.0f,
            .mNumClouds = 10,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {400.0f, 320.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -300.0f},
            .mSize = {510.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -400.0f},
            .mSize = {510.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        }
    };
#if 0
    const std::vector<HazeLayer> hazeLayers {
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -700.0f},
            .mSize = {1500.0f, 1050.0f},
            .mUpperColor = {0.4f, 0.2f, 0.4f, 1.0f},
            .mLowerColor = {1.0f, 0.4f, 0.5f, 1.0f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 500.0f},
            .mUpperColor = {0.4f, 0.2f, 0.4f, 1.0f},
            .mLowerColor = {1.0f, 0.4f, 0.5f, 1.0f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -250.0f},
            .mSize = {380.0f, 380.0f},
            .mUpperColor = {0.4f, 0.2f, 0.4f, 1.0f},
            .mLowerColor = {1.0f, 0.4f, 0.5f, 1.0f}
        }
    };
#endif

    const std::vector<HazeLayer> hazeLayers {
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -700.0f},
            .mSize = {1500.0f, 1050.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 1.0f},
            .mLowerColor = {0.55f, 0.89f, 1.0f, 1.0f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 500.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.55f, 0.89f, 1.0f, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -250.0f},
            .mSize = {380.0f, 380.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.55f, 0.89f, 1.0f, 0.35f}
        }
    };
}

World RowBlast::GetWorld2() {
    return World {places, blockPaths, cloudPaths, hazeLayers};
}
