#include "PiecePathSystem.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GameScene.hpp"
#include "MoveDefinitions.hpp"
#include "Level.hpp"
#include "Piece.hpp"

using namespace RowBlast;

namespace {
    constexpr auto numFillRenderables = 2;
    constexpr auto colorAlpha = 0.1f;
    constexpr auto fadeInWaveSpeed = 40.0f;
    constexpr auto cellFadeInDuration = 0.2f;
    constexpr auto numAlreadyLitRows = 2.0f;
    constexpr auto sineWaveWaitDuration = 1.0f;
    constexpr auto sineWaveSpeed = 24.0f;
    constexpr auto sineWaveAmplitude = 0.1f;
    constexpr auto sineWaveLength = 16.0f;

    const Pht::Vec4 redColor {1.0f, 0.5f, 0.5f, 1.0f};
    const Pht::Vec4 greenColor {0.5f, 0.8f, 0.5f, 1.0f};
    const Pht::Vec4 blueColor {0.3f, 0.72f, 1.0f, 1.0f};
    const Pht::Vec4 yellowColor {0.875f, 0.75f, 0.0f, 1.0f};
    
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
    
    auto numRenderables = numFillRenderables * Quantities::numBlockColors * Field::maxNumRows;
    mRenderables.resize(numRenderables);
    
    Pht::Material material;
    material.SetBlend(Pht::Blend::Yes);
        
    for (auto colorIndex = 0; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        auto blockColor = static_cast<BlockColor>(colorIndex);
        
        for (auto row = 0; row < Field::maxNumRows; ++row) {
            mRenderables[CalcRenderableIndex(Fill::Full, blockColor, row)] =
                CreateRenderable(Fill::Full, blockColor, engine, material);

            mRenderables[CalcRenderableIndex(Fill::LowerRightHalf, blockColor, row)] =
                CreateRenderable(Fill::LowerRightHalf, blockColor, engine, material);
        }
    }
}
                     
std::unique_ptr<Pht::RenderableObject> PiecePathSystem::CreateRenderable(Fill fill,
                                                                         BlockColor blockColor,
                                                                         Pht::IEngine& engine,
                                                                         const Pht::Material& material) {
    auto name = ToString(fill) + ToString(blockColor);
    auto vertices = CreateVertices(fill, blockColor);
    auto& sceneManager = engine.GetSceneManager();
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {vertices, name}, material);
}

Pht::QuadMesh::Vertices PiecePathSystem::CreateVertices(Fill fill, BlockColor blockColor) {
    auto halfSize = mScene.GetCellSize() / 2.0f;
    auto color = ToColor(blockColor);

    switch (fill) {
        case Fill::Full:
            return Pht::QuadMesh::Vertices {
                {{-halfSize, -halfSize, 0.0f}, color},
                {{halfSize, -halfSize, 0.0f}, color},
                {{halfSize, halfSize, 0.0f}, color},
                {{-halfSize, halfSize, 0.0f}, color}
            };
        default: {
            auto nudge = 0.0001f;
            return Pht::QuadMesh::Vertices {
                {{-halfSize, -halfSize, 0.0f}, color},
                {{halfSize, -halfSize, 0.0f}, color},
                {{halfSize, halfSize, 0.0f}, color},
                {{-halfSize + nudge, -halfSize + nudge, 0.0f}, color}
            };
        }
    }
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
    mState = State::Inactive;
    HidePath();
}

