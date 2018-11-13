#include "GameHud.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "TextComponent.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "RoundedCylinder.hpp"
#include "GameLogic.hpp"
#include "Field.hpp"
#include "LevelResources.hpp"
#include "PieceResources.hpp"
#include "GameHudRectangles.hpp"
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
    constexpr auto roundedCylinderOpacity {0.45f};
    constexpr auto cellSize {1.25f};
    
    template <typename T, typename U>
    void CreatePreviewPieces(T& previewPieces,
                             U& previewPieceRelativePositions,
                             Pht::SceneObject& parentObject) {
        for (auto i {0}; i < previewPieces.size(); ++i) {
            auto& piece {previewPieces[i]};
            
            piece.mSceneObjects = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PreviewPieceBlocks,
                                                                    parentObject);
            piece.mSceneObjects->SetIsActive(false);
            
            auto& transform {piece.mSceneObjects->GetContainerSceneObject().GetTransform()};
            transform.SetPosition(previewPieceRelativePositions[i]);
            transform.SetRotation({-30.0f, -30.0f, 0.0f});
        }
    }
    
    float CalculateLowerHudObjectYPosition(Pht::IEngine& engine) {
        auto& renderer {engine.GetRenderer()};
        auto bottomPadding {renderer.GetBottomPaddingHeight()};
        auto yPosition {-renderer.GetHudFrustumSize().y / 2.0f + bottomPadding + 1.35f};
        
        if (bottomPadding != 0.0f) {
            yPosition += 0.19f;
        }
        
        return yPosition;
    }
    
    void WriteIntegerAtBeginningOfString(int value, std::string& str) {
        constexpr auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%3d", value);
        
        assert(std::strlen(buffer) <= 3);
        assert(str.size() >= 3);

        str[0] = buffer[0];
        str[1] = buffer[1];
        str[2] = buffer[2];
    }
}

GameHud::GameHud(Pht::IEngine& engine,
                 const GameLogic& gameLogic,
                 const Field& field,
                 const LevelResources& levelResources,
                 const PieceResources& pieceResources,
                 const GameHudRectangles& hudRectangles,
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
    mNextPreviewPiecesRelativePositions {
        Pht::Vec3{-1.88f, 0.0f, UiLayer::block},
        Pht::Vec3{-0.58f, 0.0f, UiLayer::block},
        Pht::Vec3{1.42f, 0.0f, UiLayer::block},
        Pht::Vec3{2.72f, 0.0f, UiLayer::block}
    },
    mSelectablePreviewPiecesRelativePositions {
        Pht::Vec3{-2.96f, 0.0f, UiLayer::block},
        Pht::Vec3{-1.78f, 0.0f, UiLayer::block},
        Pht::Vec3{0.22f, 0.0f, UiLayer::block},
        Pht::Vec3{2.62f, 0.0f, UiLayer::block},
        Pht::Vec3{3.72f, 0.0f, UiLayer::block}
    } {
    
    mNext2PiecesPreviousFrame.fill(nullptr);
    mSelectablePiecesPreviousFrame.fill(nullptr);
    
    gameHudController.SetHudEventListener(*this);

    CreateLightAndCamera(scene, parentObject, hudLayer);

    CreateProgressObject(scene, parentObject, field, commonResources, levelResources);
    CreateMovesObject(scene, parentObject, commonResources);
    CreateNextPiecesObject(scene, parentObject, hudRectangles);
    CreateSelectablePiecesObject(scene, parentObject, hudRectangles);
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
                                   const Field& field,
                                   const CommonResources& commonResources,
                                   const LevelResources& levelResources) {
    auto& progressContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    
    Pht::Vec3 position {
        -3.4f,
        renderer.GetHudFrustumSize().y / 2.0f - renderer.GetTopPaddingHeight() - 0.67f,
        UiLayer::root
    };
    
    mProgressContainer = &progressContainer;
    progressContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(progressContainer);
    
    Pht::Vec3 cylinderPosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateRoundedCylinder(scene,
                          progressContainer,
                          cylinderPosition,
                          {2.5, 1.05f},
                          roundedCylinderOpacity,
                          roundedCylinderAmbient,
                          roundedCylinderDiffuse);
    
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::Yes),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    mProgressText = &scene.CreateText("", textProperties);
    auto& progressTextSceneobject {mProgressText->GetSceneObject()};
    
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            CreateGrayBlock(scene, progressContainer, commonResources);
            progressTextSceneobject.GetTransform().SetPosition({-0.48f, -0.215f, UiLayer::text});
            mProgressGoal = field.CalculateNumLevelBlocks();
            break;
        case Level::Objective::BringDownTheAsteroid:
            CreateAsteroid(scene, progressContainer, levelResources);
            progressTextSceneobject.GetTransform().SetPosition({-0.21f, -0.215f, UiLayer::text});
            mProgressGoal = 1;
            break;
        case Level::Objective::Build:
            CreateBlueprintSlot(scene, progressContainer, levelResources);
            progressTextSceneobject.GetTransform().SetPosition({-0.48f, -0.215f, UiLayer::text});
            mProgressGoal = field.CalculateNumEmptyBlueprintSlots();
            break;
    }
    
    auto* textComponent {progressTextSceneobject.GetComponent<Pht::TextComponent>()};
    assert(textComponent);
    std::string digitsPlaceholder {"   "}; // Warning! Must be three spaces to fit digits.
    textComponent->GetText() = digitsPlaceholder + "/" + std::to_string(mProgressGoal);
    
    progressContainer.AddChild(progressTextSceneobject);
    
    Pht::SceneObjectUtils::ScaleRecursively(*mProgressContainer, 1.1f);
}

