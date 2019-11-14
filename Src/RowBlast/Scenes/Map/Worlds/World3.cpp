#include "World3.hpp"

using namespace RowBlast;

namespace {
    const std::vector<MapPlace> places {
        MapPlace {Portal {2, 34, {-24.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {34, {-21.5f, -1.2f, 9.0f}}},
        MapPlace {MapLevel {35, {-18.0f, -1.2f, 9.0f}}},
        MapPlace {MapLevel {36, {-16.0f, -2.5f, 10.0f}, {0.0f, 2.5f, 0.0f}}},
        MapPlace {MapLevel {37, {-13.0f, -4.0f, 10.0f}, {0.0f, 2.5f, 0.0f}}},
        MapPlace {MapLevel {38, {-10.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {39, {-8.0f, -1.0f, 9.0f}}},
        MapPlace {MapLevel {40, {-6.0f, 0.5f, 9.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {41, {-3.0f, 1.0f, 9.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {42, {-0.5f, -1.0f, 9.5f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {43, {1.0f, -3.5f, 9.5f}}},
        MapPlace {MapLevel {44, {4.0f, -4.0f, 10.0f}, {0.0f, 2.6f, 0.0f}}},
        MapPlace {MapLevel {45, {7.0f, -3.0f, 10.0f}}},
        MapPlace {MapLevel {46, {9.0f, -0.5f, 10.0f}}},
        MapPlace {MapLevel {47, {11.5f, 1.5f, 10.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {48, {14.5f, 2.0f, 10.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {49, {17.5f, 1.0f, 10.0f}, {0.0f, -0.7f, 0.0f}}},
        MapPlace {MapLevel {50, {20.0f, -1.0f, 10.0f}, {0.0f, 2.3f, 1.0f}}},
        MapPlace {Portal {4, 51, {23.0f, -1.5f, 10.0f}}}
    };
    
    const std::vector<TerrainSegment>& terrainSegments {
        TerrainSegment {{-48.0f, -10.0f, -40.0f}, TerrainMesh::Mesh1, TerrainMaterial::Moon1},
        TerrainSegment {{49.0f, -10.0f, -40.0f}, TerrainMesh::Mesh1, TerrainMaterial::Moon1},
        TerrainSegment {{0.0f, -11.0f, -140.0f}, TerrainMesh::Mesh1, TerrainMaterial::Moon1}
    };

    const std::vector<BlockPathVolume> blockPaths {
        BlockPathVolume {{-27.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{-25.0f, 3.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-20.5f, -3.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::Asteroid},
        BlockPathVolume {{-17.0f, -4.0f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{-17.0f, 2.0f, 7.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::Asteroid},
        BlockPathVolume {{-13.0f, -1.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-12.0f, -4.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-9.0f, 0.5f, 5.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{-5.0f, -4.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::ShortI},
        BlockPathVolume {{-5.0f, -3.0f, 13.7f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::Asteroid},
        BlockPathVolume {{-1.0f, 5.9f, 8.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{1.0f, -7.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{3.0f, 0.5f, 8.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{5.0f, -5.0f, 11.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{6.0f, 4.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::BigAsteroid},
        BlockPathVolume {{10.0f, -3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
        BlockPathVolume {{13.0f, -10.0f, 4.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{14.5f, -3.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::B},
        BlockPathVolume {{17.0f, -3.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::BigAsteroid},
        BlockPathVolume {{20.0f, -3.5f, 12.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::I},
        BlockPathVolume {{23.0f, 3.5f, 3.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::SingleBlock},
        BlockPathVolume {{27.0f, -7.5f, 4.0f}, {0.0f, 0.0f, 0.0f}, FloatingPieceType::L},
    };
    
    const std::vector<CloudPathVolume> cloudPaths {};
    const std::vector<HazeLayer> hazeLayers {};

    const std::vector<PlanetConfig> planets {
        PlanetConfig {
            .mPosition = {50.0f, 75.0f, -550.0f},
            .mOrientation = {15.0f, 0.0f, 35.0f},
            .mSize = 3.5f,
            .mAngularVelocity = 2.5f,
            .mType = PlanetType::Wadow,
            .mShaderId = Pht::ShaderId::TexturedPixelLighting
        },
        PlanetConfig {
            .mPosition = {200.0f, -300.0f, -420.0f},
            .mOrientation = {0.0f, 90.0f, 0.0f},
            .mSize = 4.5f,
            .mAngularVelocity = 2.5f,
            .mType = PlanetType::Rayeon
        },
        PlanetConfig {
            .mPosition = {180.0f, 200.0f, -500.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 1.2f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Moon
        }
    };

    const std::vector<BackgroundLight> backgroundLights {
        BackgroundLight {
            .mDirection = {-1.0f, 1.0f, 0.25f},
            .mIntensity = 1.15f,
            .mSun = Sun {.mPosition = {-20.0f, 440.0f, -720.0f}, .mSize = {580.0f, 580.0f}}
        },
        BackgroundLight {
            .mDirection = {-0.75f, 1.0f, -0.5f},
            .mIntensity = 1.15f,
            .mSun = Sun {.mPosition = {-10.0f, 240.0f, -620.0f}, .mSize = {580.0f, 580.0f}}
        }
    };

    constexpr auto ambientLightIntensity = 0.81f;
    constexpr auto directionalLightIntensity = 0.81f;

    const Pht::Color cloudColor {1.0f, 1.0f, 1.0f};
    const std::string backgroundTextureFilename {"space.jpg"};
}

World RowBlast::GetWorld3() {
    return World {
        places,
        terrainSegments,
        blockPaths,
        cloudPaths,
        hazeLayers,
        planets,
        backgroundLights,
        backgroundTextureFilename,
        ambientLightIntensity,
        directionalLightIntensity,
        cloudColor
    };
}
