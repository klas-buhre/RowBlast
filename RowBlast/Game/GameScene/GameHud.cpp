#include "GameHud.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GradientRectangle.hpp"
#include "RoundedCylinder.hpp"
#include "GameLogic.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"
#include "GameHudController.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 lightDirectionA {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 lightDirectionB {0.4f, 1.0f, 1.0f};
    constexpr auto lightAnimationDuration {5.0f};
    const Pht::Color roundedCylinderAmbient {0.75f, 0.75f, 0.75f};
    const Pht::Color roundedCylinderDiffuse {0.55f, 0.55f, 0.55f};
    constexpr auto roundedCylinderOpacity {0.5f};
    constexpr auto cellSize {1.25f};
}

GameHud::GameHud(Pht::IEngine& engine,
                 const GameLogic& gameLogic,
                 const LevelResources& levelResources,
                 const PieceResources& pieceResources,
                 GameHudController& gameHudController,
                 const CommonResources& commonResources,
                 Pht::Scene& scene,
                 Pht::SceneObject& parentObject,
                 int hudLayer,
                 const Level& level) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mPieceResources {pieceResources},
    mLevelObjective {level.GetObjective()},
    mPreviewPieceRelativePositions {
        Pht::Vec3{-2.03f, 0.01f, UiLayer::block},
        Pht::Vec3{-0.73f, 0.01f, UiLayer::block},
        Pht::Vec3{1.27f, 0.01f, UiLayer::block},
        Pht::Vec3{2.57f, 0.01f, UiLayer::block}
    } {
    
    mNext2PiecesPreviousFrame.fill(nullptr);
    mSelectablePiecesPreviousFrame.fill(nullptr);
    
    gameHudController.SetHudEventListener(*this);

    CreateLightAndCamera(scene, parentObject, hudLayer);

    Pht::TextProperties upperTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };

    CreateProgressObject(scene, parentObject, upperTextProperties, levelResources);
    CreateMovesObject(scene, parentObject, upperTextProperties);
    
    Pht::TextProperties lowerTextProperties {
        commonResources.GetHussarFontSize22(PotentiallyZoomedScreen::Yes),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    CreateNextPiecesObject(scene, parentObject, lowerTextProperties);
    CreateSelectablePiecesObject(scene, parentObject, lowerTextProperties);
}

void GameHud::CreateLightAndCamera(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   int hudLayer) {
    auto& lightSceneObject {scene.CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    mLight = lightComponent.get();
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    parentObject.AddChild(lightSceneObject);
    
    auto& cameraSceneObject {scene.CreateSceneObject()};
    cameraSceneObject.SetIsVisible(false);
    cameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 15.5f});
    auto cameraComponent {std::make_unique<Pht::CameraComponent>(cameraSceneObject)};
    auto* camera {cameraComponent.get()};
    cameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(cameraComponent));
    parentObject.AddChild(cameraSceneObject);
    
    auto* hudRenderPass {scene.GetRenderPass(hudLayer)};
    assert(hudRenderPass);
    hudRenderPass->SetLight(mLight);
    hudRenderPass->SetCamera(camera);
}

void GameHud::CreateProgressObject(Pht::Scene& scene,
                                   Pht::SceneObject& parentObject,
                                   const Pht::TextProperties& textProperties,
                                   const LevelResources& levelResources) {
    auto& progressContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    
    Pht::Vec3 position {
        -3.4f,
        renderer.GetHudFrustumSize().y / 2.0f - renderer.GetTopPaddingHeight() - 0.87f,
        UiLayer::root
    };
    
    mProgressContainer = &progressContainer;
    progressContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(progressContainer);
    
    Pht::Vec3 cylinderPosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateRoundedCylinder(scene,
                          progressContainer,
                          cylinderPosition,
                          {2.5, 1.1f},
                          roundedCylinderOpacity,
                          roundedCylinderAmbient,
                          roundedCylinderDiffuse);
    
    std::string text {"    "};  // Warning! Must be four spaces to fit digits.
    mProgressText = &scene.CreateText(text, textProperties);
    auto& progressTextSceneobject {mProgressText->GetSceneObject()};
    progressTextSceneobject.GetTransform().SetPosition({-0.25f, -0.3f, UiLayer::text});
    progressContainer.AddChild(progressTextSceneobject);
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            CreateGrayBlock(scene, progressContainer, levelResources);
            break;
        case Level::Objective::Build:
            CreateBlueprintSlot(scene, progressContainer, levelResources);
            break;
    }
    
    Pht::SceneObjectUtils::ScaleRecursively(*mProgressContainer, 1.1f);
}

