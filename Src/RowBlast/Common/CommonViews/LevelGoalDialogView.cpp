#include "LevelGoalDialogView.hpp"

// Engine includes.
#include "TextComponent.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "PieceResources.hpp"
#include "UiLayer.hpp"
#include "GradientRectangle.hpp"

using namespace RowBlast;

namespace {
    constexpr auto cellSize {1.25f};
    constexpr auto previewPieceSpacing {2.0f};
    constexpr auto numPieceTypeRows {2};
    constexpr auto bombAnimationDuration {4.5f};
    constexpr auto bombRotationAmplitude {22.0f};
    constexpr auto rowBombRotationSpeed {35.0f};
    constexpr auto emissiveAnimationDuration {1.5f};
    constexpr auto emissiveAmplitude {1.7f};
    constexpr auto asteroidRotationSpeed {35.0f};
    const Pht::Vec3 captionPosition {-1.7f, 8.25f, UiLayer::text};
}

LevelGoalDialogView::LevelGoalDialogView(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const PieceResources& pieceResources,
                                         Scene scene) :
    mEngine {engine},
    mPieceResources {pieceResources} {
    
    auto zoom {scene == Scene::Game ? PotentiallyZoomedScreen::Yes : PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    mCaption = &CreateText(captionPosition, "LEVEL 1", largeTextProperties);
    
    if (scene == Scene::Map) {
        Pht::Vec3 closeButtonPosition {
            GetSize().x / 2.0f - 1.3f,
            GetSize().y / 2.0f - 1.3f,
            UiLayer::textRectangle
        };

        Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

        MenuButton::Style closeButtonStyle;
        closeButtonStyle.mPressedScale = 1.05f;
        closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
        
        mCloseButton = std::make_unique<MenuButton>(engine,
                                                    *this,
                                                    closeButtonPosition,
                                                    closeButtonInputSize,
                                                    closeButtonStyle);
    }
    
    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);

    CreateText({-1.45f, 5.9f, UiLayer::text}, "PIECES", largeTextProperties);
    
    auto& lineSceneObject2 {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject2.GetTransform().SetPosition({0.0f, 0.5f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject2);

    if (scene == Scene::Map) {
        Pht::Vec2 playButtonInputSize {205.0f, 59.0f};
        
        MenuButton::Style playButtonStyle;
        playButtonStyle.mPressedScale = 1.05f;
        playButtonStyle.mRenderableObject = &guiResources.GetLargeBlueGlossyButton(zoom);
        playButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkBlueGlossyButton(zoom);

        mPlayButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -7.6f, UiLayer::textRectangle},
                                                   playButtonInputSize,
                                                   playButtonStyle);
        mPlayButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                                "PLAY",
                                largeTextProperties);
    } else {
        Pht::Vec2 backButtonInputSize {194.0f, 43.0f};
        
        MenuButton::Style backButtonStyle;
        backButtonStyle.mPressedScale = 1.05f;
        backButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
        backButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);

        
        mBackButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -7.7f, UiLayer::textRectangle},
                                                   backButtonInputSize,
                                                   backButtonStyle);
        mBackButton->CreateText({-0.81f, -0.23f, UiLayer::buttonText},
                                "Back",
                                guiResources.GetSmallWhiteTextProperties(zoom));
    }

    CreatePreviewPiecesContainer(engine);
    CreateGoalContainer(engine, commonResources, zoom);
}

void LevelGoalDialogView::CreatePreviewPiecesContainer(Pht::IEngine& engine) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, 3.3f, UiLayer::block});
    
    for (auto& previewPiece: mPreviewPieces) {
        previewPiece.mBlockSceneObjects =
            std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks, container);
        previewPiece.mBlockSceneObjects->SetIsActive(false);
        
        auto& blocksTransform {
            previewPiece.mBlockSceneObjects->GetContainerSceneObject().GetTransform()
        };

        blocksTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    }
    
    CreateGlowEffectsBehindPieces(container, engine);
    GetRoot().AddChild(container);
}

