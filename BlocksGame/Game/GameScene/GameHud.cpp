#include "GameHud.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "GradientRectangle.hpp"
#include "GameLogic.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"
#include "GameHudController.hpp"
#include "UiLayer.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Vec3 lightDirectionA {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 lightDirectionB {0.4f, 1.0f, 1.0f};
    const auto lightAnimationDuration {5.0f};
    const auto cellSize {1.25f};

    const std::array<Pht::Vec3, 2> pieceRelativePositions = {
        Pht::Vec3{-0.73f, 0.01f, UiLayer::block},
        Pht::Vec3{1.27f, 0.01f, UiLayer::block}
    };
}

GameHud::GameHud(Pht::IEngine& engine,
                 const GameLogic& gameLogic,
                 const LevelResources& levelResources,
                 const PieceResources& pieceResources,
                 GameHudController& gameHudController,
                 const Pht::Font& font,
                 Pht::Scene& scene,
                 Pht::SceneObject& parentObject,
                 int hudLayer,
                 const Level& level) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mPieceResources {pieceResources},
    mLevelObjective {level.GetObjective()} {
    
    gameHudController.SetHudEventListener(*this);

    CreateLightAndCamera(scene, parentObject, hudLayer);
    
    Pht::TextProperties textProperties {font, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}};
    CreateProgressObject(scene, parentObject, textProperties, levelResources);
    CreateMovesObject(scene, parentObject, textProperties);
    CreateNextPiecesObject(scene, parentObject, textProperties, level);
    CreateSelectablePiecesObject(scene, parentObject, textProperties, level);
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
    progressContainer.GetTransform().SetPosition({-4.1f, 12.6f, UiLayer::root});
    parentObject.AddChild(progressContainer);
    
    Pht::Vec3 textRectanglePosition {0.9f, 0.2f, UiLayer::lowerTextRectangle};
    CreateTextRectangle(textRectanglePosition, 4.4f, false, scene, progressContainer);
    
    std::string text {"    "};  // Warning! Must be four spaces to fit digits.
    mProgressText = &scene.CreateText(text, textProperties);
    auto& progressTextSceneobject {mProgressText->GetSceneObject()};
    progressTextSceneobject.GetTransform().SetPosition({1.1f, 0.0f, UiLayer::text});
    progressContainer.AddChild(progressTextSceneobject);
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            CreateSmallPiecesRectangle({0.15f, 0.1f, UiLayer::piecesRectangle},
                                       scene,
                                       progressContainer);
            CreateGrayBlock(scene, progressContainer, levelResources);
            break;
        case Level::Objective::Build:
            CreateBlueprintSlot(scene, progressContainer, levelResources);
            break;
    }
}

