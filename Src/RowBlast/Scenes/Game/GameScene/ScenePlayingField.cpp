#include "ScenePlayingField.hpp"

// Engine includes.
#include "RenderableObject.hpp"

// Game includes.
#include "Field.hpp"
#include "GameLogic.hpp"
#include "Piece.hpp"
#include "FallingPiece.hpp"
#include "GameScene.hpp"
#include "BombsAnimation.hpp"
#include "AsteroidAnimation.hpp"
#include "ValidAreaAnimation.hpp"
#include "PieceResources.hpp"
#include "GhostPieceBlocks.hpp"
#include "LevelResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto dz = 0.025f;
    constexpr auto numVisibleGridRows = 17;
    
    float GhostPieceTriangleBlockRotationToDeg(BlockKind blockKind, Rotation rotation) {
        auto baseRotation = 0.0f;
        switch (blockKind) {
            case BlockKind::LowerLeftHalf:
                baseRotation = -90.0f;
                break;
            case BlockKind::UpperLeftHalf:
                baseRotation = -180.0f;
                break;
            case BlockKind::UpperRightHalf:
                baseRotation = -270.0f;
                break;
            default:
                break;
        }
        
        return baseRotation + RotationToDeg(rotation);
    }
    
    void UpdateBlockBond(const Pht::Vec3& bondPosition,
                         float rotation,
                         float scale,
                         Pht::RenderableObject& bondRenderableObject,
                         SceneObjectPool& pool) {
        auto& sceneObject = pool.AccuireSceneObject();
        auto& transform = sceneObject.GetTransform();
        transform.SetRotation({0.0f, 0.0f, rotation});
        transform.SetScale({scale, 1.0f, 1.0f});
        transform.SetPosition(bondPosition);
        sceneObject.SetRenderable(&bondRenderableObject);
    }
}

ScenePlayingField::ScenePlayingField(GameScene& scene,
                                     const Field& field,
                                     const GameLogic& gameLogic,
                                     const ScrollController& scrollController,
                                     const BombsAnimation& bombsAnimation,
                                     AsteroidAnimation& asteroidAnimation,
                                     const FallingPieceAnimation& fallingPieceAnimation,
                                     const DraggedPieceAnimation& draggedPieceAnimation,
                                     const ValidAreaAnimation& validAreaAnimation,
                                     const PieceResources& pieceResources,
                                     const GhostPieceBlocks& ghostPieceBlocks,
                                     const LevelResources& levelResources) :
    mScene {scene},
    mField {field},
    mGameLogic {gameLogic},
    mScrollController {scrollController},
    mBombsAnimation {bombsAnimation},
    mAsteroidAnimation {asteroidAnimation},
    mFallingPieceAnimation {fallingPieceAnimation},
    mDraggedPieceAnimation {draggedPieceAnimation},
    mValidAreaAnimation {validAreaAnimation},
    mPieceResources {pieceResources},
    mGhostPieceBlocks {ghostPieceBlocks},
    mLevelResources {levelResources} {}

void ScenePlayingField::Update() {
    UpdateFieldGrid();
    UpdateBlueprintSlots();
    UpdateValidArea();
    UpdateFieldBlocks();
    UpdateFallingPiece();
    UpdateDraggedPiece();
    UpdateGhostPieces();
}

void ScenePlayingField::UpdateFieldGrid() {
    auto& gridSegments = mScene.GetFieldGrid().GetSegments();
    for (auto& gridSegment: gridSegments) {
        gridSegment.mSceneObject.SetIsVisible(false);
    }
    
    auto lowestVisibleRow = static_cast<int>(mScrollController.GetLowestVisibleRow());
    auto pastHighestVisibleRow = lowestVisibleRow + numVisibleGridRows;
    auto numSegments = static_cast<int>(gridSegments.size());
    
    for (auto i = numSegments - 1; i >= 0; --i) {
        auto& gridSegment = gridSegments[i];
        if (gridSegment.mRow <= lowestVisibleRow) {
            for (auto visibleSegmentIndex = i;
                 visibleSegmentIndex < numSegments;
                 ++visibleSegmentIndex) {

                auto& visibleGridSegment = gridSegments[visibleSegmentIndex];
                if (visibleGridSegment.mRow >= pastHighestVisibleRow) {
                    break;
                }
                
                visibleGridSegment.mSceneObject.SetIsVisible(true);
            }
            
            break;
        }
    }
}

