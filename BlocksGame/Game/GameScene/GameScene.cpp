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
#include "LevelResources.hpp"

using namespace BlocksGame;

namespace {
    const auto fieldQuadZ {-1.0f};
    const auto lowerClipAreaHeightInCells {2.15f};
    const auto fieldPadding {0.1f};
    
    enum class Layer {
        Background,
        FieldQuad,
        FieldBlueprintSlots,
        FieldPieceDropEffects,
        FieldBlocksAndFallingPiece
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

void GameScene::Reset(const Level& level, const LevelResources& levelResources) {
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("gameScene"))};
    mScene = scene.get();
    
    CreateRenderPasses();
    CreateLightAndCamera();
    InitFieldDimensions(level);
    
    CreateBackground();
    CreateFloatingCubes();
    CreateFieldQuad(level);
    CreateFieldContainer();
    CreateBlueprintSlots(level, levelResources);
    CreatePieceDropEffectsContainer();
    CreateFieldBlocksContainer();
    CreateSceneObjectPools(level);
    
    mScissorBoxSize = Pht::Vec2 {mFieldWidth + fieldPadding, 19.0f * mCellSize};
    
    UpdateCameraPositionAndScissorBox();
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    sceneManager.SetLoadedScene(std::move(scene));
}

void GameScene::CreateRenderPasses() {
    mScene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::Background)});

    Pht::RenderPass fieldQuadRenderPass {static_cast<int>(Layer::FieldQuad)};
    fieldQuadRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    mScene->AddRenderPass(fieldQuadRenderPass);

    Pht::RenderPass blueprintSlotsRenderPass {static_cast<int>(Layer::FieldBlueprintSlots)};
    blueprintSlotsRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    mScene->AddRenderPass(blueprintSlotsRenderPass);
    
    Pht::RenderPass pieceDropEffectsRenderPass {static_cast<int>(Layer::FieldPieceDropEffects)};
    pieceDropEffectsRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    mScene->AddRenderPass(pieceDropEffectsRenderPass);

    Pht::RenderPass fieldBlocksRenderPass {static_cast<int>(Layer::FieldBlocksAndFallingPiece)};
    fieldBlocksRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    mScene->AddRenderPass(fieldBlocksRenderPass);
}

void GameScene::CreateLightAndCamera() {
    auto& light {mScene->CreateGlobalLight()};
    light.SetDirection(mLightDirection);
    mScene->GetRoot().AddChild(light.GetSceneObject());
    
    mCamera = &mScene->CreateCamera();
    mScene->GetRoot().AddChild(mCamera->GetSceneObject());
}

void GameScene::CreateBackground() {
    Pht::Material backgroundMaterial {"sky_blurred.jpg"};
    auto& background {mScene->CreateSceneObject(Pht::QuadMesh {150.0f, 150.0f}, backgroundMaterial)};
    background.GetTransform().SetPosition({0.0f, -5.0f, -42.0f});
    background.SetLayer(static_cast<int>(Layer::Background));
    mScene->GetRoot().AddChild(background);
}

void GameScene::CreateFloatingCubes() {
    mFloatingCubes = std::make_unique<FloatingCubes>(mEngine,
                                                     *mScene,
                                                     static_cast<int>(Layer::Background),
                                                     floatingCubePaths,
                                                     mCommonResources,
                                                     7.7f);
}

void GameScene::InitFieldDimensions(const Level& level) {
    mFieldWidth = mCellSize * level.GetNumColumns();
    mFieldHeight = mCellSize * level.GetNumRows();
    
    mFieldLoweLeft = Pht::Vec2 {
        mFieldPosition.x - mFieldWidth / 2.0f,
        mFieldPosition.y - mFieldHeight / 2.0f
    };
}

