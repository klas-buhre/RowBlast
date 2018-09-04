#include "GameScene.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "ISceneManager.hpp"
#include "Fnv1Hash.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"
#include "CameraShake.hpp"

// Game includes.
#include "Level.hpp"
#include "ScrollController.hpp"
#include "CommonResources.hpp"
#include "LevelResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fieldQuadZ {-1.0f};
    constexpr auto blueprintZ {-0.7f};
    constexpr auto fieldBorderZ {-0.5f};
    constexpr auto lowerClipAreaHeightInCells {2.15f};
    constexpr auto fieldPadding {0.1f};
    constexpr auto scissorBoxPadding {0.01f};
    constexpr auto lightAnimationDuration {5.0f};
    const Pht::Vec3 lightDirectionA {0.57f, 1.0f, 0.6f};
    const Pht::Vec3 lightDirectionB {1.0f, 1.0f, 0.74f};
    constexpr auto darkLevelLightIntensity {0.94f};
    constexpr auto brightLevelLightIntensity {0.985f};

#if 0
    const std::vector<BlockPathVolume> floatingBlockPaths {
        BlockPathVolume {
            .mPosition = {0.0f, 0.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        BlockPathVolume {
            .mPosition = {0.0f, -15.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        BlockPathVolume {
            .mPosition = {0.0f, 15.0f, -10.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        BlockPathVolume {
            .mPosition = {0.0f, 25.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        BlockPathVolume {
            .mPosition = {0.0f, -25.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        },
        BlockPathVolume {
            .mPosition = {0.0f, 0.0f, -20.0f},
            .mSize = {40.0f, 10.0f, 5.0f}
        }
    };
#endif

    const std::vector<BlockPathVolume> floatingBlockPaths {
        BlockPathVolume {
            .mPosition = {-10.0f, 15.0f, -10.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L,
            .mBlockColor = FloatingBlockColor::RandomExceptGray
        },
        BlockPathVolume {
            .mPosition = {12.0f, 22.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mBlockColor = FloatingBlockColor::RandomExceptGray
        },
        BlockPathVolume {
            .mPosition = {-10.0f, 0.0f, -10.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mBlockColor = FloatingBlockColor::RandomExceptGray
        },
        BlockPathVolume {
            .mPosition = {13.0f, 0.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mPieceType = FloatingPieceType::L,
            .mBlockColor = FloatingBlockColor::RandomExceptGray
        },
        BlockPathVolume {
            .mPosition = {-7.0f, -20.0f, -10.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mBlockColor = FloatingBlockColor::Gray
        },
        BlockPathVolume {
            .mPosition = {10.0f, -30.0f, -20.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mBlockColor = FloatingBlockColor::Gray
        },
    };
}

GameScene::GameScene(Pht::IEngine& engine,
                     const ScrollController& scrollController,
                     const CommonResources& commonResources,
                     GameHudController& gameHudController,
                     const Pht::CameraShake& cameraShake,
                     const GameHudRectangles& hudRectangles) :
    mEngine {engine},
    mScrollController {scrollController},
    mCommonResources {commonResources},
    mGameHudController {gameHudController},
    mCameraShake {cameraShake},
    mHudRectangles {hudRectangles},
    mFieldBorder {engine, *this, commonResources},
    mFieldGrid {engine, *this, commonResources},
    mFieldPosition {0.0f, 0.0f, 0.0f} {}

void GameScene::Init(const Level& level,
                     const LevelResources& levelResources,
                     const PieceResources& pieceResources,
                     const GameLogic& gameLogic) {
    auto& sceneManager {mEngine.GetSceneManager()};
    auto scene {sceneManager.CreateScene(Pht::Hash::Fnv1a("gameScene"))};
    mScene = scene.get();
    
    sceneManager.InitSceneSystems(CommonResources::narrowFrustumHeightFactor);
    
    CreateRenderPasses();
    CreateLightAndCamera();
    InitFieldDimensions(level);
    
    CreateBackground(level);
    CreateBackgroundLayerLight(level);
    CreateFloatingCubes();
    CreateLevelCompletedEffectsContainer();
    CreateFieldQuad();
    CreateFieldContainer();
    CreateBlueprintSlots(level, levelResources);
    CreatePieceDropEffectsContainer();
    CreateFieldBlocksContainer();
    CreateSceneObjectPools(level);
    CreateEffectsContainer();
    CreateFlyingBlocksContainer();
    CreateHud(gameLogic, levelResources, pieceResources, level);
    CreateUiViewsContainer();
    CreateStarsContainer();
    
    InitFieldBorder(level);
    mFieldGrid.Init(level);
    
    scene->SetDistanceFunction(Pht::DistanceFunction::WorldSpaceNegativeZ);
    sceneManager.SetLoadedScene(std::move(scene));
    
    UpdateCameraPosition();
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

    Pht::RenderPass effectsRenderPass {static_cast<int>(Layer::Effects)};
    effectsRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    mScene->AddRenderPass(effectsRenderPass);
    
    Pht::RenderPass flyingBlocksRenderPass {static_cast<int>(Layer::FlyingBlocks)};
    flyingBlocksRenderPass.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    mScene->AddRenderPass(flyingBlocksRenderPass);

    Pht::RenderPass hudRenderPass {static_cast<int>(Layer::Hud)};
    hudRenderPass.SetHudMode(true);
    mScene->AddRenderPass(hudRenderPass);
    
    Pht::RenderPass levelCompletedFadeEffectRenderPass {
        static_cast<int>(Layer::LevelCompletedFadeEffect)
    };
    levelCompletedFadeEffectRenderPass.SetHudMode(true);
    mScene->AddRenderPass(levelCompletedFadeEffectRenderPass);
    
    mScene->AddRenderPass(Pht::RenderPass {static_cast<int>(Layer::LevelCompletedEffects)});
    
    Pht::RenderPass uiViewsRenderPass {static_cast<int>(Layer::UiViews)};
    uiViewsRenderPass.SetHudMode(true);
    uiViewsRenderPass.SetIsDepthTestAllowed(false);
    uiViewsRenderPass.SetRenderOrder(Pht::RenderOrder::BackToFront);
    mScene->AddRenderPass(uiViewsRenderPass);

    Pht::RenderPass starsRenderPass {static_cast<int>(Layer::Stars)};
    starsRenderPass.SetHudMode(true);
    mScene->AddRenderPass(starsRenderPass);

    Pht::RenderPass fadeEffectRenderPass {static_cast<int>(Layer::SceneSwitchFadeEffect)};
    fadeEffectRenderPass.SetHudMode(true);
    mScene->AddRenderPass(fadeEffectRenderPass);
}

void GameScene::CreateLightAndCamera() {
    mLight = &mScene->CreateGlobalLight();
    mScene->GetRoot().AddChild(mLight->GetSceneObject());
    
    mCamera = &mScene->CreateCamera();
    mScene->GetRoot().AddChild(mCamera->GetSceneObject());
}

void GameScene::CreateBackground(const Level& level) {
    Pht::Material backgroundMaterial {level.GetBackgroundTextureFilename()};

    auto& background {mScene->CreateSceneObject(Pht::QuadMesh {300.0f, 300.0f}, backgroundMaterial)};
    background.GetTransform().SetPosition({0.0f, 0.0f, -115.0f});
    background.SetLayer(static_cast<int>(Layer::Background));
    mScene->GetRoot().AddChild(background);
}

void GameScene::CreateBackgroundLayerLight(const Level& level) {
    auto& lightSceneObject {mScene->CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    lightComponent->SetDirection(lightDirectionB);
    
    if (level.IsDark()) {
        lightComponent->SetAmbientIntensity(darkLevelLightIntensity);
        lightComponent->SetDirectionalIntensity(darkLevelLightIntensity);
    } else {
        lightComponent->SetAmbientIntensity(brightLevelLightIntensity);
        lightComponent->SetDirectionalIntensity(brightLevelLightIntensity);
    }

    auto* backgroundRenderPass {mScene->GetRenderPass(static_cast<int>(Layer::Background))};
    assert(backgroundRenderPass);
    backgroundRenderPass->SetLight(lightComponent.get());
    
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    mScene->GetRoot().AddChild(lightSceneObject);
}

void GameScene::CreateFloatingCubes() {
    mFloatingBlocks = std::make_unique<FloatingBlocks>(mEngine,
                                                       *mScene,
                                                       static_cast<int>(Layer::Background),
                                                       floatingBlockPaths,
                                                       mCommonResources,
                                                       7.7f,
                                                       8.0f);
}

void GameScene::CreateLevelCompletedEffectsContainer() {
    mLevelCompletedEffectsContainer = &mScene->CreateSceneObject();
    mLevelCompletedEffectsContainer->SetLayer(static_cast<int>(Layer::LevelCompletedEffects));
    mScene->GetRoot().AddChild(*mLevelCompletedEffectsContainer);
}

void GameScene::InitFieldDimensions(const Level& level) {
    mFieldWidth = mCellSize * level.GetNumColumns();
    mFieldHeight = mCellSize * level.GetNumRows();
    
    mFieldLoweLeft = Pht::Vec2 {
        mFieldPosition.x - mFieldWidth / 2.0f,
        mFieldPosition.y - mFieldHeight / 2.0f
    };
}

void GameScene::CreateFieldQuad() {
    mFieldQuadContainer = &mScene->CreateSceneObject();
    mFieldQuadContainer->SetLayer(static_cast<int>(Layer::FieldQuad));
    mScene->GetRoot().AddChild(*mFieldQuadContainer);

    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.95f);

    auto vertices {CreateFieldVertices()};
    auto& fieldQuad {mScene->CreateSceneObject(Pht::QuadMesh {vertices}, fieldMaterial)};
    Pht::Vec3 quadPosition {mFieldPosition.x, mFieldPosition.y, mFieldPosition.z + fieldQuadZ};
    fieldQuad.GetTransform().SetPosition(quadPosition);
    mFieldQuadContainer->AddChild(fieldQuad);
}

void GameScene::CreateFieldContainer() {
    mFieldContainer = &mScene->CreateSceneObject();
    mFieldContainer->GetTransform().SetPosition({mFieldLoweLeft.x, mFieldLoweLeft.y, 0.0f});
    mScene->GetRoot().AddChild(*mFieldContainer);
}

Pht::QuadMesh::Vertices GameScene::CreateFieldVertices() {
    auto width {mFieldWidth + fieldPadding};
    auto height {mFieldHeight + fieldPadding};

    return {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.3f, 0.3f, 0.79f, 1.0f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.8f, 0.225f, 0.425f, 1.0f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.3f, 0.3f, 0.79f, 1.0f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.81f, 0.225f, 0.425f, 1.0f}},
    };
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
                    blueprintZ
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
                                                     level.GetNumColumns());
    mPieceBlocks = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PieceBlocks,
                                                     *mFieldBlocksContainer);
    mGhostPieces = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::GhostPieces,
                                                     *mFieldBlocksContainer);
}

void GameScene::CreateEffectsContainer() {
    mEffectsContainer = &mScene->CreateSceneObject();
    mEffectsContainer->SetLayer(static_cast<int>(Layer::Effects));
    mFieldContainer->AddChild(*mEffectsContainer);
}

void GameScene::CreateFlyingBlocksContainer() {
    mFlyingBlocksContainer = &mScene->CreateSceneObject();
    mFlyingBlocksContainer->SetLayer(static_cast<int>(Layer::FlyingBlocks));
    mScene->GetRoot().AddChild(*mFlyingBlocksContainer);
}

void GameScene::CreateHud(const GameLogic& gameLogic,
                          const LevelResources& levelResources,
                          const PieceResources& pieceResources,
                          const Level& level) {
    mHudContainer = &mScene->CreateSceneObject();
    mHudContainer->SetLayer(static_cast<int>(Layer::Hud));
    mScene->GetRoot().AddChild(*mHudContainer);
    
    mHud = std::make_unique<GameHud>(mEngine,
                                     gameLogic,
                                     levelResources,
                                     pieceResources,
                                     mHudRectangles,
                                     mGameHudController,
                                     mCommonResources,
                                     *mScene,
                                     *mHudContainer,
                                     static_cast<int>(Layer::Hud),
                                     level);
}

void GameScene::CreateUiViewsContainer() {
    mUiViewsContainer = &mScene->CreateSceneObject();
    mUiViewsContainer->SetLayer(static_cast<int>(Layer::UiViews));
    mScene->GetRoot().AddChild(*mUiViewsContainer);
    
    auto& uiCameraSceneObject {mScene->CreateSceneObject()};
    uiCameraSceneObject.SetIsVisible(false);
    uiCameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 300.0f});
    auto uiCameraComponent {std::make_unique<Pht::CameraComponent>(uiCameraSceneObject)};
    
    auto* uiRenderPass {mScene->GetRenderPass(static_cast<int>(Layer::UiViews))};
    assert(uiRenderPass);
    uiRenderPass->SetCamera(uiCameraComponent.get());
    
    uiCameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(uiCameraComponent));
    mUiViewsContainer->AddChild(uiCameraSceneObject);
}

void GameScene::CreateStarsContainer() {
    mStarsContainer = &mScene->CreateSceneObject();
    mStarsContainer->SetLayer(static_cast<int>(Layer::Stars));
    mScene->GetRoot().AddChild(*mStarsContainer);
    
    auto& lightSceneObject {mScene->CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    lightComponent->SetDirection({0.8f, 0.8f, 1.0f});
    lightComponent->SetDirectionalIntensity(0.75f);
    
    auto* starsRenderPass {mScene->GetRenderPass(static_cast<int>(Layer::Stars))};
    assert(starsRenderPass);
    starsRenderPass->SetLight(lightComponent.get());
    
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    mStarsContainer->AddChild(lightSceneObject);
}

void GameScene::InitFieldBorder(const Level& level) {
    Pht::Vec3 leftBorderPosition {
        mFieldPosition.x - (mFieldWidth + fieldPadding) / 2.0f,
        mFieldPosition.y,
        fieldBorderZ
    };

    Pht::Vec3 rightBorderPosition {
        mFieldPosition.x + (mFieldWidth + fieldPadding) / 2.0f,
        mFieldPosition.y,
        fieldBorderZ
    };

    mFieldBorder.Init(leftBorderPosition, rightBorderPosition, level.GetNumRows());
}

void GameScene::Update() {
    mFloatingBlocks->Update();
    mHud->Update();
    UpdateLightAnimation();
    
    if (mScrollController.IsScrolling() || mCameraShake.IsShaking()) {
        UpdateCameraPosition();
    }
}

void GameScene::UpdateCameraPosition() {
    auto& renderer {mEngine.GetRenderer()};
    auto& frustumSize {renderer.GetOrthographicFrustumSize()};
    auto bottomPadding {renderer.GetBottomPaddingHeight()};
    
    auto cameraYPosition {
        mFieldLoweLeft.y + mScrollController.GetLowestVisibleRow() * mCellSize +
        frustumSize.y / 2.0f - mCellSize * lowerClipAreaHeightInCells - bottomPadding
    };

    auto& cameraShakeTranslation {mCameraShake.GetCameraTranslation()};
    Pht::Vec3 cameraPosition {0.0f, cameraYPosition, 20.5f};
    cameraPosition += cameraShakeTranslation;
    mCamera->GetSceneObject().GetTransform().SetPosition(cameraPosition);
    
    Pht::Vec3 target {0.0f, cameraYPosition, 0.0f};
    target += cameraShakeTranslation;
    Pht::Vec3 up {0.0f, 1.0f, 0.0f};
    mCamera->SetTarget(target, up);
    
    Pht::Vec2 scissorBoxLowerLeft {
        mFieldPosition.x -
        (mFieldWidth + fieldPadding + FieldBorder::borderThickness + scissorBoxPadding) / 2.0f,
        cameraYPosition - frustumSize.y / 2.0f + lowerClipAreaHeightInCells * mCellSize +
        bottomPadding
    };
    
    Pht::Vec2 scissorBoxSize {
        mFieldWidth + fieldPadding + FieldBorder::borderThickness + scissorBoxPadding,
        16.3f * mCellSize
    };

    Pht::ScissorBox scissorBox {scissorBoxLowerLeft, scissorBoxSize};
    SetScissorBox(scissorBox);
}

void GameScene::SetScissorBox(const Pht::ScissorBox& scissorBox) {
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

const Pht::ScissorBox& GameScene::GetFieldScissorBox() const {
    auto* fieldQuadRenderPass {mScene->GetRenderPass(static_cast<int>(Layer::FieldQuad))};
    assert(fieldQuadRenderPass);
    
    auto& scissorBox {fieldQuadRenderPass->GetScissorBox()};
    assert(scissorBox.HasValue());
    
    return scissorBox.GetValue();
}

void GameScene::UpdateLightAnimation() {
    mLightAnimationTime += mEngine.GetLastFrameSeconds();
    
    if (mLightAnimationTime > lightAnimationDuration) {
        mLightAnimationTime = 0.0f;
    }
    
    mLightAnimationTime = 2.5f;
    
    auto t {(cos(mLightAnimationTime * 2.0f * 3.1415f / lightAnimationDuration) + 1.0f) / 2.0f};
    mLight->SetDirection(lightDirectionA.Lerp(t, lightDirectionB));
}

const Pht::Material& GameScene::GameScene::GetGrayMaterial() const {
    return mCommonResources.GetMaterials().GetGrayFieldBlockMaterial();
}
