#include "GameSceneRenderer.hpp"

#include <array>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "RenderUtils.hpp"
#include "Field.hpp"
#include "GameLogic.hpp"
#include "Piece.hpp"
#include "FallingPiece.hpp"
#include "GameScene.hpp"
#include "GameHud.hpp"
#include "GameViewControllers.hpp"
#include "ExplosionParticleEffect.hpp"
#include "RowExplosionParticleEffect.hpp"
#include "FlyingBlocksAnimation.hpp"
#include "FlashingBlocksAnimation.hpp"
#include "SlidingTextAnimation.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"

using namespace BlocksGame;

namespace {
    const std::array<Pht::Mat4, 4> rotationMatrices = {
        Pht::Mat4::RotateZ(RotationToDeg(Rotation::Deg0)),
        Pht::Mat4::RotateZ(RotationToDeg(Rotation::Deg90)),
        Pht::Mat4::RotateZ(RotationToDeg(Rotation::Deg180)),
        Pht::Mat4::RotateZ(RotationToDeg(Rotation::Deg270))
    };
    
    const auto plus45RotationMatrix {Pht::Mat4::RotateZ(45.0f)};
    const auto minus45RotationMatrix {Pht::Mat4::RotateZ(-45.0f)};
    const auto previewRotationMatrix {Pht::Mat4::RotateX(-30.0f) * Pht::Mat4::RotateY(-30.0f)};
    const auto dz {0.05f};
}

GameSceneRenderer::GameSceneRenderer(Pht::IEngine& engine,
                                     GameScene& scene,
                                     const Field& field,
                                     const GameLogic& gameLogic,
                                     const ExplosionParticleEffect& explosionParticleEffect,
                                     const RowExplosionParticleEffect& rowExplosionParticleEffect,
                                     const FlyingBlocksAnimation& flyingBlocksAnimation,
                                     const SlidingTextAnimation& slidingTextAnimation,
                                     const ScrollController& scrollController,
                                     const GameHud& hud,
                                     const GameViewControllers& gameViewControllers,
                                     const PieceResources& pieceResources,
                                     const LevelResources& levelResources) :
    mEngine {engine},
    mEngineRenderer {engine.GetRenderer()},
    mScene {scene},
    mField {field},
    mGameLogic {gameLogic},
    mExplosionParticleEffect {explosionParticleEffect},
    mRowExplosionParticleEffect {rowExplosionParticleEffect},
    mFlyingBlocksAnimation {flyingBlocksAnimation},
    mSlidingTextAnimation {slidingTextAnimation},
    mScrollController {scrollController},
    mHud {hud},
    mGameViewControllers {gameViewControllers},
    mPieceResources {pieceResources},
    mLevelResources {levelResources} {}

void GameSceneRenderer::RenderFrame() {
    RenderBlueprintSlots();
    RenderFieldBlocks();
    RenderFallingPiece();
    RenderGhostPieces();
}

void GameSceneRenderer::Render() {
    auto* scene {mEngine.GetSceneManager().GetActiveScene()};
    mEngine.GetRenderer().RenderScene(*scene);

    mEngineRenderer.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    
    RenderExplosion();
    RenderRowExplosion();
    RenderFlyingBlocks();
    RenderSlidingText();
    RenderHud();
    RenderGameViews();
}

void GameSceneRenderer::RenderBlueprintSlots() {
    auto* blueprintGrid {mField.GetBlueprintGrid()};
    
    if (blueprintGrid == nullptr) {
        return;
    }
    
    auto lowestVisibleRow {static_cast<int>(mScrollController.GetLowestVisibleRow())};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    
    for (auto row {0}; row < pastHighestVisibleRow; row++) {
        for (auto column {0}; column < mField.GetNumColumns(); column++) {
            auto& blueprintCell {(*blueprintGrid)[row][column]};
            
            if (blueprintCell.mSceneObject == nullptr) {
                continue;
            }
            
            if (row < lowestVisibleRow || blueprintCell.mAnimation.mIsActive) {
                blueprintCell.mSceneObject->SetIsVisible(false);
            } else {
                blueprintCell.mSceneObject->SetIsVisible(true);
            }
        }
    }
}