void GameHud::CreateGrayBlock(Pht::Scene& scene,
                              Pht::SceneObject& progressContainer,
                              const CommonResources& commonResources) {
    auto& grayBlock {
        scene.CreateSceneObject(Pht::ObjMesh {"cube_428.obj", 1.25f},
                                commonResources.GetMaterials().GetGrayYellowMaterial())
    };
    
    auto& transform {grayBlock.GetTransform()};
    transform.SetPosition({-1.05f, 0.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.505f);
    
    progressContainer.AddChild(grayBlock);
}

void GameHud::CreateAsteroid(Pht::Scene& scene,
                             Pht::SceneObject& progressContainer,
                             const LevelResources& levelResources) {
    auto& asteroid {scene.CreateSceneObject()};
    asteroid.SetRenderable(&levelResources.GetAsteroidFragmentRenderable());
    
    auto& transform {asteroid.GetTransform()};
    transform.SetPosition({-0.95f, 0.0f, UiLayer::block});
    transform.SetRotation({-25.0f, 45.0f, -12.0f});
    transform.SetScale(2.4f);
    
    progressContainer.AddChild(asteroid);
}

void GameHud::CreateBlueprintSlot(Pht::Scene& scene,
                                  Pht::SceneObject& progressContainer,
                                  const LevelResources& levelResources) {
    auto& blueprintSlotContainer {scene.CreateSceneObject()};
    auto& transform {blueprintSlotContainer.GetTransform()};
    transform.SetPosition({-1.0f, 0.0f, UiLayer::block});
    transform.SetRotation({-30.0f, -30.0f, 0.0f});
    transform.SetScale(0.56f);
    progressContainer.AddChild(blueprintSlotContainer);
    
    auto& blueprintSlot {scene.CreateSceneObject()};
    blueprintSlot.SetRenderable(&levelResources.GetBlueprintSlotNonDepthWritingRenderable());
    blueprintSlotContainer.AddChild(blueprintSlot);
    
    auto& fieldCell {scene.CreateSceneObject()};
    fieldCell.SetRenderable(&levelResources.GetFieldCellRenderable());
    fieldCell.GetTransform().SetPosition({0.0f, 0.0f, -0.005f});
    blueprintSlotContainer.AddChild(fieldCell);
}

void GameHud::CreateMovesObject(Pht::Scene& scene,
                                Pht::SceneObject& parentObject,
                                const CommonResources& commonResources) {
    auto& movesContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    
    Pht::Vec3 position {
        3.4f,
        renderer.GetHudFrustumSize().y / 2.0f - renderer.GetTopPaddingHeight() - 0.67f,
        UiLayer::root
    };

    mMovesContainer = &movesContainer;
    movesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(movesContainer);
    
    Pht::Vec3 cylinderPosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateRoundedCylinder(scene,
                          movesContainer,
                          cylinderPosition,
                          {2.5, 1.05f},
                          roundedCylinderOpacity,
                          roundedCylinderAmbient,
                          roundedCylinderDiffuse);
    
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.27f, 0.27f, 0.27f, 0.5f}
    };
    
    std::string text {"   "};   // Warning! Must be three spaces to fit digits.
    mMovesText = &scene.CreateText(text, textProperties);
    auto& movesTextSceneobject {mMovesText->GetSceneObject()};
    movesTextSceneobject.GetTransform().SetPosition({-0.05f, -0.29f, UiLayer::text});
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
                                     const GameHudRectangles& hudRectangles) {
    auto& nextPiecesContainer {scene.CreateSceneObject()};
    Pht::Vec3 position {-3.65f, CalculateLowerHudObjectYPosition(mEngine), UiLayer::root};

    mNextPiecesContainer = &nextPiecesContainer;
    nextPiecesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(nextPiecesContainer);

    auto& nextPiecesRectangle {scene.CreateSceneObject()};
    nextPiecesRectangle.SetRenderable(&hudRectangles.GetNextPiecesRectangle());
    nextPiecesRectangle.GetTransform().SetPosition({0.15f, 0.0f, UiLayer::piecesRectangle});
    nextPiecesContainer.AddChild(nextPiecesRectangle);

    CreatePreviewPieces(mNextPreviewPieces,
                        mNextPreviewPiecesRelativePositions,
                        nextPiecesContainer);
}