void GameHud::CreateGrayBlock(Pht::Scene& scene,
                              Pht::SceneObject& progressContainer,
                              const LevelResources& levelResources) {
    auto& grayBlock {scene.CreateSceneObject()};
    grayBlock.SetRenderable(&levelResources.GetLevelBlockRenderable(BlockRenderableKind::Full));
    
    auto& transform {grayBlock.GetTransform()};
    transform.SetPosition({0.19f, 0.2f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.505f};
    transform.SetScale({scale, scale, scale});
    
    progressContainer.AddChild(grayBlock);
}

void GameHud::CreateBlueprintSlot(Pht::Scene& scene,
                                  Pht::SceneObject& progressContainer,
                                  const LevelResources& levelResources) {
    auto& blueprintSlot {scene.CreateSceneObject()};
    blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotRenderable());
    
    auto& transform {blueprintSlot.GetTransform()};
    transform.SetPosition({0.55f, 0.2f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.56f};
    transform.SetScale({scale, scale, scale});
    
    progressContainer.AddChild(blueprintSlot);
}

void GameHud::CreateMovesObject(Pht::Scene& scene,
                                Pht::SceneObject& parentObject,
                                const Pht::TextProperties& textProperties) {
    auto& movesContainer {scene.CreateSceneObject()};
    movesContainer.GetTransform().SetPosition({3.1f, 12.6f, UiLayer::root});
    parentObject.AddChild(movesContainer);
    
    Pht::Vec3 textRectanglePosition {0.9f, 0.2f, UiLayer::lowerTextRectangle};
    CreateTextRectangle(textRectanglePosition, 4.4f, false, scene, movesContainer);
    
    std::string text {"   "};   // Warning! Must be three spaces to fit digits.
    mMovesText = &scene.CreateText(text, textProperties);
    auto& movesTextSceneobject {mMovesText->GetSceneObject()};
    movesTextSceneobject.GetTransform().SetPosition({1.1f, 0.0f, UiLayer::text});
    movesContainer.AddChild(movesTextSceneobject);
    
    CreateSmallPiecesRectangle({0.15f, 0.1f, UiLayer::piecesRectangle}, scene, movesContainer);
    CreateLPiece(scene, movesContainer);
}

void GameHud::CreateLPiece(Pht::Scene& scene, Pht::SceneObject& movesContainer) {
    auto& lPiece {scene.CreateSceneObject()};
    movesContainer.AddChild(lPiece);
    
    auto& baseTransform {lPiece.GetTransform()};
    baseTransform.SetPosition({0.12f, 0.2f, UiLayer::root});
    baseTransform.SetRotation({-30.0f, -30.0f, 0.0f});
    auto scale {0.32f};
    baseTransform.SetScale({scale, scale, scale});
    
    auto& greenBlockRenderable {
        mPieceResources.GetBlockRenderableObject(BlockRenderableKind::Full,
                                                 BlockColor::Green,
                                                 BlockBrightness::Normal)
    };
    
    auto halfCellSize {0.625f};
    CreateGreenBlock({-halfCellSize, -halfCellSize, -scale}, greenBlockRenderable, scene, lPiece);
    CreateGreenBlock({halfCellSize, -halfCellSize, -scale}, greenBlockRenderable, scene, lPiece);
    CreateGreenBlock({halfCellSize, halfCellSize, -scale}, greenBlockRenderable, scene, lPiece);
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
                                     const Pht::TextProperties& textProperties,
                                     const Level& level) {
    auto& nextPiecesContainer {scene.CreateSceneObject()};
    nextPiecesContainer.GetTransform().SetPosition({-2.3f, -12.3f, UiLayer::root});
    parentObject.AddChild(nextPiecesContainer);
    
    CreatePiecesRectangle({0.15f, 0.0f, UiLayer::piecesRectangle}, false, scene, nextPiecesContainer);
    
    Pht::Vec3 textRectanglePosition {0.55f, 1.42f, UiLayer::textRectangle};
    CreateTextRectangle(textRectanglePosition, 4.8f, false, scene, nextPiecesContainer);
    
    auto& text {scene.CreateText("NEXT", textProperties)};
    auto& textSceneObject {text.GetSceneObject()};
    textSceneObject.SetPosition({-0.1f, 1.22f, UiLayer::text});
    nextPiecesContainer.AddChild(textSceneObject);
    
    CreateTwoPreviewPieces(mNextPieces, nextPiecesContainer, level);
}

void GameHud::CreateSelectablePiecesObject(Pht::Scene& scene,
                                           Pht::SceneObject& parentObject,
                                           const Pht::TextProperties& textProperties,
                                           const Level& level) {
    auto& selectablePiecesContainer {scene.CreateSceneObject()};
    selectablePiecesContainer.GetTransform().SetPosition({3.1f, -12.3f, UiLayer::root});
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
    textSceneObject.SetPosition({-0.55f, 1.22f, UiLayer::text});
    selectablePiecesContainer.AddChild(textSceneObject);
    
    CreateTwoPreviewPieces(mSelectablePieces, selectablePiecesContainer, level);
}

void GameHud::CreateSmallPiecesRectangle(const Pht::Vec3& position,
                                         Pht::Scene& scene,
                                         Pht::SceneObject& parentObject) {
    Pht::Vec2 size {1.8f, 1.3f};
    auto tilt {0.23f};
    float leftQuadWidth {0.3f};
    float rightQuadWidth {0.3f};
 
    GradientRectangleColors upperColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.85f, 0.75f, 0.95f, 0.9f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };

    GradientRectangleColors lowerColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.9f, 0.9f, 1.0f, 0.0f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };
    
    CreateGradientRectangle(scene,
                            parentObject,
                            position,
                            size,
                            tilt,
                            leftQuadWidth,
                            rightQuadWidth,
                            upperColors,
                            lowerColors);
}