void PiecePathSystem::ShowPath(const FallingPiece& fallingPiece,
                               const Movement& lastMovement,
                               int lowestVisibleRow) {
    mLowestVisibleRow = lowestVisibleRow;
    SetColor(fallingPiece);
    
    switch (mState) {
        case State::FadingIn:
            break;
        case State::SineWaveWait:
        case State::SineWave:
            GoToSineWaveWaitState();
            break;
        case State::Inactive:
            GoToFadingInState(fallingPiece, lastMovement);
            break;
    }
    
    ClearGrid();
    mMovements.Clear();
    
    for (auto* movement = &lastMovement; movement; movement = movement->GetPrevious()) {
        mMovements.PushBack(movement);
    }
    
    mMovements.Reverse();
    RemoveFirstMovementIfDetour(fallingPiece);
    FillWholePath(fallingPiece);

    Pht::IVec2 finalPosition {
        static_cast<int>(lastMovement.GetPosition().x),
        static_cast<int>(lastMovement.GetPosition().y)
    };

    auto& pieceType = fallingPiece.GetPieceType();
    if (!pieceType.IsRowBomb() && !pieceType.IsBomb()) {
        MovingPieceSnapshot finalSnapshot {finalPosition, lastMovement.GetRotation(), pieceType};
        PaintPieceSnapshot(finalSnapshot, SnapshotKind::Clear);
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

void PiecePathSystem::FillWholePath(const FallingPiece& fallingPiece) {
    MovingPieceSnapshot movingPiece {
        fallingPiece.GetIntPosition(),
        fallingPiece.GetRotation(),
        fallingPiece.GetPieceType()
    };

    auto snapshotKind = SnapshotKind::Standard;

    for (auto movementIndex = 0; movementIndex < mMovements.Size(); ++movementIndex) {
        const auto& movement = *mMovements.At(movementIndex);
        auto segmentStartposition = movingPiece.mPosition;
        auto movementPosition = movement.GetPosition();
        
        Pht::IVec2 targetPosition {
            static_cast<int>(movementPosition.x),
            static_cast<int>(movementPosition.y)
        };
        
        if (segmentStartposition.y > targetPosition.y) {
            for (auto y = segmentStartposition.y; y >= targetPosition.y; --y) {
                movingPiece.mPosition.y = y;
                snapshotKind =
                    (y == segmentStartposition.y && snapshotKind == SnapshotKind::MoveSideways ?
                     SnapshotKind::MoveSideways : SnapshotKind::Standard);
                PaintPieceSnapshot(movingPiece, snapshotKind);
            }
            if (movementIndex < mMovements.Size() - 1) {
                snapshotKind = SnapshotKind::MoveSideways;
                PaintPieceSnapshot(movingPiece, snapshotKind);
            }
        } else if (segmentStartposition.x > targetPosition.x) {
            for (auto x = segmentStartposition.x; x >= targetPosition.x; --x) {
                movingPiece.mPosition.x = x;
                snapshotKind = SnapshotKind::MoveSideways;
                PaintPieceSnapshot(movingPiece, snapshotKind);
            }
        } else if (segmentStartposition.x < targetPosition.x) {
            for (auto x = segmentStartposition.x; x <= targetPosition.x; ++x) {
                movingPiece.mPosition.x = x;
                snapshotKind = (movingPiece.mPosition == fallingPiece.GetIntPosition() ?
                                SnapshotKind::Standard : SnapshotKind::MoveSideways);
                PaintPieceSnapshot(movingPiece, snapshotKind);
            }
        }
        
        movingPiece.mPosition = targetPosition;
        movingPiece.mRotation = movement.GetRotation();
        if (movementIndex == mMovements.Size() - 1) {
            snapshotKind = SnapshotKind::Last;
        }
        PaintPieceSnapshot(movingPiece, snapshotKind);
    }
}

void PiecePathSystem::PaintPieceSnapshot(const MovingPieceSnapshot& movingPiece,
                                         SnapshotKind snapshotKind) {
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

            auto pieceSubCellFill = pieceSubCell.mFill;
            auto row = piecePosition.y + pieceRow;
            auto column = piecePosition.x + pieceColumn;
            
            if (row < 0) {
                continue;
            }
            
            switch (snapshotKind) {
                case SnapshotKind::Standard:
                    SetSnapshotCell(row, column, pieceSubCellFill);
                    break;
                case SnapshotKind::MoveSideways:
                    SetSnapshotCellMovingSideways(row, column, pieceSubCellFill);
                    break;
                case SnapshotKind::Last:
                    SetLastSnapshotCell(row, column, pieceSubCellFill);
                    break;
                case SnapshotKind::Clear:
                    ClearSnapshotCell(row, column, pieceSubCellFill);
                    break;
            }
        }
    }
}

void PiecePathSystem::ClearSnapshotCell(int row, int column, Fill pieceSubCellFill) {
    if (mPathGrid[row][column] == Fill::Full) {
        switch (pieceSubCellFill) {
            case Fill::LowerLeftHalf:
                mPathGrid[row][column] = Fill::UpperRightHalf;
                break;
            case Fill::LowerRightHalf:
                mPathGrid[row][column] = Fill::UpperLeftHalf;
                break;
            case Fill::UpperLeftHalf:
                mPathGrid[row][column] = Fill::LowerRightHalf;
                break;
            case Fill::UpperRightHalf:
                mPathGrid[row][column] = Fill::LowerLeftHalf;
                break;
            case Fill::Full:
                mPathGrid[row][column] = Fill::Empty;
                break;
            case Fill::Empty:
                break;
        }
    } else if (mPathGrid[row][column] == pieceSubCellFill) {
        mPathGrid[row][column] = Fill::Empty;
    }
}