void ScenePlayingField::UpdateBlueprintSlots() {
    auto* blueprintGrid = mField.GetBlueprintGrid();
    if (blueprintGrid == nullptr) {
        return;
    }
    
    auto lowestVisibleRow = static_cast<int>(mScrollController.GetLowestVisibleRow());
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    
    for (auto row = 0; row < pastHighestVisibleRow; row++) {
        for (auto column = 0; column < mField.GetNumColumns(); column++) {
            auto& blueprintCell = (*blueprintGrid)[row][column];
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

void ScenePlayingField::UpdateValidArea() {
    auto& invalidCellsPool = mScene.GetInvalidCells();
    invalidCellsPool.ReclaimAll();

    if (!mValidAreaAnimation.IsActive()) {
        return;
    }
    
    auto lowestVisibleRow = static_cast<int>(mScrollController.GetLowestVisibleRow());
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    const auto cellSize = mScene.GetCellSize();
    auto invaldCellZ = mScene.GetInvalidCellZ();
    
    for (auto row = lowestVisibleRow; row < pastHighestVisibleRow; row++) {
        for (auto column = 0; column < mField.GetNumColumns(); column++) {
            if (mValidAreaAnimation.IsCellInvalid(row, column)) {
                auto& sceneObject = invalidCellsPool.AccuireSceneObject();
                sceneObject.SetRenderable(&mLevelResources.GetInvalidCellRenderable());
                
                Pht::Vec3 position {
                    column * cellSize + cellSize / 2.0f,
                    row * cellSize + cellSize / 2.0f,
                    invaldCellZ
                };
                
                sceneObject.GetTransform().SetPosition(position);
            }
        }
    }
}

void ScenePlayingField::UpdateFieldBlocks() {
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
    
    auto pastHighestVisibleRow = lowestVisibleRow + mField.GetNumRowsInOneScreen();
    
    for (auto row = lowestVisibleRow; row < pastHighestVisibleRow; row++) {
        for (auto column = 0; column < mField.GetNumColumns(); column++) {
            auto& cell = mField.GetCell(row, column);
            UpdateFieldBlock(cell.mFirstSubCell, false);
            UpdateFieldBlock(cell.mSecondSubCell, true);
        }
    }
}

void ScenePlayingField::UpdateFieldBlock(const SubCell& subCell, bool isSecondSubCell) {
    auto blockKind = subCell.mBlockKind;
    switch (blockKind) {
        case BlockKind::None:
        case BlockKind::ClearedRowBlock:
        case BlockKind::BigAsteroid:
            return;
        default:
            break;
    }
    
    auto& sceneObject = mScene.GetFieldBlocks().AccuireSceneObject();
    const auto cellSize = mScene.GetCellSize();
    
    Pht::Vec3 blockPosition {
        subCell.mPosition.x * cellSize + cellSize / 2.0f,
        subCell.mPosition.y * cellSize + cellSize / 2.0f,
        subCell.mFallingBlockAnimation.mState == FallingBlockAnimationComponent::State::Bouncing ?
        mScene.GetBouncingBlockZ() : 0.0f
    };

    auto& transform = sceneObject.GetTransform();
    transform.SetPosition(blockPosition);
    
    switch (blockKind) {
        case BlockKind::Bomb:
            transform.SetRotation(mBombsAnimation.GetBombStaticRotation());
            sceneObject.SetRenderable(&mLevelResources.GetLevelBombRenderable());
            break;
        case BlockKind::RowBomb:
            transform.SetRotation(mBombsAnimation.GetRowBombStaticRotation());
            sceneObject.SetRenderable(&mPieceResources.GetRowBombRenderableObject());
            break;
        case BlockKind::BigAsteroidMainCell:
            transform.Translate({cellSize / 2.0f, cellSize / 2.0f, mScene.GetBigAsteroidZ()});
            sceneObject.SetRenderable(&mLevelResources.GetBigAsteroidRenderable());
            mAsteroidAnimation.SetSceneObject(&sceneObject);
            break;
        case BlockKind::SmallAsteroid:
            sceneObject.SetRenderable(&mLevelResources.GetSmallAsteroidRenderable());
            mAsteroidAnimation.SetSceneObject(&sceneObject);
            break;
        default:
            if (blockKind != BlockKind::Full) {
                Pht::Vec3 blockRotation {0.0f, 0.0f, RotationToDeg(subCell.mRotation)};
                transform.SetRotation(blockRotation);
            } else {
                transform.SetRotation({0.0f, 0.0f, 0.0f});
            }
            if (subCell.mIsGrayLevelBlock) {
                sceneObject.SetRenderable(&mLevelResources.GetLevelBlockRenderable(blockKind));
            } else {
                auto color = subCell.mColor;
                auto brightness = subCell.mFlashingBlockAnimation.mBrightness;
                auto& renderableObject =
                    mPieceResources.GetBlockRenderableObject(blockKind, color, brightness);
                sceneObject.SetRenderable(&renderableObject);
                UpdateBlockBonds(subCell, blockPosition, mScene.GetFieldBlocks(), isSecondSubCell);
            }
            break;
    }
}

void ScenePlayingField::UpdateBlockBonds(const SubCell& subCell,
                                         const Pht::Vec3& blockPos,
                                         SceneObjectPool& pool,
                                         bool isSecondSubCell) {
    auto& bonds = subCell.mBonds;
    auto& bondAnimations = subCell.mBondAnimations;
    const auto cellSize = mScene.GetCellSize();
    auto bondZ = blockPos.z + cellSize / 2.0f;
    
    if (bonds.mUpLeft || bondAnimations.mUpLeft.IsActive()) {
        UpdateBlockBond({blockPos.x - cellSize / 2.0f, blockPos.y + cellSize / 2.0f, bondZ},
                        45.0f,
                        bondAnimations.mUpLeft.mScale,
                        GetBondRenderable(BondRenderableKind::Aslope, subCell, bondAnimations.mUpLeft),
                        pool);
    }
    
    if (bonds.mUp || bondAnimations.mUp.IsActive()) {
        UpdateBlockBond({blockPos.x, blockPos.y + cellSize / 2.0f, bondZ},
                        -90.0f,
                        bondAnimations.mUp.mScale,
                        GetBondRenderable(BondRenderableKind::Normal, subCell, bondAnimations.mUp),
                        pool);
    }
    
    if (bonds.mUpRight || bondAnimations.mUpRight.IsActive()) {
        UpdateBlockBond({blockPos.x + cellSize / 2.0f, blockPos.y + cellSize / 2.0f, bondZ},
                        -45.0f,
                        bondAnimations.mUpRight.mScale,
                        GetBondRenderable(BondRenderableKind::Aslope, subCell, bondAnimations.mUpRight),
                        pool);
    }

    if (bonds.mRight || bondAnimations.mRight.IsActive()) {
        UpdateBlockBond({blockPos.x + cellSize / 2.0f, blockPos.y, bondZ},
                        0.0f,
                        bondAnimations.mRight.mScale,
                        GetBondRenderable(BondRenderableKind::Normal, subCell, bondAnimations.mRight),
                        pool);
    }

    if (bonds.mDiagonal && isSecondSubCell) {
        auto& diagonalAnimation = bondAnimations.mDiagonal;
        auto color = subCell.mColor;
        
        auto brightness =
            diagonalAnimation.IsSemiFlashing() ?
            BlockBrightness::SemiFlashing : subCell.mFlashingBlockAnimation.mBrightness;
        
        auto& diagonalBondRenderable =
            mPieceResources.GetBondRenderableObject(BondRenderableKind::Diagonal, color, brightness);
        
        auto bondScale {bondAnimations.mDiagonal.mScale};
        
        switch (subCell.mFill) {
            case Fill::LowerRightHalf:
            case Fill::UpperLeftHalf:
                UpdateBlockBond({blockPos.x, blockPos.y, bondZ},
                                -45.0f,
                                bondScale,
                                diagonalBondRenderable,
                                pool);
                break;
            case Fill::LowerLeftHalf:
            case Fill::UpperRightHalf:
                UpdateBlockBond({blockPos.x, blockPos.y, bondZ},
                                45.0f,
                                bondScale,
                                diagonalBondRenderable,
                                pool);
                break;
            default:
                break;
        }
    }
}

Pht::RenderableObject& ScenePlayingField::GetBondRenderable(BondRenderableKind renderableKind,
                                                            const SubCell& subCell,
                                                            const BondAnimation& bondAnimation) {
    auto color = subCell.mColor;
    
    auto brightness =
        bondAnimation.IsSemiFlashing() ?
        BlockBrightness::SemiFlashing : subCell.mFlashingBlockAnimation.mBrightness;
    
    return mPieceResources.GetBondRenderableObject(renderableKind, color, brightness);
}

void ScenePlayingField::UpdateFallingPiece() {
    mScene.GetPieceBlocks().ReclaimAll();
    
    auto* fallingPiece = mGameLogic.GetFallingPiece();
    if (fallingPiece == nullptr) {
        return;
    }
    
    auto pieceGridPos = CalculateFallingPieceGridPosition(*fallingPiece);
    const auto cellSize = mScene.GetCellSize();
    Pht::Vec3 pieceFieldPos {pieceGridPos.x * cellSize, pieceGridPos.y * cellSize, 0.0f};
    auto isTransparent = false;
    auto isGhostPiece = false;
    UpdatePieceBlocks(fallingPiece->GetPieceType(),
                      fallingPiece->GetRotation(),
                      pieceFieldPos,
                      isTransparent,
                      isGhostPiece,
                      mScene.GetPieceBlocks());
}

Pht::Vec2 ScenePlayingField::CalculateFallingPieceGridPosition(const FallingPiece& fallingPiece) {
    if (mFallingPieceAnimation.GetState() == FallingPieceAnimation::State::Active ||
        mGameLogic.GetControlType() == ControlType::Click) {
        
        return fallingPiece.GetRenderablePositionSmoothY();
    }
    
    return fallingPiece.GetRenderablePosition();
}

void ScenePlayingField::UpdateDraggedPiece() {
    mScene.GetDraggedPieceBlocks().ReclaimAll();
    auto& sceneObject = mScene.GetDraggedPieceSceneObject();
    sceneObject.SetIsVisible(false);

    auto* draggedPiece = mGameLogic.GetDraggedPiece();
    if (draggedPiece == nullptr) {
        return;
    }
    
    auto& piecePosition = draggedPiece->GetRenderablePosition();
    Pht::Vec3 pieceFieldPos {piecePosition.x, piecePosition.y, mScene.GetDraggedPieceZ()};
    
    auto yOffsetAdjustmentInCells =
        DraggedPieceAnimation::targetYOffsetInCells - mDraggedPieceAnimation.GetYOffsetInCells();

    const auto cellSize = mScene.GetCellSize();
    pieceFieldPos.y -= yOffsetAdjustmentInCells * cellSize;
    
    auto& pieceType = draggedPiece->GetPieceType();
    auto* ghostPieceRenderable = pieceType.GetPressedGhostPieceRenderable();
    if (ghostPieceRenderable) {
        sceneObject.SetIsVisible(true);
        
        Pht::Vec3 pieceCenterLocalCoords {
            cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
            cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f,
            0.0f
        };
        
        auto scale = draggedPiece->GetScale();
        auto& transform = sceneObject.GetTransform();
        auto position = pieceFieldPos + pieceCenterLocalCoords * scale;
        transform.SetPosition(position);
        transform.SetRotation({0.0f, 0.0f, RotationToDeg(draggedPiece->GetRotation())});
        transform.SetScale(scale);
        sceneObject.SetRenderable(ghostPieceRenderable);
    } else {
        auto isTransparent = false;
        auto isGhostPiece = false;
        UpdatePieceBlocks(draggedPiece->GetPieceType(),
                          draggedPiece->GetRotation(),
                          pieceFieldPos,
                          isTransparent,
                          isGhostPiece,
                          mScene.GetDraggedPieceBlocks());

    }
}

void ScenePlayingField::UpdatePieceBlocks(const Piece& pieceType,
                                          Rotation rotation,
                                          const Pht::Vec3& pieceFieldPos,
                                          bool isTransparent,
                                          bool isGhostPiece,
                                          SceneObjectPool& pool) {
    auto& pieceBlocks = pieceType.GetGrid(rotation);
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    const auto cellSize = mScene.GetCellSize();
    auto isBomb = pieceType.IsBomb();
    auto isRowBomb = pieceType.IsRowBomb();
    const Pht::Vec3 pieceGridSize {pieceNumColumns * cellSize, pieceNumRows * cellSize, 0.0f};
    
    if (!isGhostPiece) {
        auto& containerObject = pool.GetContainerSceneObject();
        containerObject.GetTransform().SetPosition(pieceFieldPos + pieceGridSize / 2.0f);
    }
    
    for (auto row = 0; row < pieceNumRows; row++) {
        for (auto column = 0; column < pieceNumColumns; column++) {
            auto& subCell = pieceBlocks[row][column].mFirstSubCell;
            auto blockKind = subCell.mBlockKind;
            if (blockKind == BlockKind::None) {
                continue;
            }
            
            auto blockPosition =
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
                };
            
            auto& sceneObject = pool.AccuireSceneObject();
            auto& transform = sceneObject.GetTransform();
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
                auto color = subCell.mColor;
                auto brightness = BlockBrightness::Normal;

                auto& blockRenderableObject =
                    mPieceResources.GetBlockRenderableObject(blockKind, color, brightness);
                
                sceneObject.SetRenderable(&blockRenderableObject);

                UpdateBlockBonds(subCell, blockPosition, pool, false);
            }
        }
    }
}

void ScenePlayingField::UpdateGhostPieces() {
    mScene.GetGhostPieces().ReclaimAll();
    mScene.GetGhostPieceBlocks().ReclaimAll();
    
    auto* fallingPiece = mGameLogic.GetFallingPiece();
    if (fallingPiece == nullptr ||
        mFallingPieceAnimation.GetState() != FallingPieceAnimation::State::Inactive) {

        return;
    }
    
    if (mGameLogic.GetControlType() == ControlType::Click) {
        UpdateClickableGhostPieces(*fallingPiece);
    } else {
        auto* draggedPiece = mGameLogic.GetDraggedPiece();
        if (draggedPiece) {
            auto draggedGhostPieceRow = mGameLogic.GetDraggedGhostPieceRow();
            if (draggedGhostPieceRow.HasValue()) {
                UpdateGhostPieceForGestureControls(draggedPiece->GetPieceType(),
                                                   draggedPiece->GetFieldGridPosition().x,
                                                   draggedGhostPieceRow.GetValue(),
                                                   draggedPiece->GetRotation());
            }
        } else {
            if (mGameLogic.GetControlType() == ControlType::Swipe &&
                mGameLogic.IsSwipeGhostPieceEnabled()) {
    
                UpdateGhostPieceForGestureControls(fallingPiece->GetPieceType(),
                                                   fallingPiece->GetRenderablePosition().x,
                                                   mGameLogic.GetGhostPieceRow(),
                                                   fallingPiece->GetRotation());
            }
        }
    }
}

void ScenePlayingField::UpdateGhostPieceForGestureControls(const Piece& pieceType,
                                                           int column,
                                                           int ghostPieceRow,
                                                           Rotation rotation) {
    const auto cellSize = mScene.GetCellSize();
    
    Pht::Vec3 ghostPieceFieldPos {
        static_cast<float>(column) * cellSize,
        ghostPieceRow * cellSize,
        mScene.GetGhostPieceZ()
    };
    
    if (pieceType.GetGhostPieceRenderable()) {
        auto& pieceGrid = pieceType.GetGrid(rotation);
        UpdateGhostPieceBlocks(pieceGrid, ghostPieceFieldPos);
    } else {
        Pht::Vec3 position {ghostPieceFieldPos};
        position.z = mScene.GetPressedGhostPieceZ();
        auto isTransparent = true;
        auto isGhostPiece = true;
        UpdatePieceBlocks(pieceType,
                          rotation,
                          position,
                          isTransparent,
                          isGhostPiece,
                          mScene.GetGhostPieces());
    }
}

void ScenePlayingField::UpdateGhostPiece(Pht::RenderableObject& ghostPieceRenderable,
                                         const Pht::Vec3& position,
                                         Rotation rotation) {
    auto& sceneObject = mScene.GetGhostPieces().AccuireSceneObject();
    
    auto& transform = sceneObject.GetTransform();
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, RotationToDeg(rotation)});
    
    sceneObject.SetRenderable(&ghostPieceRenderable);
}