void GameScene::CreateFieldQuad(const Level& level) {
    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.8f);

    auto vertices {CreateFieldVertices(level)};
    auto& fieldQuad {mScene->CreateSceneObject(Pht::QuadMesh {vertices}, fieldMaterial)};
    Pht::Vec3 quadPosition {mFieldPosition.x, mFieldPosition.y, mFieldPosition.z + fieldQuadZ};
    fieldQuad.GetTransform().SetPosition(quadPosition);
    fieldQuad.SetLayer(static_cast<int>(Layer::FieldQuad));
    mScene->GetRoot().AddChild(fieldQuad);
}

void GameScene::CreateFieldContainer() {
    mFieldContainer = &mScene->CreateSceneObject();
    mFieldContainer->GetTransform().SetPosition({mFieldLoweLeft.x, mFieldLoweLeft.y, 0.0f});
    mScene->GetRoot().AddChild(*mFieldContainer);
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

void GameScene::CreateBlueprintSlots(const Level& level, const LevelResources& levelResources) {
    auto* blueprintGrid {level.GetBlueprintGrid()};
    
    if (blueprintGrid == nullptr) {
        return;
    }
    
    auto& blueprintSlotsContainer {mScene->CreateSceneObject()};
    blueprintSlotsContainer.SetLayer(static_cast<int>(Layer::FieldBlueprintSlots));
    mFieldContainer->AddChild(blueprintSlotsContainer);

    for (auto row {0}; row < level.GetNumRows(); ++row) {
        for (auto column {0}; column < level.GetNumColumns(); ++column) {
            auto& blueprintCell {(*blueprintGrid)[row][column]};
            
            if (blueprintCell.mFill != Fill::Empty) {
                auto& blueprintSlot {mScene->CreateSceneObject()};
                blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotRenderable());
                
                Pht::Vec3 blueprintSlotPosition {
                    column * mCellSize + mCellSize / 2.0f,
                    row * mCellSize + mCellSize / 2.0f,
                    mBlueprintZ
                };
                
                blueprintSlot.GetTransform().SetPosition(blueprintSlotPosition);
                blueprintSlotsContainer.AddChild(blueprintSlot);
            }
        }
    }
}

void GameScene::CreatePieceDropEffectsContainer() {
    mPieceDropEffectsContainer = &mScene->CreateSceneObject();
    mPieceDropEffectsContainer->SetLayer(static_cast<int>(Layer::FieldPieceDropEffects));
    mFieldContainer->AddChild(*mPieceDropEffectsContainer);
}

void GameScene::CreateFieldBlocksContainer() {
    mFieldBlocksContainer = &mScene->CreateSceneObject();
    mFieldBlocksContainer->SetLayer(static_cast<int>(Layer::FieldBlocksAndFallingPiece));
    mFieldContainer->AddChild(*mFieldBlocksContainer);
}

void GameScene::CreateSceneObjectPools(const Level& level) {
    mFieldBlocks = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::FieldBlocks,
                                                     *mFieldBlocksContainer,
                                                     level);
    mPieceBlocks = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PieceBlocks,
                                                     *mFieldBlocksContainer,
                                                     level);
    mGhostPieces = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::GhostPieces,
                                                     *mFieldBlocksContainer,
                                                     level);
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
    
    Pht::ScissorBox scissorBox {mScissorBoxLowerLeft, mScissorBoxSize};
    
    SetScissorBox(scissorBox, static_cast<int>(Layer::FieldQuad));
    SetScissorBox(scissorBox, static_cast<int>(Layer::FieldBlueprintSlots));
    SetScissorBox(scissorBox, static_cast<int>(Layer::FieldPieceDropEffects));
    SetScissorBox(scissorBox, static_cast<int>(Layer::FieldBlocksAndFallingPiece));
}

void GameScene::SetScissorBox(const Pht::ScissorBox& scissorBox, int layer) {
    auto* renderPass {mScene->GetRenderPass(layer)};
    assert(renderPass);
    
    renderPass->SetScissorBox(scissorBox);
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
