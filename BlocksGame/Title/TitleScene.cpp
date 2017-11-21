#include "TitleScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"

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
    mEngine {engine},
    mFloatingCubes {floatingCubePaths, engine, commonResources, 7.7f},
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(100)},
    mTitleText {{-6.5f, 5.0f}, "BLOCKS", {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}}},
    mTapFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(35)},
    mTapText {{-3.7f, -6.0f}, "Tap to continue...", {
        mTapFont,
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    }} {

    CreateBackground(commonResources.GetMaterials().GetSkyMaterial());
}
    
void TitleScene::Reset() {
    auto& renderer {mEngine.GetRenderer()};
    renderer.SetLightDirection({1.0f, 1.0f, 1.0f});
    renderer.SetDirectionalLightIntensity(1.0f);
    
    Pht::Vec3 cameraPosition {0.0f, 0.0f, 20.5f};
    Pht::Vec3 target {0.0f, 0.0f, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    renderer.LookAt(cameraPosition, target, up);
    
    mFloatingCubes.Reset();
}

void TitleScene::Update() {
    mFloatingCubes.Update();
}

const std::vector<FloatingCube>& TitleScene::GetFloatingCubes() const {
    return mFloatingCubes.GetCubes();
}

void TitleScene::CreateBackground(const Pht::Material& backgroundMaterial) {
    mBackground = std::make_unique<Pht::SceneObject>(
        mEngine.CreateRenderableObject(Pht::QuadMesh {100.0f, 100.0f}, backgroundMaterial));
    mBackground->Translate({-20.0f, 0.0f, -35.0f});
}
