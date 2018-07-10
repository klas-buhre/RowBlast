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

using namespace RowBlast;

namespace {
    enum class Layer {
        Background,
        Ui,
        SceneSwitchFadeEffect = GlobalLayer::sceneSwitchFadeEffect
    };
    
    constexpr auto lightIntensity {0.98f};

    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {75.0f, 0.0f, 0.0f},
        },
        CloudPathVolume {
            .mPosition = {0.0f, -50.0f, -50.0f},
            .mSize = {140.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -60.0f, -50.0f},
            .mSize = {150.0f, 0.0f, 10.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, -30.0f, -50.0f},
            .mSize = {150.0f, 0.0f, 10.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mNumClouds = 3,
            .mNumCloudsPerCluster = 3
        },
        CloudPathVolume {
            .mPosition = {0.0f, 40.0f, -50.0f},
            .mSize = {150.0f, 0.0f, 10.0f}
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -100.0f},
            .mSize = {210.0f, 180.0f, 20.0f},
            .mCloudSize = {60.0f, 60.0f},
            .mCloudSizeRandPart = 60.0f,
            .mNumClouds = 10,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -200.0f},
            .mSize = {380.0f, 320.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 65.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -300.0f},
            .mSize = {500.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        },
        CloudPathVolume {
            .mPosition = {0.0f, 0.0f, -400.0f},
            .mSize = {500.0f, 400.0f, 20.0f},
            .mCloudSize = {50.0f, 50.0f},
            .mCloudSizeRandPart = 50.0f,
            .mNumClouds = 16,
            .mNumCloudsPerCluster = 5
        }
    };
    
    const Pht::Color cloudColor {1.0f, 1.0f, 1.0f};
    
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

    const std::vector<BlockPathVolume> floatingBlockPaths {
        BlockPathVolume {
            .mPosition = {-3.0f, -3.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L
        },
        BlockPathVolume {
            .mPosition = {-8.0f, -13.0f, -5.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        BlockPathVolume {
            .mPosition = {-5.0f, 13.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L
        },
        BlockPathVolume {
            .mPosition = {10.0f, 23.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L
        },
        BlockPathVolume {
            .mPosition = {1.0f, -25.0f, -18.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        BlockPathVolume {
            .mPosition = {5.0f, 0.0f, -30.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        }
    };
}

TitleScene::TitleScene(Pht::IEngine& engine, const CommonResources& commonResources) :
    mTapFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(35)} {
    
    auto& sceneManager {engine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("titleScene"))};
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Background)});
    
    Pht::RenderPass uiRenderPass {static_cast<int>(Layer::Ui)};
    uiRenderPass.SetHudMode(true);
    scene->AddRenderPass(uiRenderPass);
    
    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    scene->AddRenderPass(fadeEffectRenderPass);

    auto& light {scene->CreateGlobalLight()};
    light.SetDirection({1.0f, 1.0f, 1.0f});
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

    mClouds = std::make_unique<Clouds>(engine,
                                       *scene,
                                       static_cast<int>(Layer::Background),
                                       cloudPaths,
                                       hazeLayers,
                                       2.1f,
                                       cloudColor);

    mFloatingBlocks = std::make_unique<FloatingBlocks>(engine,
                                                       *scene,
                                                       static_cast<int>(Layer::Background),
                                                       floatingBlockPaths,
                                                       commonResources,
                                                       7.7f,
                                                       20.0f);

    mTitleAnimation = std::make_unique<TitleAnimation>(engine, *scene, uiContainer);
    
    Pht::TextProperties tapTextProperties {
        mTapFont,
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };

    auto& tapText {scene->CreateText("Tap to continue...", tapTextProperties)};
    mTapTextSceneObject = &tapText.GetSceneObject();
    mTapTextSceneObject->GetTransform().SetPosition({-3.6f, -6.0f, UiLayer::text});
    mTapTextSceneObject->SetLayer(static_cast<int>(Layer::Ui));
    mTapTextSceneObject->SetIsVisible(false);
    uiContainer.AddChild(*mTapTextSceneObject);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}

void TitleScene::Update() {
    mClouds->Update();
    mFloatingBlocks->Update();
    mTitleAnimation->Update();
    
    if (mTitleAnimation->IsDone()) {
        mTapTextSceneObject->SetIsVisible(true);
    }
}