void GameSceneRenderer::RenderFieldBlocks() {
    if (!mField.HasChanged() && !mScrollController.IsScrolling()) {
        return;
    }
    
    mScene.GetFieldBlocks().ReclaimAll();

    int lowestVisibleRow {
        mScrollController.IsScrollingDownInClearMode() || mGameLogic.IsCascading() ?
            mField.GetLowestVisibleRow() - 1 :
            static_cast<int>(mScrollController.GetLowestVisibleRow()) - 1
    };
    
    if (lowestVisibleRow < 0) {
        lowestVisibleRow = 0;
    }
    
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    
    for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; row++) {
        for (auto column {0}; column < mField.GetNumColumns(); column++) {
            auto& cell {mField.GetCell(row, column)};
            RenderFieldBlock(cell.mFirstSubCell);
            RenderFieldBlock(cell.mSecondSubCell);
        }
    }
}

void GameSceneRenderer::RenderFieldBlock(const SubCell& subCell) {
    auto renderableKind {subCell.mBlockRenderableKind};
    
    if (renderableKind == BlockRenderableKind::None) {
        return;
    }
    
    auto& sceneObject {mScene.GetFieldBlocks().AccuireSceneObject()};
    const auto cellSize {mScene.GetCellSize()};
    
    Pht::Vec3 blockPosition {
        subCell.mPosition.x * cellSize + cellSize / 2.0f,
        subCell.mPosition.y * cellSize + cellSize / 2.0f,
        0.0f
    };

    auto& transform {sceneObject.GetTransform()};
    transform.SetPosition(blockPosition);
    
    if (renderableKind != BlockRenderableKind::Full) {
        Pht::Vec3 blockRotation {0.0f, 0.0f, RotationToDeg(subCell.mRotation)};
        transform.SetRotation(blockRotation);
    } else {
        transform.SetRotation({0.0f, 0.0f, 0.0f});
    }

    if (subCell.mIsLevel) {
        sceneObject.SetRenderable(&mLevelResources.GetLevelBlockRenderable(renderableKind));
    } else {
        auto color {subCell.mColor};
        auto brightness {subCell.mFlashingBlockAnimation.mBrightness};

        auto* renderableObject {
            &mPieceResources.GetBlockRenderableObject(renderableKind, color, brightness)
        };
        
        assert(renderableObject);
        sceneObject.SetRenderable(renderableObject);

        auto& weldRenderable {mPieceResources.GetWeldRenderableObject(color, brightness)};
        RenderBlockWelds(subCell, blockPosition, weldRenderable, mScene.GetFieldBlocks());
    }
}

void GameSceneRenderer::RenderBlockWelds(const SubCell& subCell,
                                         const Pht::Vec3& blockPos,
                                         Pht::RenderableObject& weldRenderalbeObject,
                                         SceneObjectPool& pool) {
    auto& welds {subCell.mWelds};
    const auto cellSize {mScene.GetCellSize()};
    auto weldZ {blockPos.z + cellSize / 2.0f};
    
    if (welds.mUpLeft) {
        RenderBlockWeld({blockPos.x - cellSize / 2.0f, blockPos.y + cellSize / 2.0f, weldZ},
                        45.0f,
                        weldRenderalbeObject,
                        pool);
    }
    
    if (welds.mUp) {
        RenderBlockWeld({blockPos.x, blockPos.y + cellSize / 2.0f, weldZ},
                        -90.0f,
                        weldRenderalbeObject,
                        pool);
    }
    
    if (welds.mUpRight) {
        RenderBlockWeld({blockPos.x + cellSize / 2.0f, blockPos.y + cellSize / 2.0f, weldZ},
                        -45.0f,
                        weldRenderalbeObject,
                        pool);
    }

    if (welds.mRight) {
        RenderBlockWeld({blockPos.x + cellSize / 2.0f, blockPos.y, weldZ},
                        0.0f,
                        weldRenderalbeObject,
                        pool);
    }
}

void GameSceneRenderer::RenderBlockWeld(const Pht::Vec3& weldPosition,
                                        float rotation,
                                        Pht::RenderableObject& weldRenderalbeObject,
                                        SceneObjectPool& pool) {
    auto& sceneObject {pool.AccuireSceneObject()};
    auto& transform {sceneObject.GetTransform()};
    transform.SetRotation({0.0f, 0.0f, rotation});
    transform.SetPosition(weldPosition);
    sceneObject.SetRenderable(&weldRenderalbeObject);
}

