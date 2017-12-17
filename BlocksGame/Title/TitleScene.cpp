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
    enum class Layer {
        Background = 0,
        Text = 1
    };

    const std::vector<CloudPathVolume> cloudPaths {
        CloudPathVolume {{0.0f, 0.0f, -10.0f}, {65.0f, 0.0f, 0.0f}},
        CloudPathVolume {{0.0f, -50.0f, -50.0f}, {110.0f, 0.0f, 10.0f}, {50.0f, 50.0f}},
        CloudPathVolume {{0.0f, -60.0f, -50.0f}, {110.0f, 0.0f, 10.0f}, {30.0f, 30.0f}},
        CloudPathVolume {{0.0f, -30.0f, -50.0f}, {110.0f, 0.0f, 10.0f}, {50.0f, 50.0f}},
        CloudPathVolume {{0.0f, 40.0f, -50.0f}, {110.0f, 0.0f, 10.0f}},
        CloudPathVolume {{0.0f, -50.0f, -100.0f}, {140.0f, 20.0f, 20.0f}},
        CloudPathVolume {{0.0f, -60.0f, -100.0f}, {140.0f, 20.0f, 20.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -30.0f, -100.0f}, {140.0f, 20.0f, 20.0f}},
        CloudPathVolume {{0.0f, -10.0f, -150.0f}, {200.0f, 30.0f, 30.0f}, {70.0f, 70.0f}},
        CloudPathVolume {{0.0f, -120.0f, -150.0f}, {200.0f, 30.0f, 30.0f}},
        CloudPathVolume {{0.0f, 30.0f, -150.0f}, {200.0f, 30.0f, 30.0f}, {30.0f, 30.0f}},
        CloudPathVolume {{0.0f, -10.0f, -200.0f}, {400.0f, 40.0f, 40.0f}},
        CloudPathVolume {{0.0f, -120.0f, -200.0f}, {400.0f, 40.0f, 40.0f}},
        CloudPathVolume {{0.0f, 30.0f, -200.0f}, {400.0f, 40.0f, 40.0f}, {70.0f, 70.0f}},
        CloudPathVolume {{0.0f, -200.0f, -300.0f}, {600.0f, 50.0f, 50.0f}},
        CloudPathVolume {{0.0f, 100.0f, -300.0f}, {600.0f, 50.0f, 50.0f}, {80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 150.0f, -300.0f}, {600.0f, 40.0f, 40.0f}},
        CloudPathVolume {{0.0f, 200.0f, -400.0f}, {800.0f, 60.0f, 60.0f}},
        CloudPathVolume {{0.0f, 300.0f, -400.0f}, {800.0f, 60.0f, 60.0f}, {20.0f, 20.0f}},
        CloudPathVolume {{0.0f, -300.0f, -400.0f}, {800.0f, 60.0f, 60.0f}},
        CloudPathVolume {{0.0f, 200.0f, -400.0f}, {800.0f, 60.0f, 60.0f}, {50.0f, 50.0f}},
        CloudPathVolume {{0.0f, 400.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -400.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 100.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 300.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -200.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 350.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -100.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 150.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 400.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -200.0f, -500.0f}, {1000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 500.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -400.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 100.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 350.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 200.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 350.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -100.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -150.0f, -600.0f}, {12000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -300.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -500.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 100.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 0.0f, -600.0f}, {12000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 300.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -500.0f, -600.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 500.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -400.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 100.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 350.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 200.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 350.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -100.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -150.0f, -700.0f}, {12000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -300.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -500.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 100.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 0.0f, -700.0f}, {12000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 300.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -500.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -300.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, -500.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, -600.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 0.0f, -700.0f}, {12000.0f, 80.0f, 80.0f}},
        CloudPathVolume {{0.0f, 300.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}, {60.0f, 60.0f}},
        CloudPathVolume {{0.0f, 400.0f, -700.0f}, {1200.0f, 80.0f, 80.0f}}
    };

    const std::vector<CubePathVolume> floatingCubePaths {
        CubePathVolume {
            .mPosition = {-3.0f, -3.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {-8.0f, -13.0f, -5.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {-5.0f, 13.0f, -8.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {10.0f, 23.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
            .mPosition = {1.0f, -25.0f, -18.0f},
            .mSize = {0.0f, 0.0f, 0.0f}
        },
        CubePathVolume {
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
    Pht::RenderPass textRenderPass {static_cast<int>(Layer::Text)};
    textRenderPass.SetHudMode(true);
    scene->AddRenderPass(textRenderPass);
    
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

    auto width {1500.0f};
    auto height {1050.0f};
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.55f, 0.76f, 1.0f, 1.0f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.55f, 0.76f, 1.0f, 1.0f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.17f, 0.34f, 1.0f, 1.0f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.17f, 0.34f, 1.0f, 1.0f}},
    };
    auto& background {scene->CreateSceneObject(Pht::QuadMesh {vertices}, Pht::Material {})};
    background.GetTransform().SetPosition({-20.0f, 0.0f, -700.0f});
    background.SetLayer(static_cast<int>(Layer::Background));
    scene->GetRoot().AddChild(background);
    
    mClouds = std::make_unique<Clouds>(engine,
                                       *scene,
                                       static_cast<int>(Layer::Background),
                                       cloudPaths,
                                       4.0f);

    mFloatingCubes = std::make_unique<FloatingCubes>(engine,
                                                     scene.get(),
                                                     static_cast<int>(Layer::Background),
                                                     floatingCubePaths,
                                                     commonResources,
                                                     7.7f);
    mFloatingCubes->Reset();
    
    auto& titleText {scene->CreateText("BLOCKS", {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}})};
    auto& titleTextSceneObject {titleText.GetSceneObject()};
    titleTextSceneObject.GetTransform().SetPosition({-6.5f, 5.0f, 0.0f});
    titleTextSceneObject.SetLayer(static_cast<int>(Layer::Text));
    scene->GetRoot().AddChild(titleTextSceneObject);

    auto& tapText {
        scene->CreateText("Tap to continue...", {mTapFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}})
    };
    auto& tapTextSceneObject {tapText.GetSceneObject()};
    tapTextSceneObject.GetTransform().SetPosition({-3.7f, -6.0f, 0.0f});
    tapTextSceneObject.SetLayer(static_cast<int>(Layer::Text));
    scene->GetRoot().AddChild(tapTextSceneObject);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceZ);
    
    sceneManager.SetLoadedScene(std::move(scene));
}

void TitleScene::Update() {
    mClouds->Update();
    mFloatingCubes->Update();
}
