#include "LevelStartDialogView.hpp"

// Engine includes.
#include "TextComponent.hpp"
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "CommonResources.hpp"
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
}

LevelStartDialogView::LevelStartDialogView(Pht::IEngine& engine,
                                           const CommonResources& commonResources,
                                           PieceResources& pieceResources) :
    mEngine {engine},
    mPieceResources {pieceResources} {

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    mCaption = &CreateText({-2.1f, 7.6f, UiLayer::text}, "LEVEL 1", largeTextProperties);

    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.5f,
        GetSize().y / 2.0f - 1.5f,
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

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, 6.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);

    CreateText({-1.45f, 5.0f, UiLayer::text}, "PIECES", largeTextProperties);
    
    auto& lineSceneObject2 {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject2.GetTransform().SetPosition({0.0f, -0.5f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject2);

    CreateText({-1.3f, -1.9f, UiLayer::text}, "GOAL", largeTextProperties);

    auto& smallTextProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    mClearObjective = &CreateText({-3.6f, -3.1f, UiLayer::text},
                                  "Clear all gray blocks",
                                  smallTextProperties);
    mClearObjective->GetSceneObject().SetIsVisible(false);
    mBuildObjective = &CreateText({-2.95f, -3.1f, UiLayer::text},
                                  "Fill all gray slots",
                                  smallTextProperties);
    mBuildObjective->GetSceneObject().SetIsVisible(false);

    Pht::Vec2 playButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style playButtonStyle;
    playButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    playButtonStyle.mColor = GuiResources::mBlueButtonColor;
    playButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    playButtonStyle.mPressedScale = 1.05f;

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -6.0f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "PLAY",
                            largeTextProperties);
    
    CreatePreviewPiecesContainer(engine);
}

void LevelStartDialogView::CreatePreviewPiecesContainer(Pht::IEngine& engine) {
    auto container {std::make_unique<Pht::SceneObject>()};
    container->GetTransform().SetPosition({0.0f, 2.3f, UiLayer::block});
    
    for (auto& previewPiece: mPreviewPieces) {
        previewPiece.mBlockSceneObjects =
            std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks, *container);
        previewPiece.mBlockSceneObjects->SetIsActive(false);
        
        auto& blocksTransform {
            previewPiece.mBlockSceneObjects->GetContainerSceneObject().GetTransform()
        };

        blocksTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    }
    
    CreateGlowEffects(*container, engine);
    AddSceneObject(std::move(container));
}

void LevelStartDialogView::CreateGlowEffects(Pht::SceneObject& parentObject, Pht::IEngine& engine) {
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
        .mZAngularVelocity = 0.0f,
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

void LevelStartDialogView::Init(const LevelInfo& levelInfo) {
    mCaption->GetText() = "LEVEL " + std::to_string(levelInfo.mId);
    
    mClearObjective->GetSceneObject().SetIsVisible(false);
    mBuildObjective->GetSceneObject().SetIsVisible(false);
    
    switch (levelInfo.mObjective) {
        case Level::Objective::Clear:
            mClearObjective->GetSceneObject().SetIsVisible(true);
            break;
        case Level::Objective::Build:
            mBuildObjective->GetSceneObject().SetIsVisible(true);
            break;
    }
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    mRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    
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
}

void LevelStartDialogView::InitPreviewPiece(LevelStartPreviewPiece& previewPiece,
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

void LevelStartDialogView::StartEffects() {
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
    mRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
}

void LevelStartDialogView::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mRoundGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    
    UpdateAnimations(dt);
}

void LevelStartDialogView::UpdateAnimations(float dt) {
    mAnimationTime += dt;
    
    if (mAnimationTime > bombAnimationDuration) {
        mAnimationTime = 0.0f;
    }

    AnimateEmissive(dt);
    AnimateBombRotation(dt);
    AnimateRowBombRotation(dt);
}

void LevelStartDialogView::AnimateEmissive(float dt) {
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

void LevelStartDialogView::AnimateBombRotation(float dt) {
    auto t {mAnimationTime * 2.0f * 3.1415f / bombAnimationDuration};
    auto xAngle {bombRotationAmplitude * sin(t) + 90.0f};
    auto yAngle {bombRotationAmplitude * cos(t)};
    
    for (auto& previewPiece: mPreviewPieces) {
        if (previewPiece.mBlockSceneObjects->IsActive() && previewPiece.mBombSceneObject) {
            previewPiece.mBombSceneObject->GetTransform().SetRotation({xAngle, yAngle, 0.0f});
        }
    }
}

void LevelStartDialogView::AnimateRowBombRotation(float dt) {
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