void GameSceneRenderer::RenderFallingPiece() {
    mScene.GetPieceBlocks().ReclaimAll();
    
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    
    if (fallingPiece == nullptr) {
        return;
    }
    
    auto pieceGridPos {
        mGameLogic.IsUsingClickControls() ? fallingPiece->GetRenderablePositionSmoothY() :
                                            fallingPiece->GetRenderablePosition()
    };
    
    const auto cellSize {mScene.GetCellSize()};
    Pht::Vec3 pieceFieldPos {pieceGridPos.x * cellSize, pieceGridPos.y * cellSize, 0.0f};
    auto& pieceGrid {fallingPiece->GetPieceType().GetGrid(fallingPiece->GetRotation())};
    
    RenderPieceBlocks(pieceGrid, pieceFieldPos, false, mScene.GetPieceBlocks());
}

void GameSceneRenderer::RenderPieceBlocks(const CellGrid& pieceBlocks,
                                          const Pht::Vec3& pieceFieldPos,
                                          bool isTransparent,
                                          SceneObjectPool& pool) {
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    assert(fallingPiece);
    
    auto& pieceType {fallingPiece->GetPieceType()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    const auto cellSize {mScene.GetCellSize()};
    auto isBomb {pieceType.IsBomb()};
    auto isRowBomb {pieceType.IsRowBomb()};
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto& subCell {pieceBlocks[row][column].mFirstSubCell};
            auto renderableKind {subCell.mBlockRenderableKind};
            
            if (renderableKind == BlockRenderableKind::None) {
                continue;
            }
            
            auto& sceneObject {pool.AccuireSceneObject()};
            
            Pht::Vec3 blockPosition {
                column * cellSize + cellSize / 2.0f + pieceFieldPos.x,
                row * cellSize + cellSize / 2.0f + pieceFieldPos.y,
                pieceFieldPos.z
            };

            auto& transform {sceneObject.GetTransform()};
            transform.SetPosition(blockPosition);

            if (renderableKind != BlockRenderableKind::Full) {
                Pht::Vec3 blockRotation {0.0f, 0.0f, RotationToDeg(subCell.mRotation)};
                transform.SetRotation(blockRotation);
            } else {
                transform.SetRotation({0.0f, 0.0f, 0.0f});
            }
            
            if (isBomb) {
                if (isTransparent) {
                    sceneObject.SetRenderable(&mPieceResources.GetTransparentBombRenderableObject());
                } else {
                    sceneObject.SetRenderable(&mPieceResources.GetBombRenderableObject());
                }
            } else if (isRowBomb) {
                if (isTransparent) {
                    sceneObject.SetRenderable(&mPieceResources.GetTransparentRowBombRenderableObject());
                } else {
                    sceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
                }
            } else {
                auto color {subCell.mColor};
                auto brightness {BlockBrightness::Normal};

                auto* blockRenderableObject {
                    &mPieceResources.GetBlockRenderableObject(renderableKind, color, brightness)
                };
                
                assert(blockRenderableObject);
                sceneObject.SetRenderable(blockRenderableObject);

                auto& weldRenderable {mPieceResources.GetWeldRenderableObject(color, brightness)};
                RenderBlockWelds(subCell, blockPosition, weldRenderable, pool);
            }
        }
    }
}

void GameSceneRenderer::RenderGhostPieces() {
    mScene.GetGhostPieces().ReclaimAll();
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    
    if (fallingPiece == nullptr) {
        return;
    }
    
    if (mGameLogic.IsUsingClickControls()) {
        RenderClickableGhostPieces(*fallingPiece);
    } else {
        RenderGhostPieceForGestureControls(*fallingPiece);
    }
}

