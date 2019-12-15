#include "TitleScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "SceneObject.hpp"
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"
#include "Universe.hpp"
#include "SunParticleEffect.hpp"
#include "Terrain.hpp"

using namespace RowBlast;

namespace {
    enum class Layer {
        Planets,
        Background,
        Ui,
        SunEffect,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
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

    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {400.0f, 0.0f, 20.0f},
            .mCloudSize = {50.0f, 40.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 2
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -300.0f},
            .mSize = {510.0f, 0.0f, 20.0f},
            .mCloudSize = {50.0f, 40.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 11,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {510.0f, 0.0f, 20.0f},
            .mCloudSize = {50.0f, 40.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 11,
            .mNumCloudsPerCluster = 5
        }
    };

    constexpr auto dayLightIntensity = 0.98f;
    const Pht::Color brightCloudColor {1.0f, 1.0f, 1.0f};

    const std::vector<HazeLayer> blueHazeLayers {
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

    constexpr auto sunsetLightIntensity = 0.895f;
    const Pht::Color sunsetCloudColor {1.035f, 0.95f, 0.975f};
    
    const std::vector<HazeLayer> sunsetHazeLayers {
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

    const std::vector<PlanetConfig> planets {
        PlanetConfig {
            .mPosition = {17.0f, 250.0f, -550.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 3.5f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Ogma
        },
        PlanetConfig {
            .mPosition = {110.0f, 233.0f, -420.0f},
            .mOrientation = {30.0f, 0.0f, -35.0f},
            .mSize = 0.95f,
            .mAngularVelocity = 5.0f,
            .mType = PlanetType::Moon
        }
    };

    const std::vector<BlockPathVolume> floatingBlockPaths {
        BlockPathVolume {
            .mPosition = {-8.0f, 6.0f, -22.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::B,
            .mBlockColor = FloatingBlockColor::Blue,
            .mBlockRotation = Pht::Vec3{45.0f, -20.0f, 90.0f}
        },
        BlockPathVolume {
            .mPosition = {10.0f, 5.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::I,
            .mBlockColor = FloatingBlockColor::Green,
            .mBlockRotation = Pht::Vec3{50.0f, -60.0f, -30.0f}
        },
        BlockPathVolume {
            .mPosition = {-5.0f, -4.0f, -10.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L,
            .mBlockColor = FloatingBlockColor::Gold,
            .mBlockRotation = Pht::Vec3{-10.0f, 60.0f, 40.0f}
        },
        BlockPathVolume {
            .mPosition = {5.0f, -4.0f, -5.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::SingleBlock,
            .mBlockColor = FloatingBlockColor::Red,
            .mBlockRotation = Pht::Vec3{20.0f, 60.0f, 60.0f}
        },
        BlockPathVolume {
            .mPosition = {-2.5f, -8.0f, 5.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::SingleBlock,
            .mBlockColor = FloatingBlockColor::RandomExceptGray,
            .mBlockRotation = Pht::Vec3{-10.0f, 60.0f, 40.0f}
        },
        BlockPathVolume {
            .mPosition = {1.0f, -9.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::RowBomb
        },
        BlockPathVolume {
            .mPosition = {4.0f, -12.0f, -2.7f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::Bomb,
            .mBlockRotation = Pht::Vec3{45.0f, 0.0f, 45.0f}
        }
    };
    
    float CalculateLightIntensity(int worldId) {
        switch (worldId) {
            case 1:
                return dayLightIntensity;
            case 2:
                return sunsetLightIntensity;
            default:
                return dayLightIntensity;
        }
    }

    Pht::Color CalculateCloudColor(int worldId) {
        switch (worldId) {
            case 1:
                return brightCloudColor;
            case 2:
                return sunsetCloudColor;
            default:
                return brightCloudColor;
        }
    }
    
    const std::vector<HazeLayer>& CalculateHazeLayers(int worldId) {
        switch (worldId) {
            case 1:
                return blueHazeLayers;
            case 2:
                return sunsetHazeLayers;
            default:
                return blueHazeLayers;
        }
    }

    int CalculateWorldId(const UserServices& userServices, const Universe& universe) {
        auto currentLevelId {userServices.GetProgressService().GetCurrentLevel()};
        auto mapSceneWorldId {universe.CalcWorldId(currentLevelId)};
        
        switch (mapSceneWorldId) {
            case 1:
                return 2;
            case 2:
                return 1;
            default:
                return (std::rand() % 2) + 1;
        }
    }
}

TitleScene::TitleScene(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       const UserServices& userServices,
                       const Universe& universe) :
    mEngine {engine},
    mCommonResources {commonResources},
    mUserServices {userServices},
    mUniverse {universe} {}
    
void TitleScene::Init() {
    auto& sceneManager = mEngine.GetSceneManager();
    auto scene = sceneManager.CreateScene(Pht::Hash::Fnv1a("titleScene"));
    mScene = scene.get();
    
    Pht::RenderPass planetsRenderPass {static_cast<int>(Layer::Planets)};
    planetsRenderPass.SetRenderOrder(Pht::RenderOrder::PiexelOptimized);
    scene->AddRenderPass(planetsRenderPass);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Background)});
    
    Pht::RenderPass uiRenderPass {static_cast<int>(Layer::Ui)};
    uiRenderPass.SetHudMode(true);
    scene->AddRenderPass(uiRenderPass);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::SunEffect)});
    
    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    scene->AddRenderPass(fadeEffectRenderPass);

    auto worldId = CalculateWorldId(mUserServices, mUniverse);

    auto& light = scene->CreateGlobalLight();
    light.SetDirection({1.0f, 1.0f, 1.0f});
    auto lightIntensity = CalculateLightIntensity(worldId);
    light.SetAmbientIntensity(lightIntensity);
    light.SetDirectionalIntensity(lightIntensity);
    scene->GetRoot().AddChild(light.GetSceneObject());

    auto& camera = scene->CreateCamera();
    Pht::Vec3 cameraPosition {0.0f, 0.0f, 20.5f};
    Pht::Vec3 target {0.0f, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    camera.GetSceneObject().GetTransform().SetPosition(cameraPosition);
    camera.SetTarget(target, up);
    scene->GetRoot().AddChild(camera.GetSceneObject());
    
    mUiContainer = &scene->CreateSceneObject();
    mUiContainer->SetLayer(static_cast<int>(Layer::Ui));
    scene->GetRoot().AddChild(*mUiContainer);
    
    mPlanets = std::make_unique<Planets>(mEngine,
                                         *scene,
                                         static_cast<int>(Layer::Planets),
                                         planets,
                                         Pht::Vec3{-1.0f, 1.0f, 1.0f},
                                         1.24f);

    mClouds = std::make_unique<Clouds>(mEngine,
                                       *scene,
                                       static_cast<int>(Layer::Background),
                                       cloudPaths,
                                       CalculateHazeLayers(worldId),
                                       1.3f,
                                       CalculateCloudColor(worldId));

    mFloatingBlocks = std::make_unique<FloatingBlocks>(mEngine,
                                                       *scene,
                                                       static_cast<int>(Layer::Background),
                                                       floatingBlockPaths,
                                                       mCommonResources,
                                                       6.6f,
                                                       20.0f,
                                                       6.0f);
    
    CreateTerrain(mEngine, *scene, static_cast<int>(Layer::Background), terrainSegments);

    CreateSunParticleEffect(mEngine,
                            *scene,
                            Sun {.mPosition = {-120.0f, 475.0f, -720.0f}, .mSize = {450.0f, 450.0f}},
                            static_cast<int>(Layer::SunEffect));

    mUfoContainer = &scene->CreateSceneObject();
    mUfoContainer->SetLayer(static_cast<int>(Layer::Background));
    scene->GetRoot().AddChild(*mUfoContainer);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    sceneManager.SetLoadedScene(std::move(scene));
}

void TitleScene::Update() {
    mPlanets->Update();
    mClouds->Update();
    mFloatingBlocks->Update();
}
