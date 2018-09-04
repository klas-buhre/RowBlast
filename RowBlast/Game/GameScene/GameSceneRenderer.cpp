#include "GameSceneRenderer.hpp"

// Engine includes.
#include "RenderableObject.hpp"

// Game includes.
#include "Field.hpp"
#include "GameLogic.hpp"
#include "Piece.hpp"
#include "FallingPiece.hpp"
#include "GameScene.hpp"
#include "BombsAnimation.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto dz {0.05f};
    constexpr auto numVisibleGridSegments {3};
}

GameSceneRenderer::GameSceneRenderer(GameScene& scene,
                                     const Field& field,
                                     const GameLogic& gameLogic,
                                     const ScrollController& scrollController,
                                     const BombsAnimation& bombsAnimation,
                                     const PieceResources& pieceResources,
                                     const LevelResources& levelResources) :
    mScene {scene},
    mField {field},
    mGameLogic {gameLogic},
    mScrollController {scrollController},
    mBombsAnimation {bombsAnimation},
    mPieceResources {pieceResources},
    mLevelResources {levelResources} {}

void GameSceneRenderer::Render() {
    RenderFieldGrid();
    RenderBlueprintSlots();
    RenderFieldBlocks();
    RenderFallingPiece();
    RenderGhostPieces();
}

void GameSceneRenderer::RenderFieldGrid() {
    auto& gridSegments {mScene.GetFieldGrid().GetSegments()};
    
    for (auto& gridSegment: gridSegments) {
        gridSegment.mSceneObject.SetIsVisible(false);
    }
    
    auto lowestVisibleRow {static_cast<int>(mScrollController.GetLowestVisibleRow())};
    auto numSegments {static_cast<int>(gridSegments.size())};
    
    for (auto i {numSegments - 1}; i >= 0; --i) {
        auto& gridSegment {gridSegments[i]};
        
        if (gridSegment.mRow <= lowestVisibleRow) {
            for (auto j {0}; j < numVisibleGridSegments; ++j) {
                auto visibleSegmentIndex {i + j};
                
                if (visibleSegmentIndex >= numSegments) {
                    break;
                }
                
                gridSegments[visibleSegmentIndex].mSceneObject.SetIsVisible(true);
            }
            
            break;
        }
    }
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
            RenderFieldBlock(cell.mFirstSubCell, false);
            RenderFieldBlock(cell.mSecondSubCell, true);
        }
    }
}

void GameSceneRenderer::RenderFieldBlock(const SubCell& subCell, bool isSecondSubCell) {
    auto blockKind {subCell.mBlockKind};
    
    switch (blockKind) {
        case BlockKind::None:
        case BlockKind::ClearedRowBlock:
            return;
        default:
            break;
    }
    
    auto& sceneObject {mScene.GetFieldBlocks().AccuireSceneObject()};
    const auto cellSize {mScene.GetCellSize()};
    
    Pht::Vec3 blockPosition {
        subCell.mPosition.x * cellSize + cellSize / 2.0f,
        subCell.mPosition.y * cellSize + cellSize / 2.0f,
        subCell.mFallingBlockAnimation.mState == FallingBlockAnimation::State::Bouncing ?
        mScene.GetBouncingBlockZ() : 0.0f
    };

    auto isSomeKindOfBomb {subCell.IsBomb()};
    auto& transform {sceneObject.GetTransform()};
    transform.SetPosition(blockPosition);
    
    if (isSomeKindOfBomb) {
        if (subCell.mBlockKind == BlockKind::Bomb) {
            transform.SetRotation(mBombsAnimation.GetBombStaticRotation());
        } else {
            transform.SetRotation(mBombsAnimation.GetRowBombStaticRotation());
        }
    } else {
        if (blockKind != BlockKind::Full) {
            Pht::Vec3 blockRotation {0.0f, 0.0f, RotationToDeg(subCell.mRotation)};
            transform.SetRotation(blockRotation);
        } else {
            transform.SetRotation({0.0f, 0.0f, 0.0f});
        }
    }
    
    if (subCell.mIsGrayLevelBlock) {
        sceneObject.SetRenderable(&mLevelResources.GetLevelBlockRenderable(blockKind));
    } else if (isSomeKindOfBomb) {
        if (subCell.mBlockKind == BlockKind::Bomb) {
            sceneObject.SetRenderable(&mLevelResources.GetLevelBombRenderable());
        } else {
            sceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
        }
    } else {
        auto color {subCell.mColor};
        auto brightness {subCell.mFlashingBlockAnimation.mBrightness};

        auto& renderableObject {
            mPieceResources.GetBlockRenderableObject(blockKind, color, brightness)
        };
        
        sceneObject.SetRenderable(&renderableObject);
        
        RenderBlockWelds(subCell, blockPosition, mScene.GetFieldBlocks(), isSecondSubCell);
    }
}