void PiecePathSystem::SetLastSnapshotCell(int row, int column, Fill pieceSubCellFill) {
    if (row + 1 < mNumRows) {
        switch (pieceSubCellFill) {
            case Fill::UpperLeftHalf:
            case Fill::UpperRightHalf:
                if (mPathGrid[row + 1][column] != Fill::Empty) {
                    mPathGrid[row + 1][column] = Fill::Full;
                }
                mPathGrid[row][column] = pieceSubCellFill;
                break;
            case Fill::LowerLeftHalf:
            case Fill::LowerRightHalf:
                if (mPathGrid[row + 1][column] != Fill::Empty) {
                    mPathGrid[row][column] = Fill::Full;
                } else {
                    mPathGrid[row][column] = pieceSubCellFill;
                }
                break;
            default:
                mPathGrid[row][column] = pieceSubCellFill;
                break;
        }
    } else {
        mPathGrid[row][column] = pieceSubCellFill;
    }
}

void PiecePathSystem::SetSnapshotCell(int row, int column, Fill pieceSubCellFill) {
    if (row + 2 < mNumRows) {
        switch (mPathGrid[row + 1][column]) {
            case Fill::UpperLeftHalf:
            case Fill::UpperRightHalf:
                mPathGrid[row + 1][column] = Fill::Full;
                break;
            case Fill::LowerLeftHalf:
            case Fill::LowerRightHalf:
                if (mPathGrid[row + 2][column] != Fill::Empty) {
                    mPathGrid[row + 1][column] = Fill::Full;
                }
                break;
            default:
                break;
        }
    }

    mPathGrid[row][column] = pieceSubCellFill;
}

