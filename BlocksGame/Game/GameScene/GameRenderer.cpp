#include "GameRenderer.hpp"

#include <array>

// Engine includes.
#include "IRenderer.hpp"

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
#include "PieceDropParticleEffect.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"

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
    const auto previewRotationMatrix {Pht::Mat4::RotateX(30.0f) * Pht::Mat4::RotateY(30.0f)};
    const auto dz {0.05f};
}

GameRenderer::GameRenderer(Pht::IRenderer& engineRenderer,
                           const Field& field, 
                           const GameLogic& gameLogic,
                           const ExplosionParticleEffect& explosionParticleEffect,
                           const RowExplosionParticleEffect& rowExplosionParticleEffect,
                           const FlyingBlocksAnimation& flyingBlocksAnimation,
                           const SlidingTextAnimation& slidingTextAnimation,
                           const PieceDropParticleEffect& pieceDropParticleEffect,
                           const GameScene& scene,
                           const ScrollController& scrollController,
                           const GameHud& hud,
                           const GameViewControllers& gameViewControllers) :
    mEngineRenderer {engineRenderer},
    mField {field},
    mGameLogic {gameLogic},
    mExplosionParticleEffect {explosionParticleEffect},
    mRowExplosionParticleEffect {rowExplosionParticleEffect},
    mFlyingBlocksAnimation {flyingBlocksAnimation},
    mSlidingTextAnimation {slidingTextAnimation},
    mPieceDropParticleEffect {pieceDropParticleEffect},
    mScene {scene},
    mScrollController {scrollController},
    mHud {hud},
    mGameViewControllers {gameViewControllers} {}

void GameRenderer::RenderFrame() {
    mEngineRenderer.SetProjectionMode(Pht::ProjectionMode::Perspective);
    RenderUtils::RenderSceneObject(mEngineRenderer, mScene.GetBackground());
    RenderUtils::RenderFloatingCubes(mEngineRenderer, mScene.GetFloatingCubes());

    mEngineRenderer.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    RenderField();
    RenderFieldBlueprintSlots();
    RenderFieldBlueprintSlotsAnimation();
    RenderPieceDropParticles();
    RenderFieldBlocks();
    RenderFallingPiece();
    RenderGhostPieces();
    RenderExplosion();
    RenderRowExplosion();
    RenderFlyingBlocks();
    RenderSlidingText();    
    RenderHud();
    RenderGameViews();
}

void GameRenderer::RenderField() {
    RenderUtils::RenderSceneObject(mEngineRenderer, mScene.GetLowerFieldClipQuad());
    RenderUtils::RenderSceneObject(mEngineRenderer, mScene.GetFieldQuad());
}

void GameRenderer::RenderFieldBlueprintSlots() {
    auto* blueprintGrid {mField.GetBlueprintGrid()};
    
    if (blueprintGrid == nullptr) {
        return;
    }
    
    auto lowestVisibleRow {static_cast<int>(mScrollController.GetLowestVisibleRow())};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    
    for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; row++) {
        for (auto column {0}; column < mField.GetNumColumns(); column++) {
            auto& blueprintCell {(*blueprintGrid)[row][column]};
            
            if (blueprintCell.mFill == Fill::Empty || blueprintCell.mAnimation.mIsActive) {
                continue;
            }
            
            RenderFieldBlueprintSlot(blueprintCell, row, column);
        }
    }
}

void GameRenderer::RenderFieldBlueprintSlot(const BlueprintCell& blueprintCell,
                                            int row,
                                            int column) {
    auto cellSize {mScene.GetCellSize()};
    auto fieldLowerLeft {mScene.GetFieldLoweLeft()};
    auto cellXPos {column * cellSize + cellSize / 2.0f + fieldLowerLeft.x};
    auto cellYPos {row * cellSize + cellSize / 2.0f + fieldLowerLeft.y};
    auto& rotationMatrix {rotationMatrices[static_cast<int>(blueprintCell.mRotation)]};

    auto matrix {
        rotationMatrix *
        Pht::Mat4::Translate(cellXPos, cellYPos, mScene.GetBlueprintZ())
    };
    
    mEngineRenderer.Render(*blueprintCell.mRenderables.mSlot, matrix);
}