void GameHud::CreateGrayBlock(Pht::Scene& scene,
                              Pht::SceneObject& progressContainer,
                              const LevelResources& levelResources) {
    auto& grayBlock {scene.CreateSceneObject()};
    grayBlock.SetRenderable(&levelResources.GetLevelBlockRenderable(BlockKind::Full));
    
    auto& transform {grayBlock.GetTransform()};
    transform.SetPosition({-0.95f, 0.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.505f);
    
    progressContainer.AddChild(grayBlock);
}

void GameHud::CreateBlueprintSlot(Pht::Scene& scene,
                                  Pht::SceneObject& progressContainer,
                                  const LevelResources& levelResources) {
    auto& blueprintSlot {scene.CreateSceneObject()};
    blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotRenderable());
    
    auto& transform {blueprintSlot.GetTransform()};
    transform.SetPosition({-0.85f, 0.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.56f);
    
    progressContainer.AddChild(blueprintSlot);
}

void GameHud::CreateMovesObject(Pht::Scene& scene,
                                Pht::SceneObject& parentObject,
                                const Pht::TextProperties& textProperties) {
    auto& movesContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    
    Pht::Vec3 position {
        3.4f,
        renderer.GetHudFrustumSize().y / 2.0f - renderer.GetTopPaddingHeight() - 0.87f,
        UiLayer::root
    };

    mMovesContainer = &movesContainer;
    movesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(movesContainer);
    
    Pht::Vec3 cylinderPosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateRoundedCylinder(scene,
                          movesContainer,
                          cylinderPosition,
                          {2.5, 1.1f},
                          roundedCylinderOpacity,
                          roundedCylinderAmbient,
                          roundedCylinderDiffuse);
    
    std::string text {"   "};   // Warning! Must be three spaces to fit digits.
    mMovesText = &scene.CreateText(text, textProperties);
    auto& movesTextSceneobject {mMovesText->GetSceneObject()};
    movesTextSceneobject.GetTransform().SetPosition({-0.05f, -0.3f, UiLayer::text});
    movesContainer.AddChild(movesTextSceneobject);
    
    CreateLPiece(scene, movesContainer);
    
    Pht::SceneObjectUtils::ScaleRecursively(*mMovesContainer, 1.1f);
}

void GameHud::CreateLPiece(Pht::Scene& scene, Pht::SceneObject& movesContainer) {
    auto& lPiece {scene.CreateSceneObject()};
    movesContainer.AddChild(lPiece);
    
    auto& baseTransform {lPiece.GetTransform()};
    baseTransform.SetPosition({-0.95f, 0.05f, UiLayer::root});
    baseTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.32f};
    baseTransform.SetScale(scale);
    
    auto& blockRenderable {
        mPieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                 BlockColor::Green,
                                                 BlockBrightness::Normal)
    };
    
    auto halfCellSize {0.625f};
    CreateGreenBlock({-halfCellSize, -halfCellSize, -scale}, blockRenderable, scene, lPiece);
    CreateGreenBlock({halfCellSize, -halfCellSize, -scale}, blockRenderable, scene, lPiece);
    CreateGreenBlock({halfCellSize, halfCellSize, -scale}, blockRenderable, scene, lPiece);
}

void GameHud::CreateGreenBlock(const Pht::Vec3& position,
                               Pht::RenderableObject& blockRenderable,
                               Pht::Scene& scene,
                               Pht::SceneObject& lPiece) {
    auto& block {scene.CreateSceneObject()};
    block.GetTransform().SetPosition(position);
    block.SetRenderable(&blockRenderable);
    lPiece.AddChild(block);
}