void GameSceneRenderer::RenderGhostPieceForGestureControls(const FallingPiece& fallingPiece) {
    const auto cellSize {mScene.GetCellSize()};
    auto& pieceType {fallingPiece.GetPieceType()};
    
    Pht::Vec3 ghostPieceFieldPos {
        fallingPiece.GetRenderablePosition().x * cellSize,
        mGameLogic.GetGhostPieceRow() * cellSize,
        mScene.GetGhostPieceZ()
    };
    
    if (auto* ghostPieceRenderable {pieceType.GetGhostPieceRenderable()}) {
        Pht::Vec3 ghostPieceCenterLocalCoords {
            cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
            cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f,
            0.0f
        };
        
        RenderGhostPiece(*ghostPieceRenderable,
                         ghostPieceFieldPos + ghostPieceCenterLocalCoords,
                         fallingPiece.GetRotation());
    } else {
        Pht::Vec3 position {ghostPieceFieldPos};
        position.z = mScene.GetPressedGhostPieceZ();
        auto& pieceGrid {pieceType.GetGrid(fallingPiece.GetRotation())};
        auto isTransparent {true};
        RenderPieceBlocks(pieceGrid, position, isTransparent, mScene.GetGhostPieces());
    }
}

void GameSceneRenderer::RenderGhostPiece(Pht::RenderableObject& ghostPieceRenderable,
                                         const Pht::Vec3& position,
                                         Rotation rotation) {
    auto& sceneObject {mScene.GetGhostPieces().AccuireSceneObject()};
    
    auto& transform {sceneObject.GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, RotationToDeg(rotation)});
    
    sceneObject.SetRenderable(&ghostPieceRenderable);
}

void GameSceneRenderer::RenderClickableGhostPieces(const FallingPiece& fallingPiece) {
    auto* moveAlternatives {mGameLogic.GetClickInputHandler().GetMoveAlternativeSet()};
    
    if (moveAlternatives == nullptr) {
        return;
    }
    
    const auto cellSize {mScene.GetCellSize()};
    auto ghostPieceZ {mScene.GetGhostPieceZ()};
    auto& pieceType {fallingPiece.GetPieceType()};
    auto* ghostPieceRenderable {pieceType.GetGhostPieceRenderable()};
    
    Pht::Vec3 ghostPieceCenterLocalCoords {
        cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
        cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f,
        0.0f
    };
    
    for (auto i {0}; i < moveAlternatives->Size(); ++i) {
        auto& move {moveAlternatives->At(i)};
        
        Pht::Vec3 ghostPieceFieldPos {
            static_cast<float>(move.mPosition.x) * cellSize,
            static_cast<float>(move.mPosition.y) * cellSize,
            ghostPieceZ + static_cast<float>(i) * dz
        };
        
        auto isMoveButtonDown {move.mButton->GetButton().IsDown()};
        
        if (ghostPieceRenderable) {
            Pht::Vec3 postion {ghostPieceFieldPos + ghostPieceCenterLocalCoords};
            auto rotation {move.mRotation};
            
            if (isMoveButtonDown) {
                RenderGhostPiece(*pieceType.GetPressedGhostPieceRenderable(), postion, rotation);
            } else {
                RenderGhostPiece(*ghostPieceRenderable, postion, rotation);
            }
        } else {
            auto& pieceGrid {pieceType.GetGrid(move.mRotation)};
            auto& pool {mScene.GetGhostPieces()};
            
            if (isMoveButtonDown) {
                Pht::Vec3 position {ghostPieceFieldPos};
                position.z = mScene.GetPressedGhostPieceZ();
                auto isTransparent {false};
                RenderPieceBlocks(pieceGrid, position, isTransparent, pool);
            } else {
                auto isTransparent {true};
                RenderPieceBlocks(pieceGrid, ghostPieceFieldPos, isTransparent, pool);
            }
        }
    }
}

void GameSceneRenderer::RenderExplosion() {
    mEngineRenderer.RenderSceneObject(mExplosionParticleEffect.GetInnerEffect());
    mEngineRenderer.RenderSceneObject(mExplosionParticleEffect.GetOuterEffect());
}

void GameSceneRenderer::RenderRowExplosion() {
    mEngineRenderer.RenderSceneObject(mRowExplosionParticleEffect.GetSceneObject());
}

void GameSceneRenderer::RenderFlyingBlocks() {
    auto& bodies {mFlyingBlocksAnimation.GetRigidBodies()};

    for (auto i {0}; i < bodies.Size(); ++i) {
        auto& body {bodies.At(i)};
        
        auto rotationMatrix {
            Pht::Mat4::RotateX(body.mOrientation.x) *
            Pht::Mat4::RotateY(body.mOrientation.y) *
            Pht::Mat4::RotateZ(body.mOrientation.z)
        };
        
        auto matrix {
            rotationMatrix *
            Pht::Mat4::Translate(body.mPosition.x, body.mPosition.y, body.mPosition.z)
        };
        
        mEngineRenderer.Render(*body.mRenderable, matrix);
    }
}

