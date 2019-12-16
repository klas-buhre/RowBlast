#include "PiecePathSystem.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "MoveDefinitions.hpp"
#include "Level.hpp"
#include "Piece.hpp"

using namespace RowBlast;

namespace {
    constexpr auto colorAlpha = 0.1f;

    const Pht::Vec4 redColor {1.0f, 0.5f, 0.5f, colorAlpha};
    const Pht::Vec4 greenColor {0.5f, 0.79f, 0.5f, colorAlpha};
    const Pht::Vec4 blueColor {0.3f, 0.72f, 1.0f, colorAlpha};
    const Pht::Vec4 yellowColor {0.875f, 0.75f, 0.0f, colorAlpha};
    
    Pht::Vec4 ToColor(BlockColor blockColor) {
        switch (blockColor) {
            case BlockColor::Red:
                return redColor;
            case BlockColor::Green:
                return greenColor;
            case BlockColor::Blue:
                return blueColor;
            case BlockColor::Yellow:
                return yellowColor;
            default:
                assert(false);
                break;
        }
    }
}

PiecePathSystem::PiecePathSystem(Pht::IEngine& engine, GameScene& scene) :
    mScene {scene} {
    
    auto numRenderables = Quantities::numBlockRenderables * Quantities::numBlockColors;
    mRenderables.resize(numRenderables);
    
    Pht::Material material;
    material.SetBlend(Pht::Blend::Yes);
        
    for (auto colorIndex = 0; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        auto blockColor = static_cast<BlockColor>(colorIndex);
        for (auto fillIndex = 0; fillIndex < Quantities::numBlockRenderables; ++fillIndex) {
            auto fill = static_cast<Fill>(fillIndex);
            auto renderableIndex = CalcRenderableIndex(fill, blockColor);
            mRenderables[renderableIndex] = CreateRenderable(fill, blockColor, engine, material);
        }
    }
}
                     
std::unique_ptr<Pht::RenderableObject> PiecePathSystem::CreateRenderable(Fill fill,
                                                                         BlockColor blockColor,
                                                                         Pht::IEngine& engine,
                                                                         const Pht::Material& material) {
    auto halfSize = mScene.GetCellSize() / 2.0f;
    auto color = ToColor(blockColor);

    Pht::QuadMesh::Vertices vertices {
        {{-halfSize, -halfSize, 0.0f}, color},
        {{halfSize, -halfSize, 0.0f}, color},
        {{halfSize, halfSize, 0.0f}, color},
        {{-halfSize, halfSize, 0.0f}, color}
    };

    auto& sceneManager = engine.GetSceneManager();
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {vertices}, material);
}

void PiecePathSystem::Init(const Level& level) {
    mNumRows = level.GetNumRows();
    mNumColumns = level.GetNumColumns();
    mPathGrid.clear();
    mPathGrid.reserve(mNumRows);
    std::vector<Fill> emptyRow(mNumColumns);
    for (auto rowIndex = 0; rowIndex < mNumRows; ++rowIndex) {
        mPathGrid.push_back(emptyRow);
    }
    
    ClearGrid();
    
    mSceneObjectPool = std::make_unique<SceneObjectPool>(SceneObjectPoolKind::PiecePath,
                                                         mScene.GetPieceDropEffectsContainer(),
                                                         level.GetNumColumns());
    mSceneObjectPool->SetIsActive(false);
}

void PiecePathSystem::ShowPath(const FallingPiece& fallingPiece, const Movement& lastMovement) {
    SetColor(fallingPiece);
    ClearGrid();
    mMovements.Clear();
    
    for (auto* movement = &lastMovement; movement; movement = movement->GetPrevious()) {
        mMovements.PushBack(movement);
    }
    
    mMovements.Reverse();
    RemoveFirstMovementIfDetour(fallingPiece);
    
    auto& pieceType = fallingPiece.GetPieceType();
    
    MovingPieceSnapshot movingPiece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        pieceType
    };
    FillWholePath(movingPiece);
    
    if (!pieceType.IsRowBomb()) {
        Pht::IVec2 finalPosition {
            static_cast<int>(lastMovement.GetPosition().x),
            static_cast<int>(lastMovement.GetPosition().y)
        };
        MovingPieceSnapshot finalSnapshot {finalPosition, lastMovement.GetRotation(), pieceType};
        PaintPieceSnapshot(finalSnapshot, true);
    }

    UpdateSceneObjects();
}

void PiecePathSystem::SetColor(const FallingPiece& fallingPiece) {
    auto& pieceType = fallingPiece.GetPieceType();
    if (pieceType.IsBomb() || pieceType.IsRowBomb()) {
        mColor = BlockColor::Red;
    } else {
        mColor = pieceType.GetColor();
    }
}