void LevelGoalDialogView::CreateGlowEffectsBehindPieces(Pht::SceneObject& parentObject,
                                                        Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.5f, 0.5f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare03.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 00.0f,
        .mSize = Pht::Vec2{15.0f, 15.0f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f,
        .mGrowDuration = 0.75f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Triangles);
    auto& material {mGlowEffect->GetRenderable()->GetMaterial()};
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    mGlowEffect->GetTransform().SetPosition({0.0f, 0.1f, UiLayer::buttonText});
    parentObject.AddChild(*mGlowEffect);
    
    Pht::ParticleSettings particleSettings2 {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.6f, 0.0f, 0.75f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 20.0f,
        .mSize = Pht::Vec2{22.0f, 22.0f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f,
        .mGrowDuration = 0.2f
    };
    
    mRoundGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings2,
                                                                      particleEmitterSettings,
                                                                      Pht::RenderMode::Triangles);
    auto& material2 {mRoundGlowEffect->GetRenderable()->GetMaterial()};
    material2.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    mRoundGlowEffect->GetTransform().SetPosition({0.0f, 0.1f, -0.9f});
    parentObject.AddChild(*mRoundGlowEffect);
}

void LevelGoalDialogView::CreateGoalContainer(Pht::IEngine& engine,
                                              const CommonResources& commonResources,
                                              PotentiallyZoomedScreen zoom) {
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition({0.0f, -3.0f, 0.0f});

    auto& guiResources {commonResources.GetGuiResources()};
    auto& smallTextProperties {guiResources.GetSmallWhiteTextProperties(zoom)};

    CreateText({-1.3f, 2.0f, UiLayer::text},
               "GOAL",
               guiResources.GetLargeWhiteTextProperties(zoom),
               container);

    mClearObjectiveSceneObject = &CreateSceneObject();
    CreateText({-3.6f, 0.8f, UiLayer::text},
               "Clear all gray blocks",
               smallTextProperties,
               *mClearObjectiveSceneObject);
    CreateGrayCube(engine, commonResources, *mClearObjectiveSceneObject);
    mClearObjectiveSceneObject->SetIsVisible(false);
    container.AddChild(*mClearObjectiveSceneObject);

    mAsteroidObjectiveSceneObject = &CreateSceneObject();
    CreateText({-4.0f, 0.8f, UiLayer::text},
               "Bring down the asteroid",
               smallTextProperties,
               *mAsteroidObjectiveSceneObject);
    CreateAsteroid(engine, *mAsteroidObjectiveSceneObject);
    mAsteroidObjectiveSceneObject->SetIsVisible(false);
    container.AddChild(*mAsteroidObjectiveSceneObject);

    mBuildObjectiveSceneObject = &CreateSceneObject();
    CreateText({-2.95f, 0.8f, UiLayer::text},
               "Fill all gray slots",
               smallTextProperties,
               *mBuildObjectiveSceneObject);
    CreateBlueprintSlot(engine, *mBuildObjectiveSceneObject);
    mBuildObjectiveSceneObject->SetIsVisible(false);
    container.AddChild(*mBuildObjectiveSceneObject);
    
    CreateGlowEffectsBehindGoal(container, engine);
    GetRoot().AddChild(container);
}

