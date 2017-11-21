#include "GameScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"

// Game includes.
#include "Level.hpp"
#include "ScrollController.hpp"
#include "CommonResources.hpp"

using namespace BlocksGame;

namespace {
    const auto fieldQuadZ {-1.0f};
    const auto lowerClipAreaHeightInCells {2.15f};
    const auto fieldPadding {0.1f};

    const std::vector<Volume> floatingCubePaths {
        Volume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        Volume {
            .mPosition = {0.0f, -15.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        Volume {
            .mPosition = {0.0f, 15.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        Volume {
            .mPosition = {0.0f, 25.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        Volume {
            .mPosition = {0.0f, -25.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        Volume {
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
    mFieldPosition {0.0f, 0.0f, 0.0f},
    mFloatingCubes {floatingCubePaths, engine, commonResources, 7.7f} {

    CreateBackground();
}

void GameScene::Reset(const Level& level) {
    mEngine.GetRenderer().SetLightDirection(mLightDirection);
    
    mFieldWidth = mCellSize * level.GetNumColumns();
    mFieldHeight = mCellSize * level.GetNumRows();
    
    mFieldLoweLeft = Pht::Vec2 {
        mFieldPosition.x - mFieldWidth / 2.0f,
        mFieldPosition.y - mFieldHeight / 2.0f
    };
    
    mScissorBoxSize = Pht::Vec2 {mFieldWidth + fieldPadding, 19.0f * mCellSize};
    
    CreateFieldQuad(level);
    UpdateCameraPosition();
    
    mFloatingCubes.Reset();
}

void GameScene::Update() {
    mFloatingCubes.Update();
    
    if (mScrollController.IsScrolling()) {
        UpdateCameraPosition();
    }
}

void GameScene::UpdateCameraPosition() {
    auto& renderer {mEngine.GetRenderer()};
    
    auto cameraYPosition {
        mFieldLoweLeft.y + mScrollController.GetLowestVisibleRow() * mCellSize +
        renderer.GetOrthographicFrustumSize().y / 2.0f - mCellSize * lowerClipAreaHeightInCells
    };

    Pht::Vec3 cameraPosition {0.0f, cameraYPosition, 20.5f};
    Pht::Vec3 target {0.0f, cameraYPosition, 0.0f};
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    renderer.LookAt(cameraPosition, target, up);
    
    mScissorBoxLowerLeft = Pht::Vec2 {
        mFieldPosition.x - (mFieldWidth + fieldPadding) / 2.0f,
        cameraYPosition - renderer.GetOrthographicFrustumSize().y / 2.0f +
        lowerClipAreaHeightInCells * mCellSize
    };
}

void GameScene::CreateBackground() {
    Pht::Material backgroundMaterial {"sky_blurred.jpg"};
    
    mBackground = std::make_unique<Pht::SceneObject>(
        mEngine.CreateRenderableObject(Pht::QuadMesh {150.0f, 150.0f}, backgroundMaterial));
    mBackground->Translate({0.0f, -5.0f, -42.0f});
}

void GameScene::CreateFieldQuad(const Level& level) {
    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.8f);

    auto vertices {CreateFieldVertices(level)};
    
    mFieldQuad = std::make_unique<Pht::SceneObject>(
        mEngine.CreateRenderableObject(Pht::QuadMesh {vertices}, fieldMaterial));
    mFieldQuad->Translate({mFieldPosition.x, mFieldPosition.y, mFieldPosition.z + fieldQuadZ});
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
    return mCommonResources.GetMaterials().GetGoldMaterial();
}

const Pht::Material& GameScene::GetRedMaterial() const {
    return mCommonResources.GetMaterials().GetRedMaterial();
}

const Pht::Material& GameScene::GetBlueMaterial() const {
    return mCommonResources.GetMaterials().GetBlueMaterial();
}

const Pht::Material& GameScene::GetGreenMaterial() const {
    return mCommonResources.GetMaterials().GetGreenMaterial();
}

const Pht::Material& GameScene::GameScene::GetGrayMaterial() const {
    return mCommonResources.GetMaterials().GetGrayMaterial();
}

const Pht::Material& GameScene::GameScene::GetDarkGrayMaterial() const {
    return mCommonResources.GetMaterials().GetDarkGrayMaterial();
}

const Pht::Material& GameScene::GameScene::GetLightGrayMaterial() const {
    return mCommonResources.GetMaterials().GetLightGrayMaterial();
}

const Pht::Material& GameScene::GetYellowMaterial() const {
    return mCommonResources.GetMaterials().GetYellowMaterial();
}

const std::vector<FloatingCube>& GameScene::GetFloatingCubes() const {
    return mFloatingCubes.GetCubes();
}
