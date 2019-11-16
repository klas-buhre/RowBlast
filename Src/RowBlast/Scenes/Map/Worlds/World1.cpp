#include "World1.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapPlace> places {
        MapPlace {MapLevel {1, {-24.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {2, {-21.5f, -1.2f, 9.0f}}},
        MapPlace {MapLevel {3, {-18.0f, -1.2f, 9.0f}}},
        MapPlace {MapLevel {4, {-16.0f, -2.5f, 10.0f}, {0.0f, 2.5f, 0.0f}}},
        MapPlace {MapLevel {5, {-13.0f, -4.0f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {6, {-10.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {7, {-8.0f, -1.0f, 9.0f}, {0.0f, -0.25f, 0.0f}}},
        MapPlace {MapLevel {8, {-6.0f, 0.5f, 9.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {9, {-3.0f, 1.0f, 9.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {10, {-0.5f, -1.0f, 9.5f}, {0.0f, -0.25f, 0.0f}}},
        MapPlace {MapLevel {11, {1.0f, -3.5f, 9.5f}}},
        MapPlace {MapLevel {12, {4.0f, -4.0f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {13, {7.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {14, {9.0f, -0.5f, 10.0f}, {0.0f, -0.25f, 0.0f}}},
        MapPlace {MapLevel {15, {11.5f, 1.5f, 10.0f}, {0.0f, -1.0f, 0.0f}}},
        MapPlace {MapLevel {16, {14.5f, 2.0f, 10.0f}, {0.0f, -1.0f, 0.0f}}},
        MapPlace {MapLevel {17, {17.5f, 1.0f, 10.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {18, {20.0f, -1.0f, 10.0f}, {0.0f, 2.3f, 1.0f}}},
        MapPlace {Portal {2, 19, {23.0f, -1.5f, 10.0f}}}
    };

    const std::vector<TerrainSegment>& terrainSegments {
        TerrainSegment {{-48.0f, -10.0f, -40.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1Bright},
        TerrainSegment {{49.0f, -10.0f, -40.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1Bright},
        TerrainSegment {{-98.0f, -13.5f, -140.0f}, TerrainMesh::Mesh3, TerrainMaterial::Sand3Bright},
        TerrainSegment {{0.0f, -11.0f, -140.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1Bright},
        TerrainSegment {{98.0f, -14.0f, -140.0f}, TerrainMesh::Mesh3, TerrainMaterial::Sand3Bright},
        TerrainSegment {{49.0f, -12.0f, -240.0f}, TerrainMesh::Mesh1, TerrainMaterial::Sand1Bright},
        TerrainSegment {{-48.0f, -14.5f, -235.0f}, TerrainMesh::Mesh3, TerrainMaterial::Sand3Bright},
    };

    const std::vector<BlockPathVolume> blockPaths {
        BlockPathVolume {{-27.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{-21.0f, 3.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-21.0f, -3.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-19.0f, 3.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-17.0f, -4.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-17.0f, 3.0f, 7.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-14.0f, 3.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{-13.0f, 5.0f, -15.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-11.5f, -4.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::RowBomb},
        BlockPathVolume {{-11.0f, 0.5f, 7.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-8.5f, 2.5f, 7.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::Bomb, .mBlockRotation = Pht::Vec3{45.0f, 0.0f, 45.0f}},
        BlockPathVolume {{-7.0f, -4.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-5.0f, -4.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{-3.0f, -3.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-1.0f, 5.0f, 8.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{1.0f, -7.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{3.0f, 0.5f, 8.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{5.0f, -5.0f, 11.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{6.0f, 7.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{8.0f, 3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{10.0f, -3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{13.0f, -7.0f, 4.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{14.0f, 6.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{14.5f, -3.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{17.0f, 3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{18.5f, -3.0f, 4.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{20.0f, -3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{23.0f, 3.5f, 3.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{27.0f, -7.5f, 4.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L}
    };

    const std::vector<CloudPathVolume> cloudPaths {
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
            .mCloudSize = {30.0f, 15.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 5.0f, -80.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {30.0f, 15.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
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
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.0f},
            .mLowerColor = {0.455f, 0.7625f, 0.9725f, 1.0f},
            .mUpperScale = 0.5f
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -369.0f},
            .mSize = {1000.0f, 105.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.0f},
            .mLowerColor = {0.455f, 0.7625f, 0.9725f, 0.0f},
            .mMidColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 650.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.455f, 0.7625f, 0.9725f, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {380.0f, 480.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.455f, 0.7625f, 0.9725f, 0.35f}
        }
    };
    
    const std::vector<PlanetConfig> planets {
        PlanetConfig {
            .mPosition = {0.0f, 200.0f, -550.0f},
            .mOrientation = {40.0f, 0.0f, -45.0f},
            .mSize = 3.3f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Ogma
        },
        PlanetConfig {
            .mPosition = {-30.0f, 170.0f, -420.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 0.9f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Moon
        }
    };

    const std::vector<BackgroundLight> backgroundLights {
        BackgroundLight {
            .mDirection = {-1.0f, 1.0f, 1.0f},
            .mIntensity = 1.21f,
            .mSun = Sun {.mPosition = {-40.0f, 470.0f, -720.0f}, .mSize = {480.0f, 480.0f}}
        }
    };
    
    constexpr auto lightIntensity = 0.98f;
    const Pht::Color cloudColor {1.0f, 1.0f, 1.0f};
}

World RowBlast::GetWorld1() {
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