void GameSceneRenderer::RenderHud() {
    mEngineRenderer.SetHudMode(true);
    mEngineRenderer.SetLightDirection(mHud.GetLightDirection());

    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetProgressTextRectangle());
    RenderUtils::RenderText(mEngineRenderer, mHud.GetProgressText());
    if (auto* grayBlock {mHud.GetGrayBlock()}) {
        RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetProgressPiecesRectangle());
        RenderTiltedGrayBlockInHud(*grayBlock);
    } else {
        RenderTiltedBlueprintSlotInHud();
    }

    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetMovesTextRectangle());
    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetMovesPiecesRectangle());
    RenderUtils::RenderText(mEngineRenderer, mHud.GetMovesText());
    RenderTiltedLPieceInHud();

    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetNextPiecesRectangle());
    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetNextTextRectangle());
    RenderUtils::RenderText(mEngineRenderer, mHud.GetNextText());

    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetSelectablePiecesRectangle());
    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetSwitchTextRectangle());
    RenderUtils::RenderText(mEngineRenderer, mHud.GetSwitchText());
    
    auto& piecePositions {mHud.GetPieceRelativePositions()};
    auto& nextPiecesPosition {mHud.GetNextPiecesPosition()};
    auto& nextPieces {mGameLogic.GetNextPieceGenerator().GetNext2Pieces()};
    
    RenderPreviewPiece(nextPieces[0], nextPiecesPosition + piecePositions[0]);
    RenderPreviewPiece(nextPieces[1], nextPiecesPosition + piecePositions[1]);

    auto& selectablePiecesPosition {mHud.GetSelectablePiecesPosition()};
    auto& selectablePieces {mGameLogic.GetSelectablePieces()};
    
    RenderPreviewPiece(selectablePieces[0], selectablePiecesPosition + piecePositions[0]);
    RenderPreviewPiece(selectablePieces[1], selectablePiecesPosition + piecePositions[1]);
    
    mEngineRenderer.SetLightDirection(mScene.GetLightDirection());
    mEngineRenderer.SetHudMode(false);
}

void GameSceneRenderer::RenderTiltedGrayBlockInHud(const Pht::RenderableObject& grayBlock) {
    auto position {mHud.GetProgressPosition() + mHud.GetGrayBlockRelativePosition()};
    
    auto baseTransform {
        previewRotationMatrix *
        Pht::Mat4::Translate(position.x, position.y, -1.0f)
    };
    
    auto scale {Pht::Mat4::Scale(mHud.GetGrayBlockSize() / mScene.GetCellSize())};
    auto blockMatrix {scale * baseTransform};

    mEngineRenderer.Render(grayBlock, blockMatrix);
}

void GameSceneRenderer::RenderTiltedBlueprintSlotInHud() {
    auto position {mHud.GetProgressPosition() + mHud.GetBlueprintSlotRelativePosition()};
    
    auto baseTransform {
        previewRotationMatrix *
        Pht::Mat4::Translate(position.x, position.y, -1.0f)
    };
    
    auto scale {Pht::Mat4::Scale(mHud.GetBlueprintSlotSize() / mScene.GetCellSize())};
    auto blockMatrix {scale * baseTransform};

    mEngineRenderer.Render(*mHud.GetBlueprintSlot(), blockMatrix);
}

void GameSceneRenderer::RenderTiltedLPieceInHud() {
    auto& lPiece {mHud.GetLPiece()};

    RenderScaledTiltedPiece(mHud.GetMovesPosition() + mHud.GetLPieceRelativePosition(),
                            mHud.GetLPieceCellSize(),
                            lPiece.GetGridNumRows(),
                            lPiece.GetGridNumColumns(),
                            lPiece.GetGrid(Rotation::Deg0));
}

void GameSceneRenderer::RenderPreviewPiece(const Piece* piece, const Pht::Vec2& position) {
    if (piece == nullptr) {
        return;
    }
    
    RenderScaledTiltedPiece(position,
                            piece->GetPreviewCellSize(),
                            piece->GetGridNumRows(),
                            piece->GetGridNumColumns(),
                            piece->GetGrid(Rotation::Deg0));
}

