#include "World2.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapPlace> places {
        MapPlace {Portal {1, 40, {-24.0f, -3.0f, 9.5f}}},
        MapPlace {MapLevel {40, {-20.0f, -3.3f, 9.5f}}},
        MapPlace {MapLevel {41, {-16.0f, -3.6f, 8.5f}}},
        MapPlace {MapLevel {42, {-12.0f, -4.0f, 8.5f}}},
        MapPlace {MapLevel {43, {-10.0f, -4.7f, 10.0f}}},
        MapPlace {MapLevel {44, {-6.2f, -5.4f, 10.0f}}},
        MapPlace {MapLevel {45, {-3.2f, -4.5, 8.5f}}},
        MapPlace {MapLevel {46, {-3.0f, -1.0f, 8.5f}}},
        MapPlace {Portal {3, 47, {-5.5f, 1.0f, 9.5f}}}
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
    
    const std::vector<CloudPathVolume> cloudPaths {
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
