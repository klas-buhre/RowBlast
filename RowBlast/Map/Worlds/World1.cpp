#include "World1.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapPlace> places {
        MapPlace {MapLevel {1, {-24.0f, -3.0f, 9.5f}}},
        MapPlace {MapLevel {2, {-20.0f, -3.3f, 9.5f}}},
        MapPlace {MapLevel {3, {-16.0f, -3.6f, 8.5f}}},
        MapPlace {MapLevel {4, {-12.0f, -4.0f, 8.5f}}},
        MapPlace {MapLevel {5, {-10.0f, -4.7f, 10.0f}}},
        MapPlace {MapLevel {6, {-6.2f, -5.4f, 10.0f}}},
        MapPlace {MapLevel {7, {-3.2f, -4.5, 8.5f}}},
        MapPlace {MapLevel {8, {-3.0f, -1.0f, 8.5f}}},
        MapPlace {MapLevel {9, {-5.5f, 1.0f, 9.5f}}},
        MapPlace {MapLevel {10, {-9.2f, 1.5f, 9.5f}}},
        MapPlace {MapLevel {11, {-13.0f, 1.0f, 9.5f}}},
        MapPlace {MapLevel {12, {-17.0f, 1.0f, 9.5f}}},
        MapPlace {MapLevel {13, {-20.f, 0.0f, 9.5f}}},
        MapPlace {MapLevel {14, {-22.5f, 2.0f, 9.5f}}},
        MapPlace {MapLevel {15, {-21.0f, 5.0f, 9.5f}}},
        MapPlace {MapLevel {16, {-17.5f, 4.5f, 9.5f}}},
        MapPlace {MapLevel {17, {-13.5f, 4.5f, 9.5f}}},
        MapPlace {MapLevel {18, {-10.0f, 5.0f, 9.5f}}},
        MapPlace {MapLevel {19, {-6.5f, 5.0f, 9.5}}},
        MapPlace {MapLevel {20, {-3.5f, 4.5f, 9.5f}}},
        MapPlace {MapLevel {21, {-1.0f, 2.0f, 10.0f}}},
        MapPlace {MapLevel {22, {1.0f, -1.0f, 10.0f}}},
        MapPlace {MapLevel {23, {3.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {24, {7.0f, -3.5, 10.0f}}},
        MapPlace {MapLevel {25, {11.0f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {26, {15.5f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {27, {19.5f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {28, {22.0f, -2.0f, 10.0f}}},
        MapPlace {MapLevel {29, {23.5f, 1.0f, 10.0f}}},
        MapPlace {MapLevel {30, {22.0f, 4.0f, 10.0f}}},
        MapPlace {MapLevel {31, {19.5f, 5.0f, 10.0f}}},
        MapPlace {MapLevel {32, {17.0f, 4.0f, 10.0f}}},
        MapPlace {MapLevel {33, {15.5f, 1.0f, 10.0f}}},
        MapPlace {MapLevel {34, {12.0f, 0.0f, 10.0f}}},
        MapPlace {MapLevel {35, {8.0f, 0.0f, 10.0f}}},
        MapPlace {MapLevel {36, {4.0f, 0.0f, 10.0f}}},
        MapPlace {MapLevel {37, {2.0f, 2.0f, 10.0f}}},
        MapPlace {MapLevel {38, {4.0f, 4.0f, 10.0f}}},
        MapPlace {MapLevel {39, {8.0f, 4.0f, 10.0f}}},
        MapPlace {Portal {2, 40, {12.0f, 4.0f, 10.0f}}}
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

World RowBlast::GetWorld1() {
    return World {places, blockPaths, cloudPaths, hazeLayers};
}