void ScenePlayingField::UpdateClickableGhostPieces(const FallingPiece& fallingPiece) {
    auto* moveAlternatives = mGameLogic.GetClickInputHandler().GetVisibleMoves();
    if (moveAlternatives == nullptr) {
        return;
    }
    
    const auto cellSize = mScene.GetCellSize();
    auto ghostPieceZ = mScene.GetGhostPieceZ();
    auto ghostPieceShadowZ = mScene.GetGhostPieceShadowZ();
    auto ghostPieceShadowOffset = mScene.GetGhostPieceShadowOffset();
    auto& pieceType = fallingPiece.GetPieceType();
    auto* ghostPieceRenderable = pieceType.GetGhostPieceRenderable();
    auto* ghostPieceShadowRenderable = pieceType.GetGhostPieceShadowRenderable();
    
    Pht::Vec3 ghostPieceCenterLocalCoords {
        cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
        cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f,
        0.0f
    };
    
    for (auto i = 0; i < moveAlternatives->Size(); ++i) {
        auto& move = moveAlternatives->At(i);
        if (move.mIsHidden) {
            continue;
        }
        
        Pht::Vec3 ghostPieceFieldPos {
            static_cast<float>(move.mPosition.x) * cellSize,
            static_cast<float>(move.mPosition.y) * cellSize,
            ghostPieceZ + static_cast<float>(i) * dz
        };
        
        auto isMoveButtonDown = move.mButton->GetButton().IsDown();
        
        if (ghostPieceRenderable) {
            Pht::Vec3 position {ghostPieceFieldPos + ghostPieceCenterLocalCoords};
            auto rotation = move.mRotation;
            
            if (isMoveButtonDown) {
                UpdateGhostPiece(*pieceType.GetPressedGhostPieceRenderable(), position, rotation);
            } else {
                UpdateGhostPiece(*ghostPieceRenderable, position, rotation);
            }

            if (ghostPieceShadowRenderable) {
                position.x += ghostPieceShadowOffset.x;
                position.y += ghostPieceShadowOffset.y;
                position.z = ghostPieceShadowZ;
                // UpdateGhostPiece(*ghostPieceShadowRenderable, position, rotation);
            }
        } else {
            auto& pool = mScene.GetGhostPieces();
            auto isGhostPiece = true;
            
            if (isMoveButtonDown) {
                Pht::Vec3 position {ghostPieceFieldPos};
                position.z = mScene.GetPressedGhostPieceZ();
                auto isTransparent = false;
                UpdatePieceBlocks(pieceType,
                                  move.mRotation,
                                  position,
                                  isTransparent,
                                  isGhostPiece,
                                  pool);
            } else {
                auto isTransparent = true;
                UpdatePieceBlocks(pieceType,
                                  move.mRotation,
                                  ghostPieceFieldPos,
                                  isTransparent,
                                  isGhostPiece,
                                  pool);
            }
        }
    }
}