void GameHud::CreateSelectablePiecesObject(Pht::Scene& scene,
                                           Pht::SceneObject& parentObject,
                                           const GameHudRectangles& hudRectangles) {
    auto& selectablePiecesContainer {scene.CreateSceneObject()};
    Pht::Vec3 position {2.65f, CalculateLowerHudObjectYPosition(mEngine), UiLayer::root};
    
    mSelectablePiecesContainer = &selectablePiecesContainer;
    selectablePiecesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(selectablePiecesContainer);

    mSelectablePiecesRectangle = &scene.CreateSceneObject();
    mSelectablePiecesRectangle->SetRenderable(&hudRectangles.GetSelectablePiecesRectangle());
    mSelectablePiecesRectangle->GetTransform().SetPosition({0.15f, 0.0f, UiLayer::piecesRectangle});
    selectablePiecesContainer.AddChild(*mSelectablePiecesRectangle);
    
    mPressedSelectablePiecesRectangle = &scene.CreateSceneObject();
    mPressedSelectablePiecesRectangle->SetRenderable(&hudRectangles.GetPressedSelectablePiecesRectangle());
    mPressedSelectablePiecesRectangle->GetTransform().SetPosition({0.15f, 0.0f, UiLayer::piecesRectangle});
    selectablePiecesContainer.AddChild(*mPressedSelectablePiecesRectangle);
    mPressedSelectablePiecesRectangle->SetIsVisible(false);

    CreatePreviewPieces(mSelectablePreviewPieces,
                        mSelectablePreviewPiecesRelativePositions,
                        selectablePiecesContainer);
}

void GameHud::OnSwitchButtonDown() {
    mPressedSelectablePiecesRectangle->SetIsVisible(true);
    mSelectablePiecesRectangle->SetIsVisible(false);
}

void GameHud::OnSwitchButtonUp() {
    mPressedSelectablePiecesRectangle->SetIsVisible(false);
    mSelectablePiecesRectangle->SetIsVisible(true);
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
    auto progress {mProgressGoal - mGameLogic.GetNumObjectsLeftToClear()};

    if (progress != mProgress) {
        WriteIntegerAtBeginningOfString(progress, mProgressText->GetText());
        mProgress = progress;
    }
}

void GameHud::UpdateMovesLeft() {
    auto movesLeft {mGameLogic.GetMovesLeft()};
    
    if (movesLeft != mMovesLeft) {
        WriteIntegerAtBeginningOfString(movesLeft, mMovesText->GetText());
        mMovesLeft = movesLeft;
    }
}

void GameHud::UpdatePreviewPieces() {
    auto previewPieceAnimationToStart {mGameLogic.GetPreviewPieceAnimationToStart()};
    
    auto shouldStartNextPieceAndSwitchAnimation {
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::NextPieceAndSwitch
    };
    
    auto shouldStartSwitchPieceAnimation {
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::SwitchPiece ||
        shouldStartNextPieceAndSwitchAnimation
    };
    
    auto shouldStartRemoveActivePieceAnimation {
        previewPieceAnimationToStart == PreviewPieceAnimationToStart::RemoveActivePiece
    };
    
    UpdateNextPreviewPieceGroup(shouldStartNextPieceAndSwitchAnimation);
    UpdateSelectablePreviewPieceGroup(shouldStartSwitchPieceAnimation,
                                      shouldStartNextPieceAndSwitchAnimation,
                                      shouldStartRemoveActivePieceAnimation);
}

