#include "GameScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "ISceneManager.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "Level.hpp"
#include "ScrollController.hpp"
#include "CommonResources.hpp"

using namespace BlocksGame;

namespace {
    const auto fieldQuadZ {-1.0f};
    const auto lowerClipAreaHeightInCells {2.15f};
    const auto fieldPadding {0.1f};
    
    enum class Layer {
        Background,
        Field
    };

    const std::vector<CubePathVolume> floatingCubePaths {
        CubePathVolume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        CubePathVolume {
            .mPosition = {0.0f, -15.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        CubePathVolume {
            .mPosition = {0.0f, 15.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        CubePathVolume {
            .mPosition = {0.0f, 25.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        CubePathVolume {
            .mPosition = {0.0f, -25.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        CubePathVolume {
            .mPosition = {0.0f, 0.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        }
    };
}
        
GameScene::GameScene(Pht::IEngine& engine,
                     const ScrollController& scrollController,
                     const CommonResources& commonResources) :
    mEngine {engine},
    mScrollController {scrollController},
    mCommonResources {commonResources},
    mLightDirection {1.0f, 1.0f, 0.74f},
    mFieldPosition {0.0f, 0.0f, 0.0f} {}

void GameScene::Reset(const Level& level) {
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("gameScene"))};
    mScene = scene.get();
    
    scene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Background)});
    
    Pht::RenderPass fieldRenderPass {static_cast<int>(Layer::Field)};
    fieldRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    scene->AddRenderPass(fieldRenderPass);

    auto& light {scene->CreateGlobalLight()};
    light.SetDirection(mLightDirection);
    scene->GetRoot().AddChild(light.GetSceneObject());
    
    mCamera = &scene->CreateCamera();
    scene->GetRoot().AddChild(mCamera->GetSceneObject());

    CreateBackground();
    
    mFloatingCubes = std::make_unique<FloatingCubes>(mEngine,
                                                     *mScene,
                                                     static_cast<int>(Layer::Background),
                                                     floatingCubePaths,
                                                     mCommonResources,
                                                     7.7f);
    
    mFieldWidth = mCellSize * level.GetNumColumns();
    mFieldHeight = mCellSize * level.GetNumRows();
    
    mFieldLoweLeft = Pht::Vec2 {
        mFieldPosition.x - mFieldWidth / 2.0f,
        mFieldPosition.y - mFieldHeight / 2.0f
    };
    
    mScissorBoxSize = Pht::Vec2 {mFieldWidth + fieldPadding, 19.0f * mCellSize};
    
    CreateFieldQuad(level);
    UpdateCameraPositionAndScissorBox();
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    sceneManager.SetLoadedScene(std::move(scene));
}

void GameScene::Update() {
    mFloatingCubes->Update();
    
    if (mScrollController.IsScrolling()) {
        UpdateCameraPositionAndScissorBox();
    }
}
    
void GameScene::UpdateCameraPositionAndScissorBox() {
    auto& renderer {mEngine.GetRenderer()};
    
    auto cameraYPosition {
        mFieldLoweLeft.y + mScrollController.GetLowestVisibleRow() * mCellSize +
        renderer.GetOrthographicFrustumSize().y / 2.0f - mCellSize * lowerClipAreaHeightInCells
    };

    Pht::Vec3 cameraPosition {0.0f, cameraYPosition, 20.5f};
    mCamera->GetSceneObject().GetTransform().SetPosition(cameraPosition);
    
    Pht::Vec3 target {0.0f, cameraYPosition, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera->SetTarget(target, up);
    
    mScissorBoxLowerLeft = Pht::Vec2 {
        mFieldPosition.x - (mFieldWidth + fieldPadding) / 2.0f,
        cameraYPosition - renderer.GetOrthographicFrustumSize().y / 2.0f +
        lowerClipAreaHeightInCells * mCellSize
    };
    
    auto* fieldRenderPass {mScene->GetRenderPass(static_cast<int>(Layer::Field))};
    assert(fieldRenderPass);
    
    Pht::ScissorBox scissorBox {mScissorBoxLowerLeft, mScissorBoxSize};
    fieldRenderPass->SetScissorBox(scissorBox);
}

void GameScene::CreateBackground() {
    Pht::Material backgroundMaterial {"sky_blurred.jpg"};
    auto& background {mScene->CreateSceneObject(Pht::QuadMesh {150.0f, 150.0f}, backgroundMaterial)};
    background.GetTransform().SetPosition({0.0f, -5.0f, -42.0f});
    background.SetLayer(static_cast<int>(Layer::Background));
    mScene->GetRoot().AddChild(background);
}

void GameScene::CreateFieldQuad(const Level& level) {
    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.8f);

    auto vertices {CreateFieldVertices(level)};
    auto& fieldQuad {mScene->CreateSceneObject(Pht::QuadMesh {vertices}, fieldMaterial)};
    Pht::Vec3 quadPosition {mFieldPosition.x, mFieldPosition.y, mFieldPosition.z + fieldQuadZ};
    fieldQuad.GetTransform().SetPosition(quadPosition);
    fieldQuad.SetLayer(static_cast<int>(Layer::Field));
    mScene->GetRoot().AddChild(fieldQuad);
}

Pht::QuadMesh::Vertices GameScene::CreateFieldVertices(const Level& level) {
    auto width {mFieldWidth + fieldPadding};
    auto height {mFieldHeight + fieldPadding};
    
    switch (level.GetColor()) {
        case Level::Color::Pink:
            return {
                {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.2f, 0.2f, 0.75f, 1.0f}},
                {{width / 2.0f, -height / 2.0f, 0.0f}, {0.75f, 0.2f, 0.4f, 1.0f}},
                {{width / 2.0f, height / 2.0f, 0.0f}, {0.2f, 0.2f, 0.9f, 1.0f}},
                {{-width / 2.0f, height / 2.0f, 0.0f}, {0.9f, 0.2f, 0.4f, 1.0f}},
            };
        case Level::Color::Green:
            return {
                {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.2f, 0.2f, 0.6f, 1.0f}},
                {{width / 2.0f, -height / 2.0f, 0.0f}, {0.2f, 0.2f, 0.6f, 1.0f}},
                {{width / 2.0f, height / 2.0f, 0.0f}, {0.2f, 0.7f, 0.4f, 1.0f}},
                {{-width / 2.0f, height / 2.0f, 0.0f}, {0.9f, 0.2f, 0.4f, 1.0f}},
            };
    }
}

const Pht::Material& GameScene::GetGoldMaterial() const {
    return mCommonResources.GetMaterials().GetGoldFieldBlockMaterial();
}

const Pht::Material& GameScene::GetRedMaterial() const {
    return mCommonResources.GetMaterials().GetRedFieldBlockMaterial();
}

const Pht::Material& GameScene::GetBlueMaterial() const {
    return mCommonResources.GetMaterials().GetBlueFieldBlockMaterial();
}

const Pht::Material& GameScene::GetGreenMaterial() const {
    return mCommonResources.GetMaterials().GetGreenFieldBlockMaterial();
}

const Pht::Material& GameScene::GameScene::GetGrayMaterial() const {
    return mCommonResources.GetMaterials().GetGrayFieldBlockMaterial();
}

const Pht::Material& GameScene::GameScene::GetDarkGrayMaterial() const {
    return mCommonResources.GetMaterials().GetDarkGrayFieldBlockMaterial();
}

const Pht::Material& GameScene::GetYellowMaterial() const {
    return mCommonResources.GetMaterials().GetYellowFieldBlockMaterial();
}