void GameRenderer::RenderFieldBlueprintSlotsAnimation() {
    auto* blueprintGrid {mField.GetBlueprintGrid()};
    
    if (blueprintGrid == nullptr) {
        return;
    }
    
    auto lowestVisibleRow {static_cast<int>(mScrollController.GetLowestVisibleRow())};
    auto pastHighestVisibleRow {lowestVisibleRow + mField.GetNumRowsInOneScreen()};
    
    for (auto row {lowestVisibleRow}; row < pastHighestVisibleRow; row++) {
        for (auto column {0}; column < mField.GetNumColumns(); column++) {
            auto& blueprintCell {(*blueprintGrid)[row][column]};
            
            if (blueprintCell.mAnimation.mIsActive) {
                RenderFieldBlueprintSlotAnimation(blueprintCell, row, column);
            }
        }
    }
}

void GameRenderer::RenderFieldBlueprintSlotAnimation(const BlueprintCell& blueprintCell,
                                                     int row,
                                                     int column) {
    auto cellSize {mScene.GetCellSize()};
    auto fieldLowerLeft {mScene.GetFieldLoweLeft()};
    auto cellXPos {column * cellSize + cellSize / 2.0f + fieldLowerLeft.x};
    auto cellYPos {row * cellSize + cellSize / 2.0f + fieldLowerLeft.y};
    
    auto& animation {blueprintCell.mAnimation};
    auto& rotationMatrix {rotationMatrices[static_cast<int>(blueprintCell.mRotation)]};
    
    auto& material {blueprintCell.mRenderables.mAnimation->GetMaterial()};
    auto opacity {material.GetOpacity()};
    material.SetOpacity(animation.mOpacity);
    
    auto matrix {
        rotationMatrix * Pht::Mat4::Scale(animation.mScale) *
        Pht::Mat4::Translate(cellXPos, cellYPos, mScene.GetBlueprintAnimationZ())
    };
    
    mEngineRenderer.Render(*blueprintCell.mRenderables.mAnimation, matrix);
    
    material.SetOpacity(opacity);
}