void GameHud::CreateNextPiecesObject(Pht::Scene& scene,
                                     Pht::SceneObject& parentObject,
                                     const Pht::TextProperties& textProperties) {
    auto& nextPiecesContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    
    Pht::Vec3 position {
        -2.3f,
        -renderer.GetHudFrustumSize().y / 2.0f + renderer.GetBottomPaddingHeight() + 1.0125f,
        UiLayer::root
    };

    mNextPiecesContainer = &nextPiecesContainer;
    nextPiecesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(nextPiecesContainer);
    
    CreatePiecesRectangle({0.15f, 0.0f, UiLayer::piecesRectangle}, false, scene, nextPiecesContainer);
    
    Pht::Vec3 textRectanglePosition {0.55f, 1.42f, UiLayer::textRectangle};
    CreateTextRectangle(textRectanglePosition, 4.8f, false, scene, nextPiecesContainer);
    
    auto& text {scene.CreateText("NEXT", textProperties)};
    auto& textSceneObject {text.GetSceneObject()};
    textSceneObject.GetTransform().SetPosition({-0.1f, 1.22f, UiLayer::text});
    nextPiecesContainer.AddChild(textSceneObject);
    
    CreateThreePreviewPieces(mNextPreviewPieces, nextPiecesContainer);
}

void GameHud::CreateSelectablePiecesObject(Pht::Scene& scene,
                                           Pht::SceneObject& parentObject,
                                           const Pht::TextProperties& textProperties) {
    auto& selectablePiecesContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    
    Pht::Vec3 position {
        3.1f,
        -renderer.GetHudFrustumSize().y / 2.0f + renderer.GetBottomPaddingHeight() + 1.0125f,
        UiLayer::root
    };

    mSelectablePiecesContainer = &selectablePiecesContainer;
    selectablePiecesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(selectablePiecesContainer);
    
    mSelectablePiecesRectangle = &CreatePiecesRectangle({0.15f, 0.0f, UiLayer::piecesRectangle},
                                                        false,
                                                        scene,
                                                        selectablePiecesContainer);
    mSwitchTextRectangle = &CreateTextRectangle({0.55f, 1.42f, UiLayer::textRectangle}, 4.8f,
                                                false,
                                                scene,
                                                selectablePiecesContainer);
    mBrightSelectablePiecesRectangle = &CreatePiecesRectangle({0.15f, 0.0f, UiLayer::piecesRectangle},
                                                              true,
                                                              scene,
                                                              selectablePiecesContainer);
    mBrightSwitchTextRectangle = &CreateTextRectangle({0.55f, 1.42f, UiLayer::textRectangle}, 4.8f,
                                                      true,
                                                      scene,
                                                      selectablePiecesContainer);
    mBrightSelectablePiecesRectangle->SetIsVisible(false);
    mBrightSwitchTextRectangle->SetIsVisible(false);

    auto& text {scene.CreateText("SWITCH", textProperties)};
    auto& textSceneObject {text.GetSceneObject()};
    textSceneObject.GetTransform().SetPosition({-0.55f, 1.22f, UiLayer::text});
    selectablePiecesContainer.AddChild(textSceneObject);
    
    CreateThreePreviewPieces(mSelectablePreviewPieces, selectablePiecesContainer);
}

Pht::SceneObject& GameHud::CreateTextRectangle(const Pht::Vec3& position,
                                               float length,
                                               bool isBright,
                                               Pht::Scene& scene,
                                               Pht::SceneObject& parentObject) {
    Pht::Vec2 size {length, 0.7f};
    auto leftQuadWidth {1.0f};
    auto rightQuadWidth {1.0f};
    
    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {isBright ? 0.7f : 0.6f, isBright ? 0.5f : 0.3f, isBright ? 0.85f : 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };

    return CreateGradientRectangle(scene,
                                   parentObject,
                                   position,
                                   size,
                                   0.0f,
                                   leftQuadWidth,
                                   rightQuadWidth,
                                   colors,
                                   colors);
}

Pht::SceneObject& GameHud::CreatePiecesRectangle(const Pht::Vec3& position,
                                                 bool isBright,
                                                 Pht::Scene& scene,
                                                 Pht::SceneObject& parentObject) {
    Pht::Vec2 size {4.8f, 2.2f};
    auto tilt {0.50f};
    auto leftQuadWidth {0.4f};
    auto rightQuadWidth {0.4f};
    
    GradientRectangleColors upperColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {isBright ? 0.95f : 0.85f, isBright ? 0.6f : 0.4f, 0.95f, 0.93f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };

    GradientRectangleColors lowerColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.9f, 0.9f, 1.0f, 0.0f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };
    
    return CreateGradientRectangle(scene,
                                   parentObject,
                                   position,
                                   size,
                                   tilt,
                                   leftQuadWidth,
                                   rightQuadWidth,
                                   upperColors,
                                   lowerColors);
}