void GameSceneRenderer::RenderBlockWelds(const SubCell& subCell,
                                         const Pht::Vec3& blockPos,
                                         SceneObjectPool& pool,
                                         bool isSecondSubCell) {
    auto& welds {subCell.mWelds};
    auto& weldAnimations {welds.mAnimations};
    const auto cellSize {mScene.GetCellSize()};
    auto weldZ {blockPos.z + cellSize / 2.0f};
    
    if (welds.mUpLeft || weldAnimations.mUpLeft.IsActive()) {
        RenderBlockWeld({blockPos.x - cellSize / 2.0f, blockPos.y + cellSize / 2.0f, weldZ},
                        45.0f,
                        weldAnimations.mUpLeft.mScale,
                        GetWeldRenderable(WeldRenderableKind::Aslope, subCell, weldAnimations.mUpLeft),
                        pool);
    }
    
    if (welds.mUp || weldAnimations.mUp.IsActive()) {
        RenderBlockWeld({blockPos.x, blockPos.y + cellSize / 2.0f, weldZ},
                        -90.0f,
                        weldAnimations.mUp.mScale,
                        GetWeldRenderable(WeldRenderableKind::Normal, subCell, weldAnimations.mUp),
                        pool);
    }
    
    if (welds.mUpRight || weldAnimations.mUpRight.IsActive()) {
        RenderBlockWeld({blockPos.x + cellSize / 2.0f, blockPos.y + cellSize / 2.0f, weldZ},
                        -45.0f,
                        weldAnimations.mUpRight.mScale,
                        GetWeldRenderable(WeldRenderableKind::Aslope, subCell, weldAnimations.mUpRight),
                        pool);
    }

    if (welds.mRight || weldAnimations.mRight.IsActive()) {
        RenderBlockWeld({blockPos.x + cellSize / 2.0f, blockPos.y, weldZ},
                        0.0f,
                        weldAnimations.mRight.mScale,
                        GetWeldRenderable(WeldRenderableKind::Normal, subCell, weldAnimations.mRight),
                        pool);
    }

    if (welds.mDiagonal && isSecondSubCell) {
        auto& diagonalAnimation {weldAnimations.mDiagonal};
        auto color {subCell.mColor};
        auto brightness {
            diagonalAnimation.IsSemiFlashing() ?
            BlockBrightness::SemiFlashing : subCell.mFlashingBlockAnimation.mBrightness
        };
        
        auto& diagonalWeldRenderable {
            mPieceResources.GetWeldRenderableObject(WeldRenderableKind::Diagonal, color, brightness)
        };
        
        auto weldScale {weldAnimations.mDiagonal.mScale};
        
        switch (subCell.mFill) {
            case Fill::LowerRightHalf:
            case Fill::UpperLeftHalf:
                RenderBlockWeld({blockPos.x, blockPos.y, weldZ},
                                -45.0f,
                                weldScale,
                                diagonalWeldRenderable,
                                pool);
                break;
            case Fill::LowerLeftHalf:
            case Fill::UpperRightHalf:
                RenderBlockWeld({blockPos.x, blockPos.y, weldZ},
                                45.0f,
                                weldScale,
                                diagonalWeldRenderable,
                                pool);
                break;
            default:
                break;
        }
    }
}

void GameSceneRenderer::RenderBlockWeld(const Pht::Vec3& weldPosition,
                                        float rotation,
                                        float scale,
                                        Pht::RenderableObject& weldRenderableObject,
                                        SceneObjectPool& pool) {
    auto& sceneObject {pool.AccuireSceneObject()};
    auto& transform {sceneObject.GetTransform()};
    transform.SetRotation({0.0f, 0.0f, rotation});
    transform.SetScale({scale, 1.0f, 1.0f});
    transform.SetPosition(weldPosition);
    sceneObject.SetRenderable(&weldRenderableObject);
}

