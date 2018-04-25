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
            .mIsLPiece = true
        },
        BlockPathVolume {
            .mPosition = {-8.0f, -13.0f, -5.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        BlockPathVolume {
            .mPosition = {-5.0f, 13.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mIsLPiece = true
        },
        BlockPathVolume {
            .mPosition = {10.0f, 23.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mIsLPiece = true
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
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(100)},
    mTapFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(35)} {
    
    auto& sceneManager {engine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("titleScene"))};
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Background)});
    
    Pht::RenderPass uiRenderPass {static_cast<int>(Layer::Ui)};
    uiRenderPass.SetHudMode(true);
    scene->AddRenderPass(uiRenderPass);
    
    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    scene->AddRenderPass(fadeEffectRenderPass);

    auto& light {scene->CreateGlobalLight()};
    light.SetDirection({1.0f, 1.0f, 1.0f});
    scene->GetRoot().AddChild(light.GetSceneObject());

    auto& camera {scene->CreateCamera()};
    Pht::Vec3 cameraPosition {0.0f, 0.0f, 20.5f};
    Pht::Vec3 target {0.0f, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    camera.GetSceneObject().GetTransform().SetPosition(cameraPosition);
    camera.SetTarget(target, up);
    scene->GetRoot().AddChild(camera.GetSceneObject());

    mClouds = std::make_unique<Clouds>(engine,
                                       *scene,
                                       static_cast<int>(Layer::Background),
                                       cloudPaths,
                                       hazeLayers,
                                       3.0f);

    mFloatingBlocks = std::make_unique<FloatingBlocks>(engine,
                                                       *scene,
                                                       static_cast<int>(Layer::Background),
                                                       floatingBlockPaths,
                                                       commonResources,
                                                       7.7f,
                                                       20.0f);
    
    Pht::Vec4 textColor {1.0f, 1.0f, 1.0f, 1.0f};

    Pht::TextProperties titleTextProperties {
        mFont,
        1.0f,
        textColor,
        Pht::TextShadow::Yes,
        {0.1f, 0.1f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };

    auto& titleLine1 {scene->CreateText("ROW", titleTextProperties)};
    auto& titleLine1SceneObject {titleLine1.GetSceneObject()};
    titleLine1SceneObject.GetTransform().SetPosition({-4.0f, 7.0f, UiLayer::text});
    titleLine1SceneObject.SetLayer(static_cast<int>(Layer::Ui));
    scene->GetRoot().AddChild(titleLine1SceneObject);

    auto& titleLine2 {scene->CreateText("BLAST", titleTextProperties)};
    auto& titleLine2SceneObject {titleLine2.GetSceneObject()};
    titleLine2SceneObject.GetTransform().SetPosition({-5.6f, 4.8f, UiLayer::text});
    titleLine2SceneObject.SetLayer(static_cast<int>(Layer::Ui));
    scene->GetRoot().AddChild(titleLine2SceneObject);

    Pht::TextProperties tapTextProperties {
        mTapFont,
        1.0f,
        textColor,
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };

    auto& tapText {scene->CreateText("Tap to continue...", tapTextProperties)};
    auto& tapTextSceneObject {tapText.GetSceneObject()};
    tapTextSceneObject.GetTransform().SetPosition({-3.6f, -6.0f, UiLayer::text});
    tapTextSceneObject.SetLayer(static_cast<int>(Layer::Ui));
    scene->GetRoot().AddChild(tapTextSceneObject);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}

void TitleScene::Update() {
    mClouds->Update();
    mFloatingBlocks->Update();
}