void GameHud::CreateThreePreviewPieces(ThreePreviewPieces& previewPieces,
                                       Pht::SceneObject& parentObject) {
    for (auto i {0}; i < previewPieces.size(); ++i) {
        auto& piece {previewPieces[i]};
        
        piece.mSceneObjects = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks,
                                                                parentObject);
        piece.mSceneObjects->SetIsActive(false);
        
        auto& transform {piece.mSceneObjects->GetContainerSceneObject().GetTransform()};
        transform.SetPosition(mPreviewPieceRelativePositions[i]);
        transform.SetRotation({-30.0f, -30.0f, 0.0f});
    }
}

void GameHud::OnSwitchButtonDown() {
    mBrightSelectablePiecesRectangle->SetIsVisible(true);
    mBrightSwitchTextRectangle->SetIsVisible(true);
    mSelectablePiecesRectangle->SetIsVisible(false);
    mSwitchTextRectangle->SetIsVisible(false);
}

void GameHud::OnSwitchButtonUp() {
    mBrightSelectablePiecesRectangle->SetIsVisible(false);
    mBrightSwitchTextRectangle->SetIsVisible(false);
    mSelectablePiecesRectangle->SetIsVisible(true);
    mSwitchTextRectangle->SetIsVisible(true);
}

void GameHud::Update() {
    UpdateLightAnimation();
    UpdateProgress();
    UpdateMovesLeft();
    UpdatePreviewPieces();
}

void GameHud::UpdateLightAnimation() {
    mLightAnimationTime += mEngine.GetLastFrameSeconds();
    
    if (mLightAnimationTime > lightAnimationDuration) {
        mLightAnimationTime = 0.0f;
    }
    
    auto t {(sin(mLightAnimationTime * 2.0f * 3.1415f / lightAnimationDuration) + 1.0f) / 2.0f};
    mLight->SetDirection(lightDirectionA.Lerp(t, lightDirectionB));
}

void GameHud::UpdateProgress() {
    auto progress {
        mLevelObjective == Level::Objective::Clear ? mGameLogic.GetNumLevelBlocksLeft() :
                                                     mGameLogic.GetNumEmptyBlueprintSlotsLeft()
    };
    
    if (progress != mProgress) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%4d", progress);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 4);
        
        auto& text {mProgressText->GetText()};
        auto textLength {text.size()};
        assert(textLength == 4);
        text[0] = buffer[0];
        text[1] = buffer[1];
        text[2] = buffer[2];
        text[3] = buffer[3];
        
        mProgress = progress;
    }
}

void GameHud::UpdateMovesLeft() {
    auto movesLeft {mGameLogic.GetMovesLeft()};
    
    if (movesLeft != mMovesLeft) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%3d", movesLeft);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 3);
        
        auto& text {mMovesText->GetText()};
        auto textLength {text.size()};
        assert(textLength == 3);
        text[0] = buffer[0];
        text[1] = buffer[1];
        text[2] = buffer[2];
        
        mMovesLeft = movesLeft;
    }
}

void GameHud::UpdatePreviewPieces() {
    auto& next2Pieces {mGameLogic.GetNextPieceGenerator().GetNext2Pieces()};
    auto& selectablePieces {mGameLogic.GetSelectablePieces()};
    auto previewPieceAnimationToStart {mGameLogic.GetPreviewPieceAnimationToStart()};
    
    auto shouldStartNextPieceAndSwitchAnimation {
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::NextPieceAndSwitch
    };
    
    auto shouldStartSwitchPieceAnimation {
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::SwitchPiece ||
        shouldStartNextPieceAndSwitchAnimation
    };
    
    UpdatePreviewPieceGroup(mNextPreviewPieces,
                            next2Pieces,
                            mNext2PiecesPreviousFrame,
                            shouldStartNextPieceAndSwitchAnimation,
                            false);
    UpdatePreviewPieceGroup(mSelectablePreviewPieces,
                            selectablePieces,
                            mSelectablePiecesPreviousFrame,
                            shouldStartSwitchPieceAnimation,
                            shouldStartNextPieceAndSwitchAnimation);
    
    mNext2PiecesPreviousFrame = next2Pieces;
    mSelectablePiecesPreviousFrame = selectablePieces;
}