void GameHud::UpdateNextPreviewPieceGroup(bool shouldStartPreviewPieceAnimation) {
    auto& next2Pieces {mGameLogic.GetNextPieceGenerator().GetNext2Pieces()};
    auto& positions {mNextPreviewPiecesRelativePositions};
    
    if (shouldStartPreviewPieceAnimation) {
        if (mNextPreviewPieces[0].mSceneObjects->IsActive()) {
            UpdatePreviewPiece(mNextPreviewPieces[2], mNext2PiecesPreviousFrame[1], positions[2]);
        }
        
        UpdatePreviewPiece(mNextPreviewPieces[0], next2Pieces[0], positions[0]);
        UpdatePreviewPiece(mNextPreviewPieces[1], next2Pieces[1], positions[1]);
    } else if (next2Pieces != mNext2PiecesPreviousFrame) {
        UpdatePreviewPiece(mNextPreviewPieces[0], nullptr, positions[0]);
        UpdatePreviewPiece(mNextPreviewPieces[1], next2Pieces[0], positions[1]);
        UpdatePreviewPiece(mNextPreviewPieces[2], next2Pieces[1], positions[2]);
    }
    
    mNext2PiecesPreviousFrame = next2Pieces;
}

void GameHud::UpdateSelectablePreviewPieceGroup(bool shouldStartPreviewPieceAnimation,
                                                bool shouldDeactivateSlotZero,
                                                bool shouldStartRemoveActivePieceAnimation) {
    auto& selectablePieces {mGameLogic.GetSelectablePieces()};
    auto* activePiece {mGameLogic.GetPieceType()};
    auto& positions {mSelectablePreviewPiecesRelativePositions};

    if (shouldStartPreviewPieceAnimation) {
        if (mSelectablePreviewPieces[0].mSceneObjects->IsActive()) {
            UpdatePreviewPiece(mSelectablePreviewPieces[3],
                               mActivePiecePreviousFrame,
                               positions[3]);
        }
        
        if (shouldDeactivateSlotZero) {
            UpdatePreviewPiece(mSelectablePreviewPieces[0], nullptr, positions[0]);
        } else {
            UpdatePreviewPiece(mSelectablePreviewPieces[0], selectablePieces[0], positions[0]);
        }
        
        UpdatePreviewPiece(mSelectablePreviewPieces[1], selectablePieces[1], positions[1]);
        UpdatePreviewPiece(mSelectablePreviewPieces[2], activePiece, positions[2]);
    } else if (selectablePieces != mSelectablePiecesPreviousFrame ||
               shouldStartRemoveActivePieceAnimation) {
        UpdatePreviewPiece(mSelectablePreviewPieces[0], nullptr, positions[0]);
        UpdatePreviewPiece(mSelectablePreviewPieces[1], selectablePieces[0], positions[1]);
        UpdatePreviewPiece(mSelectablePreviewPieces[2], selectablePieces[1], positions[2]);
        
        if (shouldStartRemoveActivePieceAnimation) {
            UpdatePreviewPiece(mSelectablePreviewPieces[3], mActivePiecePreviousFrame, positions[3]);
        } else {
            UpdatePreviewPiece(mSelectablePreviewPieces[3], activePiece, positions[3]);
        }
    }
    
    mSelectablePiecesPreviousFrame = selectablePieces;
    mActivePiecePreviousFrame = activePiece;
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
    
    if (pieceType->NeedsUpAdjustmentInHud()) {
        lowerLeft.y += cellSize / 2.0f;
    } else if (pieceType->NeedsDownAdjustmentInHud()) {
        lowerLeft.y -= cellSize / 2.0f;
    }
    
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
    auto& positions {mSelectablePreviewPiecesRelativePositions};

    UpdatePreviewPiece(mSelectablePreviewPieces[0], nullptr, positions[0]);
    UpdatePreviewPiece(mSelectablePreviewPieces[1], selectablePieces[0], positions[1]);
    UpdatePreviewPiece(mSelectablePreviewPieces[2], selectablePieces[1], positions[2]);
    UpdatePreviewPiece(mSelectablePreviewPieces[3], mGameLogic.GetPieceType(), positions[3]);
}

void GameHud::OnNextPieceAnimationFinished() {
    auto& next2Pieces {mGameLogic.GetNextPieceGenerator().GetNext2Pieces()};
    auto& positions {mNextPreviewPiecesRelativePositions};

    UpdatePreviewPiece(mNextPreviewPieces[0], nullptr, positions[0]);
    UpdatePreviewPiece(mNextPreviewPieces[1], next2Pieces[0], positions[1]);
    UpdatePreviewPiece(mNextPreviewPieces[2], next2Pieces[1], positions[2]);
}