void PiecePathSystem::SetSnapshotCellMovingSideways(int row, int column, Fill pieceSubCellFill) {
    if (pieceSubCellFill != Fill::Empty) {
        mPathGrid[row][column] = Fill::Full;
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
                
                auto& transform = sceneObject.GetTransform();
                transform.SetPosition(position);
                
                auto& renderable = GetRenderableObject(fill, mColor, row - mLowestVisibleRow);
                sceneObject.SetRenderable(&renderable);
                
                switch (fill) {
                    case Fill::UpperRightHalf:
                        transform.SetRotation({0.0f, 0.0f, -270.0f});
                        break;
                    case Fill::UpperLeftHalf:
                        transform.SetRotation({0.0f, 0.0f, -180.0f});
                        break;
                    case Fill::LowerLeftHalf:
                        transform.SetRotation({0.0f, 0.0f, -90.0f});
                        break;
                    case Fill::LowerRightHalf:
                    case Fill::Full:
                        transform.SetRotation({0.0f, 0.0f, 0.0f});
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void PiecePathSystem::HidePath() {
    mState = State::Inactive;
    mSceneObjectPool->SetIsActive(false);
}

void PiecePathSystem::Update(float dt) {
    switch (mState) {
        case State::FadingIn:
            UpdateInFadingInState(dt);
            break;
        case State::SineWaveWait:
            UpdateInSineWaveWaitState(dt);
            break;
        case State::SineWave:
            UpdateInSineWaveState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void PiecePathSystem::UpdateInFadingInState(float dt) {
    auto fadeStateDuration =
        static_cast<float>(Field::maxNumRows) / fadeInWaveSpeed + cellFadeInDuration;

    mElapsedTime += dt;
    if (mElapsedTime > fadeStateDuration) {
        GoToSineWaveWaitState();
        return;
    }

    auto wavefront = mElapsedTime * fadeInWaveSpeed;
    
    for (auto visibleRow = 0; visibleRow < Field::maxNumRows; ++visibleRow) {
        auto wavefrontRow = static_cast<int>(wavefront);
        if (visibleRow <= wavefrontRow) {
            auto distanceToWavefront = wavefront - static_cast<float>(visibleRow);
            auto elapsedTime = distanceToWavefront / fadeInWaveSpeed;
            auto opacity = elapsedTime < cellFadeInDuration ?
                           colorAlpha * elapsedTime / cellFadeInDuration : colorAlpha;
            SetOpacity(opacity, visibleRow);
        }
    }
}

void PiecePathSystem::UpdateInSineWaveWaitState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > sineWaveWaitDuration) {
        GoToSineWaveState();
    }
}

void PiecePathSystem::UpdateInSineWaveState(float dt) {
    mElapsedTime += dt;
    
    auto waveYPos = static_cast<float>(Field::maxNumRows) + sineWaveLength / 2.0f -
                    mElapsedTime * sineWaveSpeed;
    if (waveYPos + sineWaveLength / 2.0f < 0.0f) {
        mElapsedTime = 0.0f;
        return;
    }

    for (auto visibleRow = 0; visibleRow < Field::maxNumRows; ++visibleRow) {
        if (visibleRow + 1.0f > waveYPos - sineWaveLength / 2.0f &&
            visibleRow + 1.0f < waveYPos + sineWaveLength / 2.0f) {
            
            auto x = 2.0f * 3.1415f * (static_cast<float>(visibleRow) - waveYPos) / sineWaveLength;
            auto waveOpacity = 0.5f * (sineWaveAmplitude * std::cos(x) + sineWaveAmplitude);
            SetOpacity(waveOpacity + colorAlpha, visibleRow);
        } else {
            SetOpacity(colorAlpha, visibleRow);
        }
    }
}

void PiecePathSystem::SetOpacity(float opacity, int visibleRow) {
    GetRenderableObject(Fill::Full, mColor, visibleRow).GetMaterial().SetOpacity(opacity);
    GetRenderableObject(Fill::LowerRightHalf, mColor, visibleRow).GetMaterial().SetOpacity(opacity);
}

void PiecePathSystem::GoToFadingInState(const FallingPiece& fallingPiece,
                                        const Movement& lastMovement) {
    mState = State::FadingIn;
    auto& pieceType = fallingPiece.GetPieceType();
    auto draggedPieceLocalYMax = pieceType.GetDimensions(lastMovement.GetRotation()).mYmax;
    auto draggedPieceY = lastMovement.GetPosition().y;
    mElapsedTime =
        (draggedPieceY + draggedPieceLocalYMax - static_cast<float>(mLowestVisibleRow) +
         numAlreadyLitRows) / fadeInWaveSpeed;

    for (auto visibleRow = 0; visibleRow < Field::maxNumRows; ++visibleRow) {
        SetOpacity(0.0f, visibleRow);
    }
}

void PiecePathSystem::GoToSineWaveWaitState() {
    mState = State::SineWaveWait;
    mElapsedTime = 0.0f;
    
    for (auto visibleRow = 0; visibleRow < Field::maxNumRows; ++visibleRow) {
        SetOpacity(colorAlpha, visibleRow);
    }
}

void PiecePathSystem::GoToSineWaveState() {
    mState = State::SineWave;
    mElapsedTime = 0.0f;
    
    for (auto visibleRow = 0; visibleRow < Field::maxNumRows; ++visibleRow) {
        SetOpacity(colorAlpha, visibleRow);
    }
}

bool PiecePathSystem::IsPathVisible() {
    switch (mState) {
        case State::FadingIn:
        case State::SineWaveWait:
        case State::SineWave:
            return true;
        case State::Inactive:
            return false;
    }
}

void PiecePathSystem::ClearGrid() {
    for (auto row = 0; row < mNumRows; ++row) {
        for (auto column = 0; column < mNumColumns; ++column) {
            mPathGrid[row][column] = Fill::Empty;
        }
    }
}

Pht::RenderableObject& PiecePathSystem::GetRenderableObject(Fill fill,
                                                            BlockColor color,
                                                            int visibleRow) {
    return *(mRenderables[CalcRenderableIndex(fill, color, visibleRow)]);
}

int PiecePathSystem::CalcRenderableIndex(Fill fill, BlockColor color, int visibleRow) const {
    auto fillIndex = (fill == Fill::Full ? 0 : 1);
    auto colorIndex = static_cast<int>(color);
    
    if (visibleRow > Field::maxNumRows) {
        visibleRow = Field::maxNumRows - 1;
    }
    
    if (visibleRow < 0) {
        visibleRow = 0;
    }
    
    assert(fillIndex >= 0 && fillIndex < numFillRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           visibleRow >= 0 && visibleRow < Field::maxNumRows);
    
    auto index = visibleRow * numFillRenderables * Quantities::numBlockColors +
                 colorIndex * numFillRenderables + fillIndex;

    assert(index < mRenderables.size());
    return index;
}