void GameHud::UpdatePreviewPieceGroup(ThreePreviewPieces& previewPieces,
                                      const TwoPieces& pieces,
                                      const TwoPieces& piecesPreviousFrame,
                                      bool shouldStartPreviewPieceAnimation,
                                      bool shouldDeactivateSlotZero) {
    if (shouldStartPreviewPieceAnimation) {
        if (previewPieces[0].mSceneObjects->IsActive()) {
            UpdatePreviewPiece(previewPieces[2],
                               piecesPreviousFrame[1],
                               mPreviewPieceRelativePositions[2]);
        }
        
        if (shouldDeactivateSlotZero) {
            UpdatePreviewPiece(previewPieces[0], nullptr, mPreviewPieceRelativePositions[0]);
        } else {
            UpdatePreviewPiece(previewPieces[0], pieces[0], mPreviewPieceRelativePositions[0]);
        }
        
        UpdatePreviewPiece(previewPieces[1], pieces[1], mPreviewPieceRelativePositions[1]);
    } else if (pieces != piecesPreviousFrame) {
        UpdatePreviewPiece(previewPieces[0], nullptr, mPreviewPieceRelativePositions[0]);
        UpdatePreviewPiece(previewPieces[1], pieces[0], mPreviewPieceRelativePositions[1]);
        UpdatePreviewPiece(previewPieces[2], pieces[1], mPreviewPieceRelativePositions[2]);
    }
}

void GameHud::UpdatePreviewPiece(PreviewPiece& previewPiece,
                                 const Piece* pieceType,
                                 const Pht::Vec3& position) {
    previewPiece.mBombSceneObject = nullptr;
    previewPiece.mRowBombSceneObject = nullptr;
    
    auto& containerObject {previewPiece.mSceneObjects->GetContainerSceneObject()};
    auto& baseTransform {containerObject.GetTransform()};
    baseTransform.SetPosition(position);

    if (pieceType == nullptr) {
        previewPiece.mSceneObjects->SetIsActive(false);
        return;
    }
    
    previewPiece.mSceneObjects->SetIsActive(true);
    
    auto scale {pieceType->GetPreviewCellSize() / cellSize};
    
    baseTransform.SetScale(scale);
    previewPiece.mScale = scale;
    previewPiece.mSceneObjects->ReclaimAll();
    
    auto pieceNumRows {pieceType->GetGridNumRows()};
    auto pieceNumColumns {pieceType->GetGridNumColumns()};
    auto& grid {pieceType->GetGrid(Rotation::Deg0)};
    auto isBomb {pieceType->IsBomb()};
    auto isRowBomb {pieceType->IsRowBomb()};
    
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
                auto& blockSceneObject {previewPiece.mSceneObjects->AccuireSceneObject()};
                
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

void GameHud::OnSwitchPieceAnimationFinished() {
    auto& selectablePieces {mGameLogic.GetSelectablePieces()};

    UpdatePreviewPiece(mSelectablePreviewPieces[0], nullptr, mPreviewPieceRelativePositions[0]);
    UpdatePreviewPiece(mSelectablePreviewPieces[1],
                       selectablePieces[0],
                       mPreviewPieceRelativePositions[1]);
    UpdatePreviewPiece(mSelectablePreviewPieces[2],
                       selectablePieces[1],
                       mPreviewPieceRelativePositions[2]);
}

void GameHud::OnNextPieceAnimationFinished() {
    auto& next2Pieces {mGameLogic.GetNextPieceGenerator().GetNext2Pieces()};

    UpdatePreviewPiece(mNextPreviewPieces[0], nullptr, mPreviewPieceRelativePositions[0]);
    UpdatePreviewPiece(mNextPreviewPieces[1], next2Pieces[0], mPreviewPieceRelativePositions[1]);
    UpdatePreviewPiece(mNextPreviewPieces[2], next2Pieces[1], mPreviewPieceRelativePositions[2]);
}