void LevelGoalDialogView::CreateGrayCube(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         Pht::SceneObject& parent) {
    auto& sceneManager {engine.GetSceneManager()};
    
    auto& grayCube {
        CreateSceneObject(Pht::ObjMesh {"cube_428.obj", 1.25f},
                          commonResources.GetMaterials().GetGrayFieldBlockMaterial(),
                          sceneManager)
    };
    
    auto& transform {grayCube.GetTransform()};
    transform.SetPosition({0.0f, -1.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.85f);
    
    parent.AddChild(grayCube);
}

void LevelGoalDialogView::CreateAsteroid(Pht::IEngine& engine, Pht::SceneObject& parent) {
    Pht::Material asteroidMaterial {"gray_asteroid.jpg", 0.84f, 1.23f, 0.0f, 1.0f};
    asteroidMaterial.GetDepthState().mDepthTestAllowedOverride = true;
    
    auto& sceneManager {engine.GetSceneManager()};
    
    mAsteroidSceneObject =
        &CreateSceneObject(Pht::ObjMesh {"asteroid_2000.obj", 19.0f, Pht::MoveMeshToOrigin::Yes},
                           asteroidMaterial,
                           sceneManager);
    
    auto& transform {mAsteroidSceneObject->GetTransform()};
    transform.SetPosition({0.0f, -1.0f, UiLayer::block});
    transform.SetScale(1.95f);
    
    parent.AddChild(*mAsteroidSceneObject);
}

void LevelGoalDialogView::CreateBlueprintSlot(Pht::IEngine& engine, Pht::SceneObject& parent) {
    auto& blueprintSlotContainer {CreateSceneObject()};
    auto& transform {blueprintSlotContainer.GetTransform()};
    transform.SetPosition({-0.05f, -1.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    parent.AddChild(blueprintSlotContainer);

    auto squareSide {1.25f};
    auto slotSide {squareSide + 0.125f};
    Pht::Vec4 slotFillColor {1.0f, 1.0f, 1.0f, 0.192f};
    auto& sceneManager {engine.GetSceneManager()};
    
    Pht::QuadMesh::Vertices blueprintSlotVertices  {
        {{-slotSide / 2.0f, -slotSide / 2.0f, 0.0f}, slotFillColor},
        {{slotSide / 2.0f, -slotSide / 2.0f, 0.0f}, slotFillColor},
        {{slotSide / 2.0f, slotSide / 2.0f, 0.0f}, slotFillColor},
        {{-slotSide / 2.0f, slotSide / 2.0f, 0.0f}, slotFillColor},
    };

    Pht::Material slotMaterial;
    slotMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& blueprintSlot {
        CreateSceneObject(Pht::QuadMesh {blueprintSlotVertices}, slotMaterial, sceneManager)
    };
    
    blueprintSlotContainer.AddChild(blueprintSlot);

    auto f {0.9125f};
    Pht::Vec4 fieldColor {0.3f * f, 0.3f * f, 0.752f * f, 1.0f};

    Pht::QuadMesh::Vertices fieldCellVertices  {
        {{-squareSide / 2.0f, -squareSide / 2.0f, 0.0f}, fieldColor},
        {{squareSide / 2.0f, -squareSide / 2.0f, 0.0f}, fieldColor},
        {{squareSide / 2.0f, squareSide / 2.0f, 0.0f}, fieldColor},
        {{-squareSide / 2.0f, squareSide / 2.0f, 0.0f}, fieldColor},
    };

    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.96f);
    
    auto& fieldCell {
        CreateSceneObject(Pht::QuadMesh {fieldCellVertices}, fieldMaterial, sceneManager)
    };
    
    fieldCell.GetTransform().SetPosition({0.0f, 0.0f, -0.005f});
    blueprintSlotContainer.AddChild(fieldCell);
}

void LevelGoalDialogView::CreateGlowEffectsBehindGoal(Pht::SceneObject& parentObject,
                                                      Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 2
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.46f, 0.2f, 0.65f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 20.0f,
        .mZAngularVelocityRandomPart = 10.0f,
        .mSize = Pht::Vec2{11.25f, 11.25f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f,
        .mGrowDuration = 0.5f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mGoalRoundGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                          particleEmitterSettings,
                                                                          Pht::RenderMode::Triangles);
    auto& material {mGoalRoundGlowEffect->GetRenderable()->GetMaterial()};
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    mGoalRoundGlowEffect->GetTransform().SetPosition({0.0f, -1.0, -1.9f});
    parentObject.AddChild(*mGoalRoundGlowEffect);
}

void LevelGoalDialogView::Init(const LevelInfo& levelInfo) {
    mCaption->GetText() = "LEVEL " + std::to_string(levelInfo.mId);
    
    auto adjustedCaptionPosition {captionPosition};
    
    if (levelInfo.mId > 19) {
        adjustedCaptionPosition.x -= 0.31f;
    } else if (levelInfo.mId > 9) {
        adjustedCaptionPosition.x -= 0.17f;
    }
    
    mCaption->GetSceneObject().GetTransform().SetPosition(adjustedCaptionPosition);
    
    mClearObjectiveSceneObject->SetIsVisible(false);
    mAsteroidObjectiveSceneObject->SetIsVisible(false);
    mBuildObjectiveSceneObject->SetIsVisible(false);
    
    switch (levelInfo.mObjective) {
        case Level::Objective::Clear:
            mClearObjectiveSceneObject->SetIsVisible(true);
            break;
        case Level::Objective::BringDownTheAsteroid:
            mAsteroidObjectiveSceneObject->SetIsVisible(true);
            break;
        case Level::Objective::Build:
            mBuildObjectiveSceneObject->SetIsVisible(true);
            break;
    }
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mGoalRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    
    for (auto& previewPiece: mPreviewPieces) {
        previewPiece.mBlockSceneObjects->SetIsActive(false);
    }
    
    auto numPieceTypes {levelInfo.mPieceTypes.size()};
    auto numPieceTypesUpperRow {
        static_cast<int>(std::ceil(static_cast<float>(numPieceTypes) /
                                   static_cast<float>(numPieceTypeRows)))
    };
    
    Pht::Vec3 previewPiecePosition {
        -previewPieceSpacing * numPieceTypesUpperRow / 2.0f + previewPieceSpacing / 2.0f,
        previewPieceSpacing / 2.0f,
        0.0f
    };
    
    for (auto i {0}; i < numPieceTypesUpperRow; ++i) {
        auto& previewPiece {mPreviewPieces[i]};
        auto* pieceType {levelInfo.mPieceTypes[i]};
        InitPreviewPiece(previewPiece, *pieceType, previewPiecePosition);
        previewPiecePosition.x += previewPieceSpacing;
    }
    
    auto numPieceTypesLowerRow {numPieceTypes - numPieceTypesUpperRow};

    previewPiecePosition = {
        -previewPieceSpacing * numPieceTypesLowerRow / 2.0f + previewPieceSpacing / 2.0f,
        -previewPieceSpacing / 2.0f,
        0.0f
    };

    for (auto i {numPieceTypesUpperRow}; i < numPieceTypes; ++i) {
        auto& previewPiece {mPreviewPieces[i]};
        auto* pieceType {levelInfo.mPieceTypes[i]};
        InitPreviewPiece(previewPiece, *pieceType, previewPiecePosition);
        previewPiecePosition.x += previewPieceSpacing;
    }

    mAnimationTime = 0.0f;
    mEmissiveAnimationTime = 0.0f;
    mRowBombRotation = {0.0f, 0.0f, 0.0f};
    mAsteroidRotation = {0.0f, 0.0f, 0.0f};
}

void LevelGoalDialogView::InitPreviewPiece(LevelStartPreviewPiece& previewPiece,
                                           const Piece& pieceType,
                                           const Pht::Vec3& position) {
    previewPiece.mBombSceneObject = nullptr;
    previewPiece.mRowBombSceneObject = nullptr;

    auto& containerObject {previewPiece.mBlockSceneObjects->GetContainerSceneObject()};
    auto& baseTransform {containerObject.GetTransform()};
    baseTransform.SetPosition(position);

    previewPiece.mBlockSceneObjects->SetIsActive(true);
    
    auto scale {pieceType.GetPreviewCellSize() / cellSize};
    
    baseTransform.SetScale(scale);
    previewPiece.mBlockSceneObjects->ReclaimAll();
    
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    auto& grid {pieceType.GetGrid(Rotation::Deg0)};
    auto isBomb {pieceType.IsBomb()};
    auto isRowBomb {pieceType.IsRowBomb()};
    
    Pht::Vec3 lowerLeft {
        -static_cast<float>(pieceNumColumns) * cellSize / 2.0f + cellSize / 2.0f,
        -static_cast<float>(pieceNumRows) * cellSize / 2.0f + cellSize / 2.0f,
        0.0f
    };
    
    if (pieceType.NeedsUpAdjustmentInHud()) {
        lowerLeft.y += cellSize / 2.0f;
    } else if (pieceType.NeedsDownAdjustmentInHud()) {
        lowerLeft.y -= cellSize / 2.0f;
    }
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto& subCell {grid[row][column].mFirstSubCell};
            auto blockKind {subCell.mBlockKind};
 
            if (blockKind != BlockKind::None) {
                auto& blockSceneObject {previewPiece.mBlockSceneObjects->AccuireSceneObject()};
                
                if (isBomb) {
                    blockSceneObject.SetRenderable(&mPieceResources.GetBombRenderableObject());
                    previewPiece.mBombSceneObject = &blockSceneObject;
                } else if (isRowBomb) {
                    blockSceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
                    previewPiece.mRowBombSceneObject = &blockSceneObject;
                } else {
                    auto& blockRenderable {
                        mPieceResources.GetBlockRenderableObject(blockKind,
                                                                 subCell.mColor,
                                                                 BlockBrightness::Normal)
                    };
                    
                    blockSceneObject.SetRenderable(&blockRenderable);
                }
                
                Pht::Vec3 position {
                    static_cast<float>(column) * cellSize,
                    static_cast<float>(row) * cellSize,
                    0.0f
                };

                blockSceneObject.GetTransform().SetPosition(lowerLeft + position);
            }
        }
    }
}