Pht::SceneObject& GameHud::CreateTextRectangle(const Pht::Vec3& position,
                                               float length,
                                               bool isBright,
                                               Pht::Scene& scene,
                                               Pht::SceneObject& parentObject) {
    Pht::Vec2 size {length, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
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
    float leftQuadWidth {0.4f};
    float rightQuadWidth {0.4f};
    
    GradientRectangleColors upperColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {isBright ? 0.95f : 0.85f, isBright ? 0.6f : 0.4f, 0.95f, 0.93f}, // .mMid = {0.85f, 0.7f, 0.95f, 0.93f},
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

void GameHud::CreateTwoPreviewPieces(TwoPreviewPieces& previewPieces,
                                     Pht::SceneObject& parentObject,
                                     const Level& level) {
    for (auto i {0}; i < previewPieces.size(); ++i) {
        auto& piece {previewPieces[i]};
        piece.mSceneObjects = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks,
                                                                parentObject,
                                                                level);
        auto& transform {piece.mSceneObjects->GetContainerSceneObject().GetTransform()};
        transform.SetPosition(pieceRelativePositions[i]);
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
    UpdatePreviewPiece(mNextPieces[0], next2Pieces[0]);
    UpdatePreviewPiece(mNextPieces[1], next2Pieces[1]);
    
    auto& selectablePieces {mGameLogic.GetSelectablePieces()};
    UpdatePreviewPiece(mSelectablePieces[0], selectablePieces[0]);
    UpdatePreviewPiece(mSelectablePieces[1], selectablePieces[1]);
}

void GameHud::UpdatePreviewPiece(PreviewPiece& previewPiece, const Piece* currentPieceType) {
    if (previewPiece.mPieceType == currentPieceType) {
        return;
    }
    
    previewPiece.mPieceType = currentPieceType;
    
    auto& containerObject {previewPiece.mSceneObjects->GetContainerSceneObject()};
    auto& baseTransform {containerObject.GetTransform()};
    auto scale {currentPieceType->GetPreviewCellSize() / cellSize};
    baseTransform.SetScale({scale, scale, scale});
    
    previewPiece.mSceneObjects->ReclaimAll();
    
    auto pieceNumRows {currentPieceType->GetGridNumRows()};
    auto pieceNumColumns {currentPieceType->GetGridNumColumns()};
    auto& grid {currentPieceType->GetGrid(Rotation::Deg0)};
    
    auto isBomb {currentPieceType->IsBomb()};
    auto isRowBomb {currentPieceType->IsRowBomb()};
    
    Pht::Vec3 lowerLeft {
        -static_cast<float>(pieceNumColumns) * cellSize / 2.0f + cellSize / 2.0f,
        -static_cast<float>(pieceNumRows) * cellSize / 2.0f + cellSize / 2.0f,
        0.0f
    };
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto& subCell {grid[row][column].mFirstSubCell};
            auto renderableKind {subCell.mBlockRenderableKind};
 
            if (renderableKind != BlockRenderableKind::None) {
                auto& blockSceneObject {previewPiece.mSceneObjects->AccuireSceneObject()};
                
                if (isBomb) {
                    blockSceneObject.SetRenderable(&mPieceResources.GetBombRenderableObject());
                } else if (isRowBomb) {
                    blockSceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
                } else {
                    auto& blockRenderable {
                        mPieceResources.GetBlockRenderableObject(renderableKind,
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
