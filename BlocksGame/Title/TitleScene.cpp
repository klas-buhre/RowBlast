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

using namespace BlocksGame;

namespace {
    const std::vector<Volume> floatingCubePaths {
        Volume {
            .mPosition = {-3.0f, -3.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {-8.0f, -13.0f, -5.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {-5.0f, 13.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {10.0f, 23.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {1.0f, -25.0f, -18.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        Volume {
            .mPosition = {5.0f, 0.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        }
    };
}

TitleScene::TitleScene(Pht::IEngine& engine, const CommonResources& commonResources) :
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(100)},
    mTapFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(35)} {
    
    auto& sceneManager {engine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("TitleScene"))};
    
    auto& light {scene->CreateGlobalLight()};
    light.SetDirection({1.0f, 1.0f, 1.0f});
    light.SetDirectionalIntensity(1.0f);
    scene->GetRoot().AddChild(light.GetSceneObject());

    auto& camera {scene->CreateCamera()};
    Pht::Vec3 cameraPosition {0.0f, 0.0f, 20.5f};
    Pht::Vec3 target {0.0f, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    camera.GetSceneObject().GetTransform().SetPosition(cameraPosition);
    camera.SetTarget(target, up);
    scene->GetRoot().AddChild(camera.GetSceneObject());
    
    auto& background {
        scene->CreateSceneObject(Pht::QuadMesh {100.0f, 100.0f},
                                 commonResources.GetMaterials().GetSkyMaterial())
    };
    background.GetTransform().SetPosition({-20.0f, 0.0f, -35.0f});
    scene->GetRoot().AddChild(background);
    
    mFloatingCubes = std::make_unique<FloatingCubes>(engine,
                                                     scene.get(),
                                                     floatingCubePaths,
                                                     commonResources,
                                                     7.7f);
    mFloatingCubes->Reset();
    
    auto& titleText {scene->CreateText("BLOCKS", {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}})};
    titleText.GetSceneObject().GetTransform().SetPosition({-6.5f, 5.0f, 0.0f});
    scene->GetRoot().AddChild(titleText.GetSceneObject());

    auto& tapText {
        scene->CreateText("Tap to continue...", {mTapFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}})
    };
    tapText.GetSceneObject().GetTransform().SetPosition({-3.7f, -6.0f, 0.0f});
    scene->GetRoot().AddChild(tapText.GetSceneObject());
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}

void TitleScene::Update() {
    mFloatingCubes->Update();
}