Pht::RenderableObject& GameSceneRenderer::GetWeldRenderable(WeldRenderableKind renderableKind,
                                                            const SubCell& subCell,
                                                            const WeldAnimation& weldAnimation) {
    auto color {subCell.mColor};
    
    auto brightness {
        weldAnimation.IsSemiFlashing() ?
        BlockBrightness::SemiFlashing : subCell.mFlashingBlockAnimation.mBrightness
    };
    
    return mPieceResources.GetWeldRenderableObject(renderableKind, color, brightness);
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
    
    auto isTransparent {false};
    auto isGhostPiece {false};
    RenderPieceBlocks(pieceGrid,
                      pieceFieldPos,
                      isTransparent,
                      isGhostPiece,
                      mScene.GetPieceBlocks());
}

void GameSceneRenderer::RenderPieceBlocks(const CellGrid& pieceBlocks,
                                          const Pht::Vec3& pieceFieldPos,
                                          bool isTransparent,
                                          bool isGhostPiece,
                                          SceneObjectPool& pool) {
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    assert(fallingPiece);
    
    auto& pieceType {fallingPiece->GetPieceType()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    const auto cellSize {mScene.GetCellSize()};
    auto isBomb {pieceType.IsBomb()};
    auto isRowBomb {pieceType.IsRowBomb()};
    const Pht::Vec3 pieceGridSize {pieceNumColumns * cellSize, pieceNumRows * cellSize, 0.0f};
    
    if (!isGhostPiece) {
        auto& containerObject {pool.GetContainerSceneObject()};
        containerObject.GetTransform().SetPosition(pieceFieldPos + pieceGridSize / 2.0f);
    }
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto& subCell {pieceBlocks[row][column].mFirstSubCell};
            auto blockKind {subCell.mBlockKind};
            
            if (blockKind == BlockKind::None) {
                continue;
            }
            
            auto blockPosition {
                isGhostPiece ?
                Pht::Vec3 {
                    pieceFieldPos.x + column * cellSize + cellSize / 2.0f,
                    pieceFieldPos.y + row * cellSize + cellSize / 2.0f,
                    pieceFieldPos.z
                } :
                Pht::Vec3 {
                    column * cellSize + cellSize / 2.0f - pieceGridSize.x / 2.0f,
                    row * cellSize + cellSize / 2.0f - pieceGridSize.y / 2.0f,
                    0.0f
                }
            };
            
            auto& sceneObject {pool.AccuireSceneObject()};
            auto& transform {sceneObject.GetTransform()};
            transform.SetPosition(blockPosition);

            if (blockKind != BlockKind::Full) {
                Pht::Vec3 blockRotation {0.0f, 0.0f, RotationToDeg(subCell.mRotation)};
                transform.SetRotation(blockRotation);
            } else {
                transform.SetRotation({0.0f, 0.0f, 0.0f});
            }
            
            if (isBomb) {
                sceneObject.GetTransform().SetRotation(mBombsAnimation.GetBombRotation());
                
                if (isTransparent) {
                    sceneObject.SetRenderable(&mPieceResources.GetTransparentBombRenderableObject());
                } else {
                    sceneObject.SetRenderable(&mPieceResources.GetBombRenderableObject());
                }
            } else if (isRowBomb) {
                sceneObject.GetTransform().SetRotation(mBombsAnimation.GetRowBombRotation());
                
                if (isTransparent) {
                    sceneObject.SetRenderable(&mPieceResources.GetTransparentRowBombRenderableObject());
                } else {
                    sceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
                }
            } else {
                auto color {subCell.mColor};
                auto brightness {BlockBrightness::Normal};

                auto& blockRenderableObject {
                    mPieceResources.GetBlockRenderableObject(blockKind, color, brightness)
                };
                
                sceneObject.SetRenderable(&blockRenderableObject);

                RenderBlockWelds(subCell, blockPosition, pool, false);
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
        auto isGhostPiece {true};
        RenderPieceBlocks(pieceGrid,
                          position,
                          isTransparent,
                          isGhostPiece,
                          mScene.GetGhostPieces());
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
    auto* moveAlternatives {mGameLogic.GetClickInputHandler().GetVisibleMoves()};
    
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
        
        if (move.mIsHidden) {
            continue;
        }
        
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
            auto isGhostPiece {true};
            
            if (isMoveButtonDown) {
                Pht::Vec3 position {ghostPieceFieldPos};
                position.z = mScene.GetPressedGhostPieceZ();
                auto isTransparent {false};
                RenderPieceBlocks(pieceGrid, position, isTransparent, isGhostPiece, pool);
            } else {
                auto isTransparent {true};
                RenderPieceBlocks(pieceGrid, ghostPieceFieldPos, isTransparent, isGhostPiece, pool);
            }
        }
    }
}