void GameRenderer::RenderFieldBlocks() {
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

void GameRenderer::RenderFieldBlock(const SubCell& subCell) {
    auto* renderableObject {subCell.mRenderableObject};
    
    if (renderableObject == nullptr) {
        return;
    }
    
    auto& rotationMatrix {rotationMatrices[static_cast<int>(subCell.mRotation)]};
    auto cellSize {mScene.GetCellSize()};
    auto fieldLowerLeft {mScene.GetFieldLoweLeft()};
    
    Pht::Vec3 blockPos {
        subCell.mPosition.x * cellSize + cellSize / 2.0f + fieldLowerLeft.x,
        subCell.mPosition.y * cellSize + cellSize / 2.0f + fieldLowerLeft.y,
        mScene.GetFieldPosition().z
    };
    
    auto blockMatrix {
        rotationMatrix * Pht::Mat4::Translate(blockPos.x, blockPos.y, blockPos.z)
    };
    
    if (subCell.mFlashingBlockAnimation.mIsActive) {
        auto& blockMaterial {renderableObject->GetMaterial()};
        auto blockAmbient {blockMaterial.GetAmbient()};
        auto blockDiffuse {blockMaterial.GetDiffuse()};
        auto blockSpecular {blockMaterial.GetSpecular()};

        auto& flashingBlockAdd {subCell.mFlashingBlockAnimation.mColorAdd};
        blockMaterial.SetAmbient(blockAmbient + flashingBlockAdd);
        blockMaterial.SetDiffuse(blockDiffuse + flashingBlockAdd);
        blockMaterial.SetSpecular(blockSpecular + flashingBlockAdd);
        
        Pht::Material* weldMaterial {nullptr};
        Pht::Color weldAmbient {0.0f};
        Pht::Color weldDiffuse {0.0f};
        Pht::Color weldSpecular {0.0f};
        
        if (subCell.mWeldRenderableObject) {
            weldMaterial = &subCell.mWeldRenderableObject->GetMaterial();
            weldAmbient = weldMaterial->GetAmbient();
            weldDiffuse = weldMaterial->GetDiffuse();
            weldSpecular = weldMaterial->GetSpecular();
            
            weldMaterial->SetAmbient(weldAmbient + flashingBlockAdd);
            weldMaterial->SetDiffuse(weldDiffuse + flashingBlockAdd);
            weldMaterial->SetSpecular(weldSpecular + flashingBlockAdd);
        }

        mEngineRenderer.Render(*renderableObject, blockMatrix);
        RenderBlockWelds(subCell, blockPos, cellSize);

        if (weldMaterial) {
            weldMaterial->SetAmbient(weldAmbient);
            weldMaterial->SetDiffuse(weldDiffuse);
            weldMaterial->SetSpecular(weldSpecular);
        }
        
        blockMaterial.SetAmbient(blockAmbient);
        blockMaterial.SetDiffuse(blockDiffuse);
        blockMaterial.SetSpecular(blockSpecular);
    } else {
        mEngineRenderer.Render(*renderableObject, blockMatrix);
        RenderBlockWelds(subCell, blockPos, cellSize);
    }
}

void GameRenderer::RenderFallingPiece() {
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    
    if (fallingPiece == nullptr) {
        return;
    }
    
    auto pieceGridPos {
        mGameLogic.IsUsingClickControls() ? fallingPiece->GetRenderablePositionSmoothY() :
                                            fallingPiece->GetRenderablePosition()
    };
    
    auto pieceWorldPos2 {mScene.GetFieldLoweLeft() + pieceGridPos * mScene.GetCellSize()};
    Pht::Vec3 pieceWorldPos3 {pieceWorldPos2.x, pieceWorldPos2.y, mScene.GetFieldPosition().z};
    auto& pieceGrid {fallingPiece->GetPieceType().GetGrid(fallingPiece->GetRotation())};
    
    RenderPieceBlocks(pieceGrid, pieceWorldPos3, 1.0f);
}

void GameRenderer::RenderPieceBlocks(const CellGrid& pieceBlocks,
                                     const Pht::Vec3& pieceWorldPos,
                                     float opacity) {
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    assert(fallingPiece);
    
    auto& pieceType {fallingPiece->GetPieceType()};
    auto pieceNumRows {pieceType.GetGridNumRows()};
    auto pieceNumColumns {pieceType.GetGridNumColumns()};
    auto cellSize {mScene.GetCellSize()};
    
    for (auto row {0}; row < pieceNumRows; row++) {
        for (auto column {0}; column < pieceNumColumns; column++) {
            auto& subCell {pieceBlocks[row][column].mFirstSubCell};
            auto* renderableObject {subCell.mRenderableObject};
            
            if (renderableObject) {
                auto& rotationMatrix {rotationMatrices[static_cast<int>(subCell.mRotation)]};
                auto blockXPos {column * cellSize + cellSize / 2.0f + pieceWorldPos.x};
                auto blockYPos {row * cellSize + cellSize / 2.0f + pieceWorldPos.y};
                
                auto blockMatrix {
                    rotationMatrix * Pht::Mat4::Translate(blockXPos, blockYPos, pieceWorldPos.z)
                };

                auto& blockMaterial {renderableObject->GetMaterial()};
                auto blockOpacity {blockMaterial.GetOpacity()};
                blockMaterial.SetOpacity(opacity);
                
                Pht::Material* weldMaterial {nullptr};
                float weldOpacity {0.0f};
                
                if (subCell.mWeldRenderableObject) {
                    weldMaterial = &subCell.mWeldRenderableObject->GetMaterial();
                    weldOpacity = weldMaterial->GetOpacity();
                    weldMaterial->SetOpacity(opacity);
                }
                
                RenderBlockWelds(subCell, Pht::Vec3 {blockXPos, blockYPos, pieceWorldPos.z}, cellSize);
                mEngineRenderer.Render(*renderableObject, blockMatrix);
                
                if (weldMaterial) {
                    weldMaterial->SetOpacity(weldOpacity);
                }
                
                blockMaterial.SetOpacity(blockOpacity);
            }
        }
    }
}

void GameRenderer::RenderBlockWelds(const SubCell& subCell,
                                    const Pht::Vec3& blockPos,
                                    float cellSize) {
    auto& welds {subCell.mWelds};
    auto weldZ {blockPos.z + cellSize / 2.0f};
    
    if (welds.mUpLeft) {
        auto weldMatrix {
            plus45RotationMatrix *
            Pht::Mat4::Translate(blockPos.x - cellSize / 2.0f, blockPos.y + cellSize / 2.0f, weldZ)
        };
        
        mEngineRenderer.Render(*subCell.mWeldRenderableObject, weldMatrix);
    }
    
    if (welds.mUp) {
        auto weldMatrix {
            rotationMatrices[static_cast<int>(Rotation::Deg90)] *
            Pht::Mat4::Translate(blockPos.x, blockPos.y + cellSize / 2.0f, weldZ)
        };
        
        mEngineRenderer.Render(*subCell.mWeldRenderableObject, weldMatrix);
    }
    
    if (welds.mUpRight) {
        auto weldMatrix {
            minus45RotationMatrix *
            Pht::Mat4::Translate(blockPos.x + cellSize / 2.0f, blockPos.y + cellSize / 2.0f, weldZ)
        };
        
        mEngineRenderer.Render(*subCell.mWeldRenderableObject, weldMatrix);
    }

    if (welds.mRight) {
        auto weldMatrix {Pht::Mat4::Translate(blockPos.x + cellSize / 2.0f, blockPos.y, weldZ)};
        mEngineRenderer.Render(*subCell.mWeldRenderableObject, weldMatrix);
    }
}

void GameRenderer::RenderGhostPieces() {
    auto* fallingPiece {mGameLogic.GetFallingPiece()};
    
    if (fallingPiece == nullptr) {
        return;
    }
    
    auto cellSize {mScene.GetCellSize()};
    auto& pieceType {fallingPiece->GetPieceType()};
    
    Pht::Vec3 ghostPieceCenterLocalCoords {
        cellSize * static_cast<float>(pieceType.GetGridNumColumns()) / 2.0f,
        cellSize * static_cast<float>(pieceType.GetGridNumRows()) / 2.0f,
        0.0f
    };
    
    if (mGameLogic.IsUsingClickControls()) {
        RenderClickableGhostPieces(*fallingPiece, ghostPieceCenterLocalCoords);
    } else {
        RenderGhostPiece(*fallingPiece, ghostPieceCenterLocalCoords);
    }
}

void GameRenderer::RenderGhostPiece(const FallingPiece& fallingPiece,
                                    const Pht::Vec3& ghostPieceCenterLocalCoords) {
    auto cellSize {mScene.GetCellSize()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};
    auto& pieceType {fallingPiece.GetPieceType()};
    auto* ghostPieceRenderable {GetGhostPieceRenderable(pieceType)};
    
    Pht::Vec3 ghostPieceWorldPos {
        fieldLowerLeft.x + fallingPiece.GetRenderablePosition().x * cellSize,
        fieldLowerLeft.y + mGameLogic.GetGhostPieceRow() * cellSize,
        mScene.GetGhostPieceZ()
    };
    
    if (ghostPieceRenderable) {
        RenderGhostPiece(*ghostPieceRenderable,
                         ghostPieceWorldPos + ghostPieceCenterLocalCoords,
                         fallingPiece.GetRotation());
    } else {
        auto& pieceGrid {fallingPiece.GetPieceType().GetGrid(fallingPiece.GetRotation())};
        RenderPieceBlocks(pieceGrid, ghostPieceWorldPos, mScene.GetGhostPieceOpacity());
    }
}

void GameRenderer::RenderClickableGhostPieces(const FallingPiece& fallingPiece,
                                              const Pht::Vec3& ghostPieceCenterLocalCoords) {
    auto* moveAlternatives {mGameLogic.GetClickInputHandler().GetMoveAlternativeSet()};
    
    if (moveAlternatives == nullptr) {
        return;
    }
    
    auto cellSize {mScene.GetCellSize()};
    auto ghostPieceZ {mScene.GetGhostPieceZ()};
    auto& fieldLowerLeft {mScene.GetFieldLoweLeft()};
    auto& pieceType {fallingPiece.GetPieceType()};
    auto* ghostPieceRenderable {GetGhostPieceRenderable(pieceType)};
    
    for (auto i {0}; i < moveAlternatives->Size(); ++i) {
        auto& move {moveAlternatives->At(i)};
        auto& pieceGrid {pieceType.GetGrid(move.mRotation)};
        
        Pht::Vec3 ghostPieceWorldPos {
            fieldLowerLeft.x + static_cast<float>(move.mPosition.x) * cellSize,
            fieldLowerLeft.y + static_cast<float>(move.mPosition.y) * cellSize,
            ghostPieceZ + static_cast<float>(i) * dz
        };
        
        auto isMoveButtonDown {move.mButton->GetButton().IsDown()};
        
        if (ghostPieceRenderable) {
            Pht::Vec3 postion {ghostPieceWorldPos + ghostPieceCenterLocalCoords};
            auto rotation {move.mRotation};
            
            if (isMoveButtonDown) {
                RenderGhostPiece(*pieceType.GetPressedGhostPieceRenderable(), postion, rotation);
            } else {
                RenderGhostPiece(*ghostPieceRenderable, postion, rotation);
            }
        } else {
            if (isMoveButtonDown) {
                RenderPieceBlocks(pieceGrid, ghostPieceWorldPos, 1.0f);
            } else {
                RenderPieceBlocks(pieceGrid, ghostPieceWorldPos, mScene.GetGhostPieceOpacity());
            }
        }
    }
}

const Pht::RenderableObject* GameRenderer::GetGhostPieceRenderable(const Piece& pieceType) const {
    if (auto* ghostPieceRenderable {pieceType.GetGhostPieceRenderable()}) {
        if (mScene.AreGhostPiecesFilled()) {
            return pieceType.GetFilledGhostPieceRenderable();
        }
        
        return ghostPieceRenderable;
    }
    
    return nullptr;
}

void GameRenderer::RenderGhostPiece(const Pht::RenderableObject& ghostPieceRenderable,
                                    const Pht::Vec3& position,
                                    Rotation rotation) {
    auto& rotationMatrix {rotationMatrices[static_cast<int>(rotation)]};
    auto matrix {rotationMatrix * Pht::Mat4::Translate(position.x, position.y, position.z)};
    mEngineRenderer.Render(ghostPieceRenderable, matrix);
}

void GameRenderer::RenderExplosion() {
    RenderUtils::RenderParticleEffect(mEngineRenderer, mExplosionParticleEffect.GetInnerEffect());
    RenderUtils::RenderParticleEffect(mEngineRenderer, mExplosionParticleEffect.GetOuterEffect());
}

void GameRenderer::RenderRowExplosion() {
    RenderUtils::RenderParticleEffect(mEngineRenderer, mRowExplosionParticleEffect.GetEffect());
}

void GameRenderer::RenderFlyingBlocks() {
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

void GameRenderer::RenderHud() {
    mEngineRenderer.SetHudMode(true);
    mEngineRenderer.SetLightPosition(mHud.GetLightDirection());

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
    
    mEngineRenderer.SetLightPosition(mScene.GetLightDirection());
    mEngineRenderer.SetHudMode(false);
}

void GameRenderer::RenderTiltedGrayBlockInHud(const Pht::RenderableObject& grayBlock) {
    auto position {mHud.GetProgressPosition() + mHud.GetGrayBlockRelativePosition()};
    
    auto baseTransform {
        previewRotationMatrix *
        Pht::Mat4::Translate(position.x, position.y, -1.0f)
    };
    
    auto scale {Pht::Mat4::Scale(mHud.GetGrayBlockSize() / mScene.GetCellSize())};
    auto blockMatrix {scale * baseTransform};

    mEngineRenderer.Render(grayBlock, blockMatrix);
}

void GameRenderer::RenderTiltedBlueprintSlotInHud() {
    auto position {mHud.GetProgressPosition() + mHud.GetBlueprintSlotRelativePosition()};
    
    auto baseTransform {
        previewRotationMatrix *
        Pht::Mat4::Translate(position.x, position.y, -1.0f)
    };
    
    auto scale {Pht::Mat4::Scale(mHud.GetBlueprintSlotSize() / mScene.GetCellSize())};
    auto blockMatrix {scale * baseTransform};

    mEngineRenderer.Render(*mHud.GetBlueprintSlot(), blockMatrix);
}

void GameRenderer::RenderTiltedLPieceInHud() {
    auto& lPiece {mHud.GetLPiece()};

    RenderScaledTiltedPiece(mHud.GetMovesPosition() + mHud.GetLPieceRelativePosition(),
                            mHud.GetLPieceCellSize(),
                            lPiece.GetGridNumRows(),
                            lPiece.GetGridNumColumns(),
                            lPiece.GetGrid(Rotation::Deg0));
}

void GameRenderer::RenderPreviewPiece(const Piece* piece, const Pht::Vec2& position) {
    if (piece == nullptr) {
        return;
    }
    
    RenderScaledTiltedPiece(position,
                            piece->GetPreviewCellSize(),
                            piece->GetGridNumRows(),
                            piece->GetGridNumColumns(),
                            piece->GetGrid(Rotation::Deg0));
}

void GameRenderer::RenderScaledTiltedPiece(const Pht::Vec2& position,
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

void GameRenderer::RenderPieceDropParticles() {
    for (const auto& effect: mPieceDropParticleEffect.GetEffects()) {
        RenderUtils::RenderParticleEffect(mEngineRenderer, effect);
    }
}

void GameRenderer::RenderGameViews() {
    switch (mGameViewControllers.GetActiveController()) {
        case GameViewControllers::None:
            break;
        case GameViewControllers::GameHud:
            mEngineRenderer.RenderGuiView(mGameViewControllers.GetGameHudController().GetView());
            break;
        case GameViewControllers::GameMenu: {
            auto& gameMenuController {mGameViewControllers.GetGameMenuController()};
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
            settingsMenuController.GetFadeEffect().Render();
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
            restartConfirmationDialogController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(restartConfirmationDialogController.GetView());
            break;
        }
        case GameViewControllers::MapConfirmationDialog:
            auto& mapConfirmationDialogController {
                mGameViewControllers.GetMapConfirmationDialogController()
            };
            mapConfirmationDialogController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(mapConfirmationDialogController.GetView());
            break;
    }
}

void GameRenderer::RenderLevelCompletedView() {
    auto& levelCompletedDialogController {mGameViewControllers.GetLevelCompletedDialogController()};
    levelCompletedDialogController.GetFadeEffect().Render();
    
    auto& view {levelCompletedDialogController.GetView()};
    mEngineRenderer.SetLightPosition({0.75f, 1.0f, 1.0f});
    mEngineRenderer.RenderGuiView(view);
    mEngineRenderer.SetLightPosition(mScene.GetLightDirection());
}

void GameRenderer::RenderSlidingText() {
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