void GameSceneRenderer::RenderScaledTiltedPiece(const Pht::Vec2& position,
                                                float cellSize,
                                                int pieceNumRows,
                                                int pieceNumColumns,
                                                const CellGrid& pieceGrid) {
    Pht::Vec3 lowerLeft {
        position.x - pieceNumColumns * cellSize / 2.0f,
        position.y - pieceNumRows * cellSize / 2.0f,
        -1.0f
    };
    
    auto baseTransform {
        previewRotationMatrix *
        Pht::Mat4::Translate(lowerLeft.x, lowerLeft.y, lowerLeft.z)
    };
    
    auto scale {Pht::Mat4::Scale(cellSize / mScene.GetCellSize())};
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto renderable {pieceGrid[row][column].mFirstSubCell.mRenderableObject};
            
            if (renderable) {
                auto cellXPos {column * cellSize + cellSize / 2.0f};
                auto cellYPos {row * cellSize + cellSize / 2.0f};
                
                auto cellMatrix {
                    scale * Pht::Mat4::Translate(cellXPos, cellYPos, 0.0f) * baseTransform
                };
                
                mEngineRenderer.Render(*renderable, cellMatrix);
            }
        }
    }
}

void GameSceneRenderer::RenderGameViews() {
    auto& gameMenuController {mGameViewControllers.GetGameMenuController()};
    
    switch (mGameViewControllers.GetActiveController()) {
        case GameViewControllers::None:
            break;
        case GameViewControllers::GameHud:
            mEngineRenderer.RenderGuiView(mGameViewControllers.GetGameHudController().GetView());
            break;
        case GameViewControllers::GameMenu: {
            gameMenuController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(gameMenuController.GetView());
            break;
        }
        case GameViewControllers::NoMovesDialog: {
            auto& noMovesDialogController {mGameViewControllers.GetNoMovesDialogController()};
            noMovesDialogController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(noMovesDialogController.GetView());
            break;
        }
        case GameViewControllers::GameOverDialog: {
            auto& gameOverDialogController {mGameViewControllers.GetGameOverDialogController()};
            gameOverDialogController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(gameOverDialogController.GetView());
            break;
        }
        case GameViewControllers::LevelCompletedDialog:
            RenderLevelCompletedView();
            break;
        case GameViewControllers::SettingsMenu: {
            auto& settingsMenuController {mGameViewControllers.GetSettingsMenuController()};
            gameMenuController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(settingsMenuController.GetView());
            break;
        }
        case GameViewControllers::NoLivesDialog: {
            auto& noLivesDialogController {mGameViewControllers.GetNoLivesDialogController()};
            noLivesDialogController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(noLivesDialogController.GetView());
            break;
        }
        case GameViewControllers::RestartConfirmationDialog: {
            auto& restartConfirmationDialogController {
                mGameViewControllers.GetRestartConfirmationDialogController()
            };
            gameMenuController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(restartConfirmationDialogController.GetView());
            break;
        }
        case GameViewControllers::MapConfirmationDialog:
            auto& mapConfirmationDialogController {
                mGameViewControllers.GetMapConfirmationDialogController()
            };
            gameMenuController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(mapConfirmationDialogController.GetView());
            break;
    }
}

void GameSceneRenderer::RenderLevelCompletedView() {
    auto& levelCompletedDialogController {mGameViewControllers.GetLevelCompletedDialogController()};
    levelCompletedDialogController.GetFadeEffect().Render();
    
    auto& view {levelCompletedDialogController.GetView()};
    mEngineRenderer.SetLightDirection({0.75f, 1.0f, 1.0f});
    mEngineRenderer.RenderGuiView(view);
    mEngineRenderer.SetLightDirection(mScene.GetLightDirection());
}

void GameSceneRenderer::RenderSlidingText() {
    if (mSlidingTextAnimation.GetState() == SlidingTextAnimation::State::Inactive) {
        return;
    }
    
    auto* text {mSlidingTextAnimation.GetText()};
    
    if (text == nullptr) {
        return;
    }
    
    for (const auto& line: text->mTextLines) {
        mEngineRenderer.RenderText(line.mText,
                                   line.mPosition + mSlidingTextAnimation.GetPosition(),
                                   mSlidingTextAnimation.GetTextProperties());
    }
}