void PiecePathSystem::RemoveFirstMovementIfDetour(const FallingPiece& fallingPiece) {
    if (mMovements.Size() > 1) {
        auto fallingPiecePos = fallingPiece.GetRenderablePosition();
        auto* secondMovement = mMovements.At(1);
        if (fallingPiecePos.y == secondMovement->GetPosition().y) {
            auto* firstMovement = mMovements.Front();
            auto firstMovementDiff = fallingPiecePos - firstMovement->GetPosition();
            auto secondMovementDiff = fallingPiecePos - secondMovement->GetPosition();
            if (std::abs(secondMovementDiff.x) <= std::abs(firstMovementDiff.x)) {
                mMovements.Erase(0);
            }
        }
    }
}

void PiecePathSystem::FillWholePath(MovingPieceSnapshot movingPiece) {
    for (auto movementIndex = 0; movementIndex < mMovements.Size(); ++movementIndex) {
        const auto& movement = *mMovements.At(movementIndex);
        const auto& position = movingPiece.mPosition;
        auto& movementPosition = movement.GetPosition();
        
        Pht::IVec2 targetPosition {
            static_cast<int>(movementPosition.x),
            static_cast<int>(movementPosition.y)
        };
        
        if (position.y > targetPosition.y) {
            for (auto y = position.y; y >= targetPosition.y; --y) {
                movingPiece.mPosition.y = y;
                PaintPieceSnapshot(movingPiece);
            }
        } else if (position.x > targetPosition.x) {
            for (auto x = position.x; x >= targetPosition.x; --x) {
                movingPiece.mPosition.x = x;
                PaintPieceSnapshot(movingPiece);
            }
        } else if (position.x < targetPosition.x) {
            for (auto x = position.x; x <= targetPosition.x; ++x) {
                movingPiece.mPosition.x = x;
                PaintPieceSnapshot(movingPiece);
            }
        }
        
        movingPiece.mPosition = targetPosition;
        movingPiece.mRotation = movement.GetRotation();
        PaintPieceSnapshot(movingPiece);
    }
}

void PiecePathSystem::PaintPieceSnapshot(const MovingPieceSnapshot& movingPiece,
                                         bool clearSnapshot) {
    auto& pieceType = movingPiece.mPieceType;
    auto& pieceGrid = pieceType.GetGrid(movingPiece.mRotation);
    auto numRows = pieceType.GetGridNumRows();
    auto numColumns = pieceType.GetGridNumColumns();
    auto piecePosition = movingPiece.mPosition;
    
    for (auto pieceRow = 0; pieceRow < numRows; ++pieceRow) {
        for (auto pieceColumn = 0; pieceColumn < numColumns; ++pieceColumn) {
            auto& pieceSubCell = pieceGrid[pieceRow][pieceColumn].mFirstSubCell;
            if (pieceSubCell.IsEmpty()) {
                continue;
            }

            auto row = piecePosition.y + pieceRow;
            auto column = piecePosition.x + pieceColumn;
            if (clearSnapshot) {
                mPathGrid[row][column] = Fill::Empty;
            } else {
                mPathGrid[row][column] = pieceSubCell.mFill;
            }
        }
    }
}

void PiecePathSystem::UpdateSceneObjects() {
    mSceneObjectPool->SetIsActive(true);
    mSceneObjectPool->ReclaimAll();
    
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            auto fill = mPathGrid[row][column];
            if (fill != Fill::Empty) {
                auto& sceneObject = mSceneObjectPool->AccuireSceneObject();
                auto cellSize = mScene.GetCellSize();
                
                Pht::Vec3 position {
                    column * cellSize + cellSize / 2.0f,
                    row * cellSize + cellSize / 2.0f,
                    mScene.GetPiecePathZ()
                };
                
                sceneObject.GetTransform().SetPosition(position);
                sceneObject.SetRenderable(&GetRenderableObject(fill, mColor));
            }
        }
    }
}

void PiecePathSystem::HidePath() {
    mSceneObjectPool->SetIsActive(false);
}

void PiecePathSystem::ClearGrid() {
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            mPathGrid[row][column] = Fill::Empty;
        }
    }
}

Pht::RenderableObject& PiecePathSystem::GetRenderableObject(Fill fill, BlockColor color) const {
    return *(mRenderables[CalcRenderableIndex(fill, color)]);
}

int PiecePathSystem::CalcRenderableIndex(Fill fill, BlockColor color) const {
    auto fillIndex = static_cast<int>(fill);
    auto colorIndex = static_cast<int>(color);
    
    assert(fillIndex >= 0 && fillIndex < Quantities::numBlockRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors);
    
    auto index = colorIndex * Quantities::numBlockRenderables + fillIndex;
    
    assert(index < mRenderables.size());
    return index;
}