void ScenePlayingField::UpdateGhostPieceBlocks(const CellGrid& pieceBlocks,
                                               const Pht::Vec3& ghostPieceFieldPos) {
    auto* fallingPiece = mGameLogic.GetFallingPiece();
    assert(fallingPiece);
    
    auto& pieceType = fallingPiece->GetPieceType();
    auto pieceNumRows = pieceType.GetGridNumRows();
    auto pieceNumColumns = pieceType.GetGridNumColumns();
    const auto cellSize = mScene.GetCellSize();
    const Pht::Vec3 pieceGridSize {pieceNumColumns * cellSize, pieceNumRows * cellSize, 0.0f};

    auto& pool = mScene.GetGhostPieceBlocks();
    auto& containerObject = pool.GetContainerSceneObject();
    containerObject.GetTransform().SetPosition(ghostPieceFieldPos + pieceGridSize / 2.0f);

    for (auto row = 0; row < pieceNumRows; row++) {
        for (auto column = 0; column < pieceNumColumns; column++) {
            auto& subCell = pieceBlocks[row][column].mFirstSubCell;
            auto blockKind = subCell.mBlockKind;
            if (blockKind == BlockKind::None) {
                continue;
            }
            
            Pht::Vec3 blockPosition {
                column * cellSize + cellSize / 2.0f - pieceGridSize.x / 2.0f,
                row * cellSize + cellSize / 2.0f - pieceGridSize.y / 2.0f,
                0.0f
            };
            
            auto& sceneObject = pool.AccuireSceneObject();
            auto& transform = sceneObject.GetTransform();
            transform.SetPosition(blockPosition);
            
            if (blockKind != BlockKind::Full) {
                Pht::Vec3 blockRotation {
                    0.0f,
                    0.0f,
                    GhostPieceTriangleBlockRotationToDeg(blockKind, subCell.mRotation)
                };
                transform.SetRotation(blockRotation);
            } else {
                transform.SetRotation({0.0f, 0.0f, 0.0f});
            }
            
            auto color = subCell.mColor;
            auto& blockRenderableObject = mGhostPieceBlocks.GetBlockRenderableObject(blockKind, color);
            
            sceneObject.SetRenderable(&blockRenderableObject);
        }
    }
}