void LevelGoalDialogView::StartEffects() {
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mGoalRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
}

void LevelGoalDialogView::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mGoalRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    
    UpdateAnimations(dt);
}

void LevelGoalDialogView::UpdateAnimations(float dt) {
    mAnimationTime += dt;
    
    if (mAnimationTime > bombAnimationDuration) {
        mAnimationTime = 0.0f;
    }

    AnimateEmissive(dt);
    AnimateBombRotation(dt);
    AnimateRowBombRotation(dt);
    AnimateAsteroidRotation(dt);
}

void LevelGoalDialogView::AnimateEmissive(float dt) {
    mEmissiveAnimationTime += dt;
    
    if (mEmissiveAnimationTime > emissiveAnimationDuration) {
        mEmissiveAnimationTime = 0.0f;
    }

    auto sineOfT {sin(mEmissiveAnimationTime * 2.0f * 3.1415f / emissiveAnimationDuration)};
    auto emissive {emissiveAmplitude * (sineOfT + 1.0f) / 2.0f};
    
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mPieceResources.GetBombRenderableObject(),
                                                   emissive);
    Pht::SceneObjectUtils::SetEmissiveInRenderable(mPieceResources.GetRowBombRenderableObject(),
                                                   emissive);
}

void LevelGoalDialogView::AnimateBombRotation(float dt) {
    auto t {mAnimationTime * 2.0f * 3.1415f / bombAnimationDuration};
    auto xAngle {bombRotationAmplitude * sin(t) + 90.0f};
    auto yAngle {bombRotationAmplitude * cos(t)};
    
    for (auto& previewPiece: mPreviewPieces) {
        if (previewPiece.mBlockSceneObjects->IsActive() && previewPiece.mBombSceneObject) {
            previewPiece.mBombSceneObject->GetTransform().SetRotation({xAngle, yAngle, 0.0f});
        }
    }
}

void LevelGoalDialogView::AnimateRowBombRotation(float dt) {
    mRowBombRotation.y += rowBombRotationSpeed * dt;
    
    if (mRowBombRotation.y > 360.0f) {
        mRowBombRotation.y -= 360.0f;
    }
    
    for (auto& previewPiece: mPreviewPieces) {
        if (previewPiece.mBlockSceneObjects->IsActive() && previewPiece.mRowBombSceneObject) {
            previewPiece.mRowBombSceneObject->GetTransform().SetRotation(mRowBombRotation);
        }
    }
}

void LevelGoalDialogView::AnimateAsteroidRotation(float dt) {
    mAsteroidRotation.y += asteroidRotationSpeed * dt;
    
    if (mAsteroidRotation.y > 360.0f) {
        mAsteroidRotation.y -= 360.0f;
    }
    
    mAsteroidSceneObject->GetTransform().SetRotation(mAsteroidRotation);
}
