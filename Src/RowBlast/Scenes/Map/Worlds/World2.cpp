#include "World2.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapPlace> places {
        MapPlace {Portal {1, 19, {-24.0f, 3.0f, 10.0f}}},
        MapPlace {MapLevel {19, {-21.5f, 1.0f, 10.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {20, {-19.5f, -1.5f, 10.0f}}},
        MapPlace {MapLevel {21, {-17.0f, -3.5f, 10.0f}}},
        MapPlace {MapLevel {22, {-13.5f, -4.0f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {23, {-10.0f, -3.5f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {24, {-7.5f, -2.0f, 10.0f}, {0.0f, 2.3f, 0.0f}}},
        MapPlace {MapLevel {25, {-4.5f, -1.5f, 10.0f}, {-0.5f, 2.3f, 0.0f}}},
        MapPlace {MapLevel {26, {-1.5f, -2.0f, 10.0f}}},
        MapPlace {MapLevel {27, {1.5f, -3.5f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {28, {4.5f, -4.0f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {29, {8.0f, -3.5f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {30, {11.0f, -2.5f, 10.0f}}},
        MapPlace {MapLevel {31, {14.0f, -1.0f, 10.0f}}},
        MapPlace {MapLevel {32, {16.5f, 1.0f, 10.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {33, {19.5f, 1.5f, 10.0f}, {0.0f, 1.5f, 1.0f}}},
        MapPlace {Portal {3, 34, {23.0f, 1.5f, 10.0f}}}
    };

    const std::vector<TerrainSegment>& terrainSegments {
        TerrainSegment {{-48.0f, -10.0f, -40.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1},
        TerrainSegment {{49.0f, -10.0f, -40.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1},
        TerrainSegment {{-98.0f, -13.5f, -140.0f}, TerrainMesh::Mesh3, TerrainMaterial::Sand3},
        TerrainSegment {{0.0f, -11.0f, -140.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1},
        TerrainSegment {{98.0f, -14.0f, -140.0f}, TerrainMesh::Mesh3, TerrainMaterial::Sand3},
        TerrainSegment {{49.0f, -12.0f, -240.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1},
        TerrainSegment {{-48.0f, -14.5f, -235.0f}, TerrainMesh::Mesh3, TerrainMaterial::Sand3}
    };

    const std::vector<BlockPathVolume> blockPaths {
        BlockPathVolume {{-25.0f, -1.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{-22.0f, -3.0f, 13.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::RowBomb},
        BlockPathVolume {{-19.0f, 3.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{-17.0f, 3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-16.5f, -7.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{-16.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-14.0f, -5.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-12.0f, -2.0f, 8.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{-10.0f, -4.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::RowBomb},
        BlockPathVolume {{-7.0f, 3.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{-6.0f, -3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-3.0f, -4.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::Bomb},
        BlockPathVolume {{-3.0f, 2.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{0.0f, -4.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{4.0f, -5.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{6.0f, 0.0f, 8.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{8.0f, 2.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{9.0f, -5.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{11.0f, 2.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{14.0f, -4.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{16.5f, -4.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{15.0f, 3.0f, 13.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::RowBomb},
        BlockPathVolume {{19.0f, -3.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{23.0f, -3.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI}
    };

    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {
            .mPosition = {0.0f, -5.3f, 14.9f},
            .mSize = {70.0f, 0.0f, 0.0f},
            .mCloudSize = {7.0f, 7.0f},
            .mNumClouds = 2,
            .mVelocity = 0.75f
        },
        CloudPathVolume {
            .mPosition = {0.0f, 4.9f, 14.9f},
            .mSize = {70.0f, 0.0f, 0.0f},
            .mCloudSize = {7.0f, 7.0f},
            .mNumClouds = 2,
            .mVelocity = 0.75f
        },
        CloudPathVolume {
            .mPosition = {0.0f, 5.0f, -70.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {40.0f, 15.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 5.0f, -70.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {40.0f, 15.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 5.0f, -80.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {40.0f, 15.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 5.0f, -90.0f},
            .mSize = {250.0f, 0.0f, 20.0f},
            .mCloudSize = {40.0f, 15.0f},
            .mCloudSizeRandPart = 10.0f,
            .mNumClouds = 10,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {400.0f, 0.0f, 20.0f},
            .mCloudSize = {50.0f, 40.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -300.0f},
            .mSize = {510.0f, 0.0f, 20.0f},
            .mCloudSize = {50.0f, 40.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {510.0f, 0.0f, 20.0f},
            .mCloudSize = {50.0f, 40.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        }
    };

    const std::vector<HazeLayer> hazeLayers {
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -370.0f},
            .mSize = {1000.0f, 700.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.0f},
            .mLowerColor = {0.8f, 0.50375f, 0.67f, 1.0f},
            .mUpperScale = 0.5f
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -369.0f},
            .mSize = {1000.0f, 100.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.0f},
            .mLowerColor = {0.8f, 0.50375f, 0.67f, 0.0f},
            .mMidColor = Pht::Vec4{1.0f, 0.9f, 0.9f, 0.8f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 650.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.35f},
            .mLowerColor = {0.8f, 0.50375f, 0.67f, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {380.0f, 480.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.35f},
            .mLowerColor = {0.8f, 0.50375f, 0.67f, 0.35f}
        }
    };

    const Pht::Color cloudColor {1.035f, 0.95f, 0.975f};

    const std::vector<PlanetConfig> planets {
        PlanetConfig {
            .mPosition = {0.0f, 200.0f, -550.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 3.5f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Ogma
        },
        PlanetConfig {
            .mPosition = {-30.0f, 170.0f, -420.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 0.95f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Moon
        },
        PlanetConfig {
            .mPosition = {180.0f, 200.0f, -500.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 1.2f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Titawin
        }
    };
    
    const std::vector<BackgroundLight> backgroundLights {
        BackgroundLight {
            .mDirection = {-1.0f, 1.0f, 1.0f},
            .mIntensity = 1.23f,
            .mSun = Sun {.mPosition = {-90.0f, 450.0f, -720.0f}, .mSize = {450.0f, 450.0f}}
        }
    };

    constexpr auto lightIntensity = 0.895f;
}

World RowBlast::GetWorld2() {
    return World {
        places,
        terrainSegments,
        blockPaths,
        cloudPaths,
        hazeLayers,
        planets,
        backgroundLights,
        "",
        lightIntensity,
        lightIntensity,
        cloudColor
    };
}
