#include "TitleScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "SceneObject.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"
#include "Universe.hpp"

using namespace RowBlast;

namespace {
    enum class Layer {
        Planets,
        Background,
        Ui,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
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
            .mPosition = {0.0f, -40.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -20.0f, -50.0f},
            .mSize = {170.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -35.0f, -100.0f},
            .mSize = {250.0f, 90.0f, 20.0f},
            .mCloudSize = {60.0f, 60.0f},
            .mCloudSizeRandPart = 60.0f,
            .mNumClouds = 10,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -200.0f},
            .mSize = {400.0f, 160.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -300.0f},
            .mSize = {510.0f, 200.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -350.0f},
            .mSize = {510.0f, 200.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        }
    };

    constexpr auto dayLightIntensity {0.98f};
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
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 650.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.455f, 0.7625f, 0.9725f, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -250.0f},
            .mSize = {380.0f, 480.0f},
            .mUpperColor = {0.17f, 0.38f, 0.89f, 0.35f},
            .mLowerColor = {0.455f, 0.7625f, 0.9725f, 0.35f}
        }
    };

    constexpr auto sunsetLightIntensity {0.895f};
    const Pht::Color sunsetCloudColor {1.04f, 0.965f, 0.975f};
    
    const std::vector<HazeLayer> sunsetHazeLayers {
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -370.0f},
            .mSize = {1000.0f, 700.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.0f},
            .mLowerColor = {0.8225, 0.50375, 0.6475, 1.0f},
            .mUpperScale = 0.5f
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -350.0f},
            .mSize = {500.0f, 650.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.35f},
            .mLowerColor = {0.8225, 0.50375, 0.6475, 0.35f}
        },
        HazeLayer {
            .mPosition = {0.0f, 0.0f, -250.0f},
            .mSize = {380.0f, 480.0f},
            .mUpperColor = {0.29f, 0.215f, 0.79f, 0.35f},
            .mLowerColor = {0.8225, 0.50375, 0.6475, 0.35f}
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
            .mPosition = {-8.0f, 8.0f, -22.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::B,
            .mBlockColor = FloatingBlockColor::Blue
        },
        BlockPathVolume {
            .mPosition = {10.0f, 7.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::I,
            .mBlockColor = FloatingBlockColor::Green
        },
        BlockPathVolume {
            .mPosition = {-5.0f, -3.0f, -10.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L,
            .mBlockColor = FloatingBlockColor::Gold
        },
        BlockPathVolume {
            .mPosition = {5.0f, -4.0f, -5.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::SingleBlock,
            .mBlockColor = FloatingBlockColor::Red
        },
        BlockPathVolume {
            .mPosition = {-5.0f, -8.0f, 5.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::SingleBlock,
            .mBlockColor = FloatingBlockColor::RandomExceptGray
        },
        BlockPathVolume {
            .mPosition = {3.0f, -14.0f, -10.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::RowBomb
        },
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
    mTapFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(35)} {
    
    auto& sceneManager {engine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("titleScene"))};
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Planets)});
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Background)});
    
    Pht::RenderPass uiRenderPass {static_cast<int>(Layer::Ui)};
    uiRenderPass.SetHudMode(true);
    scene->AddRenderPass(uiRenderPass);
    
    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    scene->AddRenderPass(fadeEffectRenderPass);

    auto worldId {CalculateWorldId(userServices, universe)};

    auto& light {scene->CreateGlobalLight()};
    light.SetDirection({1.0f, 1.0f, 1.0f});
    auto lightIntensity {CalculateLightIntensity(worldId)};
    light.SetAmbientIntensity(lightIntensity);
    light.SetDirectionalIntensity(lightIntensity);
    scene->GetRoot().AddChild(light.GetSceneObject());

    auto& camera {scene->CreateCamera()};
    Pht::Vec3 cameraPosition {0.0f, 0.0f, 20.5f};
    Pht::Vec3 target {0.0f, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    camera.GetSceneObject().GetTransform().SetPosition(cameraPosition);
    camera.SetTarget(target, up);
    scene->GetRoot().AddChild(camera.GetSceneObject());
    
    auto& uiContainer {scene->CreateSceneObject()};
    uiContainer.SetLayer(static_cast<int>(Layer::Ui));
    scene->GetRoot().AddChild(uiContainer);
    
    mPlanets = std::make_unique<Planets>(engine,
                                         *scene,
                                         static_cast<int>(Layer::Planets),
                                         planets,
                                         Pht::Vec3{-1.0f, 1.0f, 1.0f},
                                         1.2f);

    mClouds = std::make_unique<Clouds>(engine,
                                       *scene,
                                       static_cast<int>(Layer::Background),
                                       cloudPaths,
                                       CalculateHazeLayers(worldId),
                                       2.1f,
                                       CalculateCloudColor(worldId));

    mFloatingBlocks = std::make_unique<FloatingBlocks>(engine,
                                                       *scene,
                                                       static_cast<int>(Layer::Background),
                                                       floatingBlockPaths,
                                                       commonResources,
                                                       7.7f,
                                                       20.0f);

    mUfoContainer = &scene->CreateSceneObject();
    mUfoContainer->SetLayer(static_cast<int>(Layer::Background));
    scene->GetRoot().AddChild(*mUfoContainer);

    mTitleAnimation = std::make_unique<TitleAnimation>(engine, *scene, uiContainer);
    
    Pht::TextProperties tapTextProperties {
        mTapFont,
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.35f, 0.35f, 0.35f, 0.75f}
    };

    auto& tapText {scene->CreateText("Tap to continue...", tapTextProperties)};
    mTapTextSceneObject = &tapText.GetSceneObject();
    mTapTextSceneObject->GetTransform().SetPosition({-3.6f, -9.0f, UiLayer::text});
    mTapTextSceneObject->SetLayer(static_cast<int>(Layer::Ui));
    mTapTextSceneObject->SetIsVisible(false);
    uiContainer.AddChild(*mTapTextSceneObject);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}

void TitleScene::Update() {
    mPlanets->Update();
    mClouds->Update();
    mFloatingBlocks->Update();
    mTitleAnimation->Update();
    
    if (mTitleAnimation->IsDone()) {
        mTapTextSceneObject->SetIsVisible(true);
    }
}
