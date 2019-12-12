#include "Piece.hpp"

#include <algorithm>
#include <assert.h>

using namespace RowBlast;

namespace {
    Fill RotateCellFillClockwise90Deg(Fill fill) {
        switch (fill) {
            case Fill::Empty:
            case Fill::Full:
                return fill;
            case Fill::LowerRightHalf:
                return Fill::LowerLeftHalf;
            case Fill::LowerLeftHalf:
                return Fill::UpperLeftHalf;
            case Fill::UpperLeftHalf:
                return Fill::UpperRightHalf;
            case Fill::UpperRightHalf:
                return Fill::LowerRightHalf;
        }
    }
    
    Bonds RotateBondsClockwise90Deg(const Bonds& bonds) {
        Bonds rotatedBonds;
        
        if (bonds.mUp) {
            rotatedBonds.mRight = true;
        }

        if (bonds.mUpRight) {
            rotatedBonds.mDownRight = true;
        }
        
        if (bonds.mRight) {
            rotatedBonds.mDown = true;
        }

        if (bonds.mDownRight) {
            rotatedBonds.mDownLeft = true;
        }

        if (bonds.mDown) {
            rotatedBonds.mLeft = true;
        }

        if (bonds.mDownLeft) {
            rotatedBonds.mUpLeft = true;
        }

        if (bonds.mLeft) {
            rotatedBonds.mUp = true;
        }

        if (bonds.mUpLeft) {
            rotatedBonds.mUpRight = true;
        }

        return rotatedBonds;
    }
}

Piece::Piece() {
    mDuplicateMoveChecks.resize(4);
}

const CellGrid& Piece::GetGrid(Rotation rotation) const {
    return mGrids[static_cast<int>(rotation)];
}

const ClickGrid& Piece::GetClickGrid(Rotation rotation) const {
    return mClickGrids[static_cast<int>(rotation)];
}

const Pht::IVec2& Piece::GetRightOverhangCheckPosition(Rotation rotation) const {
    return mRightOverhangCheckPositions[static_cast<int>(rotation)];
}

const Pht::IVec2& Piece::GetLeftOverhangCheckPosition(Rotation rotation) const {
    return mLeftOverhangCheckPositions[static_cast<int>(rotation)];
}

const Pht::Optional<Pht::IVec2>& Piece::GetRightExtremityCheckPosition(Rotation rotation) const {
    return mRightExtremityCheckPositions[static_cast<int>(rotation)];
}

const Pht::Optional<Pht::IVec2>& Piece::GetLeftExtremityCheckPosition(Rotation rotation) const {
    return mLeftExtremityCheckPositions[static_cast<int>(rotation)];
}

const Piece::Dimensions& Piece::GetDimensions(Rotation rotation) const {
    return mDimensions[static_cast<int>(rotation)];
}

const Pht::Optional<Piece::DuplicateMoveCheck>& Piece::GetDuplicateMoveCheck(Rotation rotation) const {
    return mDuplicateMoveChecks[static_cast<int>(rotation)];
}

const Pht::Optional<Piece::TiltedBondCheck>& Piece::GetTiltedBondCheck(Rotation rotation) const {
    return mTiltedBondChecks[static_cast<int>(rotation)];
}

Pht::Vec2 Piece::GetCenterPosition(Rotation rotation) const {
    return GetButtonCenterPosition(rotation) / 2.0f;
}

const Pht::Vec2& Piece::GetButtonCenterPosition(Rotation rotation) const {
    return mButtonCenterPositions[static_cast<int>(rotation)];
}

const Pht::Vec2& Piece::GetButtonSize(Rotation rotation) const {
    return mButtonSizes[static_cast<int>(rotation)];
}

bool Piece::IsBomb() const {
    return false;
}

bool Piece::IsRowBomb() const {
    return false;
}

Rotation Piece::GetSpawnRotation() const {
    return Rotation::Deg0;
}

int Piece::GetSpawnYPositionAdjustment() const {
    return 0;
}

bool Piece::PositionCanBeAdjusteInMovesSearch() const {
    return true;
}

bool Piece::NeedsRightAdjustmentInHud() const {
    return false;
}

bool Piece::NeedsLeftAdjustmentInHud() const {
    return false;
}

bool Piece::NeedsDownAdjustmentInHud() const {
    return false;
}

Pht::RenderableObject* Piece::GetGhostPieceRenderable() const {
    return mGhostPieceRenderable.get();
}

Pht::RenderableObject* Piece::GetGhostPieceShadowRenderable() const {
    return mGhostPieceShadowRenderable.get();
}

Pht::RenderableObject* Piece::GetPressedGhostPieceRenderable() const {
    return mPressedGhostPieceRenderable.get();
}

void Piece::InitGrids(const FillGrid& fillGrid,
                      const ClickGrid& clickGrid,
                      BlockColor blockColor,
                      bool isIndivisible) {
    mColor = blockColor;

    mGridNumRows = static_cast<int>(fillGrid.size());
    mGridNumColumns = static_cast<int>(fillGrid.front().size());
    assert(mGridNumRows == mGridNumColumns);
    
    mClickGridNumRows = static_cast<int>(clickGrid.size());
    mClickGridNumColumns = static_cast<int>(clickGrid.front().size());
    assert(mClickGridNumRows == 2 * mGridNumRows && mClickGridNumColumns == 2 * mGridNumColumns);
    
    InitCellGrids(fillGrid, blockColor, isIndivisible);
    InitClickGrids(clickGrid);
    
    mRightOverhangCheckPositions.resize(4);
    mLeftOverhangCheckPositions.resize(4);
    AddOverhangCheckPositions(Rotation::Deg0);
    AddOverhangCheckPositions(Rotation::Deg90);
    AddOverhangCheckPositions(Rotation::Deg180);
    AddOverhangCheckPositions(Rotation::Deg270);
    
    mRightExtremityCheckPositions.resize(4);
    mLeftExtremityCheckPositions.resize(4);
    AddExtremityCheckPositions(Rotation::Deg0);
    AddExtremityCheckPositions(Rotation::Deg90);
    AddExtremityCheckPositions(Rotation::Deg180);
    AddExtremityCheckPositions(Rotation::Deg270);
    
    mTiltedBondChecks.resize(4);
    AddTiltedBondCheck(Rotation::Deg0);
    AddTiltedBondCheck(Rotation::Deg90);
    AddTiltedBondCheck(Rotation::Deg180);
    AddTiltedBondCheck(Rotation::Deg270);

    mDimensions.resize(4);
    AddDimensions(Rotation::Deg0);
    AddDimensions(Rotation::Deg90);
    AddDimensions(Rotation::Deg180);
    AddDimensions(Rotation::Deg270);

    mButtonCenterPositions.resize(4);
    mButtonSizes.resize(4);
    AddButtonPositionAndSize(Rotation::Deg0);
    AddButtonPositionAndSize(Rotation::Deg90);
    AddButtonPositionAndSize(Rotation::Deg180);
    AddButtonPositionAndSize(Rotation::Deg270);
}

void Piece::InitCellGrids(const Piece::FillGrid& fillGrid,
                          BlockColor blockColor,
                          bool isIndivisible) {
    auto reversedFillGrid = fillGrid;
    std::reverse(reversedFillGrid.begin(), reversedFillGrid.end());
    
    auto deg0Grid = InitCellGrid(reversedFillGrid, blockColor, isIndivisible);
    mGrids.push_back(deg0Grid);
    
    auto deg90Grid = RotateGridClockwise90Deg(deg0Grid, Rotation::Deg90);
    mGrids.push_back(deg90Grid);
    
    auto deg180Grid = RotateGridClockwise90Deg(deg90Grid, Rotation::Deg180);
    mGrids.push_back(deg180Grid);
    
    auto deg270Grid = RotateGridClockwise90Deg(deg180Grid, Rotation::Deg270);
    mGrids.push_back(deg270Grid);
}

CellGrid Piece::InitCellGrid(const Piece::FillGrid& fillGrid,
                             BlockColor blockColor,
                             bool isIndivisible) {
    CellGrid result {static_cast<std::size_t>(mGridNumRows)};
    for (auto& row: result) {
        row.resize(mGridNumColumns);
    }
    
    for (auto row = 0; row < mGridNumRows; row++) {
        for (auto column = 0; column < mGridNumColumns; column++) {
            auto& subCell = result[row][column].mFirstSubCell;
            subCell.mFill = fillGrid[row][column];
            subCell.mBonds = MakeBonds(row, column, fillGrid);
            subCell.mBlockKind = ToBlockKind(subCell.mFill);
            subCell.mColor = blockColor;
            subCell.mFlashingBlockAnimation.mState = FlashingBlockAnimationComponent::State::Waiting;
            subCell.mIsPartOfIndivisiblePiece = isIndivisible;
        }
    }
    
    return result;
}

CellGrid Piece::RotateGridClockwise90Deg(const CellGrid& grid, Rotation newRotation) {
    auto result = grid;
    
    for (auto row = 0; row < mGridNumRows; row++) {
        for (auto column = 0; column < mGridNumColumns; column++) {
            auto& resultCell = result[mGridNumColumns - 1 - column][row];
            resultCell = grid[row][column];
            auto& subCell = resultCell.mFirstSubCell;
            subCell.mRotation = newRotation;
            subCell.mFill = RotateCellFillClockwise90Deg(subCell.mFill);
            subCell.mBonds = RotateBondsClockwise90Deg(subCell.mBonds);
        }
    }
    
    return result;
}

void Piece::InitClickGrids(const ClickGrid& clickGrid) {
    auto deg0Grid = clickGrid;
    std::reverse(deg0Grid.begin(), deg0Grid.end());
    mClickGrids.push_back(deg0Grid);
    
    auto deg90Grid = RotateClickGridClockwise90Deg(deg0Grid, Rotation::Deg90);
    mClickGrids.push_back(deg90Grid);
    
    auto deg180Grid = RotateClickGridClockwise90Deg(deg90Grid, Rotation::Deg180);
    mClickGrids.push_back(deg180Grid);
    
    auto deg270Grid = RotateClickGridClockwise90Deg(deg180Grid, Rotation::Deg270);
    mClickGrids.push_back(deg270Grid);
}

ClickGrid Piece::RotateClickGridClockwise90Deg(const ClickGrid& grid, Rotation newRotation) {
    auto result = grid;
    
    for (auto row = 0; row < mClickGridNumRows; row++) {
        for (auto column = 0; column < mClickGridNumColumns; column++) {
            auto& resultCell = result[mClickGridNumColumns - 1 - column][row];
            resultCell = grid[row][column];
        }
    }
    
    return result;
}

void Piece::SetPreviewCellSize(float previewCellSize) {
    mPreviewCellSize = previewCellSize;
}

void Piece::SetNumRotations(int numRotations) {
    mNumRotations = numRotations;
}

void Piece::SetDuplicateMoveCheck(Rotation rotation, const DuplicateMoveCheck& duplicateMoveCheck) {
    mDuplicateMoveChecks[static_cast<int>(rotation)] = duplicateMoveCheck;
}

void Piece::SetGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable) {
    mGhostPieceRenderable = std::move(renderable);
}

void Piece::SetGhostPieceShadowRenderable(std::unique_ptr<Pht::RenderableObject> renderable) {
    mGhostPieceShadowRenderable = std::move(renderable);
}

void Piece::SetPressedGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable) {
    mPressedGhostPieceRenderable = std::move(renderable);
}

Bonds Piece::MakeBonds(int row, int column, const Piece::FillGrid& fillGrid) {
    Bonds bonds;
    
    if (!IsBlock(row, column, fillGrid)) {
        return bonds;
    }
    
    if (IsBlock(row + 1, column, fillGrid)) {
        bonds.mUp = true;
    }

    if (IsBlock(row + 1, column + 1, fillGrid) &&
        !IsBlock(row + 1, column, fillGrid) && !IsBlock(row, column + 1, fillGrid)) {
        bonds.mUpRight = true;
    }

    if (IsBlock(row, column + 1, fillGrid)) {
        bonds.mRight = true;
    }

    if (IsBlock(row - 1, column + 1, fillGrid) &&
        !IsBlock(row, column + 1, fillGrid) && !IsBlock(row - 1, column, fillGrid)) {
        bonds.mDownRight = true;
    }

    if (IsBlock(row - 1, column, fillGrid)) {
        bonds.mDown = true;
    }

    if (IsBlock(row - 1, column - 1, fillGrid) &&
        !IsBlock(row - 1, column, fillGrid) && !IsBlock(row, column - 1, fillGrid)) {
        bonds.mDownLeft = true;
    }
    
    if (IsBlock(row, column - 1, fillGrid)) {
        bonds.mLeft = true;
    }

    if (IsBlock(row + 1, column - 1, fillGrid) &&
        !IsBlock(row + 1, column, fillGrid) && !IsBlock(row, column - 1, fillGrid)) {
        bonds.mUpLeft = true;
    }

    return bonds;
}

bool Piece::IsBlock(int row, int column, const Piece::FillGrid& fillGrid) {
    if (row < 0 || row >= mGridNumRows || column < 0 || column >= mGridNumColumns) {
        return false;
    }
    
    return fillGrid[row][column] != Fill::Empty;
}

void Piece::CalculateMinMax(int& yMax, int& xMin, int& xMax, const CellGrid& grid) const {
    yMax = 0;
    xMin = mGridNumColumns - 1;
    xMax = 0;
    
    for (auto row = 0; row < mGridNumRows; ++row) {
        for (auto column = 0; column < mGridNumColumns; ++column) {
            if (!grid[row][column].IsEmpty()) {
                if (row > yMax) {
                    yMax = row;
                }
                
                if (column > xMax) {
                    xMax = column;
                }
                
                if (column < xMin) {
                    xMin = column;
                }
            }
        }
    }
}

void Piece::AddOverhangCheckPositions(Rotation rotation) {
    const auto& grid = GetGrid(rotation);
    auto yMax = 0;
    auto xMin = 0;
    auto xMax = 0;
    
    CalculateMinMax(yMax, xMin, xMax, grid);
    
    auto index = static_cast<int>(rotation);
    
    mRightOverhangCheckPositions[index] = Pht::IVec2 {xMax + 1, yMax};
    mLeftOverhangCheckPositions[index] = Pht::IVec2 {xMin - 1, yMax};
}

void Piece::AddExtremityCheckPositions(Rotation rotation) {
    const auto& grid = GetGrid(rotation);
    auto yMax = 0;
    auto xMin = 0;
    auto xMax = 0;
    
    CalculateMinMax(yMax, xMin, xMax, grid);
    
    Pht::Optional<Pht::IVec2> rightExtremityCheckPosition;
    
    if (grid[yMax][xMax].IsEmpty()) {
        for (auto row {yMax - 1}; row >= 0; --row) {
            if (!grid[row][xMax].IsEmpty()) {
                rightExtremityCheckPosition = Pht::IVec2 {xMax + 1, row};
                break;
            }
        }
    }
    
    Pht::Optional<Pht::IVec2> leftExtremityCheckPosition;
    
    if (grid[yMax][xMin].IsEmpty()) {
        for (auto row {yMax - 1}; row >= 0; --row) {
            if (!grid[row][xMin].IsEmpty()) {
                leftExtremityCheckPosition = Pht::IVec2 {xMin - 1, row};
                break;
            }
        }
    }
    
    auto index = static_cast<int>(rotation);
    
    mRightExtremityCheckPositions[index] = rightExtremityCheckPosition;
    mLeftExtremityCheckPositions[index] = leftExtremityCheckPosition;
}

void Piece::AddTiltedBondCheck(Rotation rotation) {
    const auto& grid = GetGrid(rotation);
    
    for (auto row = 0; row < mGridNumRows; ++row) {
        for (auto column = 0; column < mGridNumColumns; ++column) {
            if (grid[row][column].mFirstSubCell.mBonds.mUpRight) {
                TiltedBondCheck tiltedBondCheckValue {
                    TiltedBondCheck::Kind::DownLeftToUpRight,
                    Pht::IVec2{column, row}
                };
                
                mTiltedBondChecks[static_cast<int>(rotation)] = tiltedBondCheckValue;
                return;
            }

            if (grid[row][column].mFirstSubCell.mBonds.mUpLeft) {
                TiltedBondCheck tiltedBondCheckValue {
                    TiltedBondCheck::Kind::DownRightToUpLeft,
                    Pht::IVec2{column, row}
                };
                
                mTiltedBondChecks[static_cast<int>(rotation)] = tiltedBondCheckValue;
                return;
            }
        }
    }
    
    mTiltedBondChecks[static_cast<int>(rotation)] = Pht::Optional<TiltedBondCheck> {};
}

void Piece::AddDimensions(Rotation rotation) {
    const auto& grid = GetGrid(rotation);
    
    Dimensions dimensions {
        .mXmin = mGridNumColumns - 1,
        .mXmax = 0,
        .mYmin = mGridNumRows - 1,
        .mYmax = 0
    };
    
    for (auto row = 0; row < mGridNumRows; ++row) {
        for (auto column = 0; column < mGridNumColumns; ++column) {
            if (!grid[row][column].IsEmpty()) {
                if (row < dimensions.mYmin) {
                    dimensions.mYmin = row;
                }

                if (row > dimensions.mYmax) {
                    dimensions.mYmax = row;
                }

                if (column > dimensions.mXmax) {
                    dimensions.mXmax = column;
                }
                
                if (column < dimensions.mXmin) {
                    dimensions.mXmin = column;
                }
            }
        }
    }
    
    mDimensions[static_cast<int>(rotation)] = dimensions;
}

void Piece::AddButtonPositionAndSize(Rotation rotation) {
    const auto& grid = GetClickGrid(rotation);
    auto yMin = mClickGridNumRows - 1;
    auto yMax = 0;
    auto xMin = mClickGridNumColumns - 1;
    auto xMax = 0;
    
    for (auto row = 0; row < mClickGridNumRows; ++row) {
        for (auto column = 0; column < mClickGridNumColumns; ++column) {
            if (grid[row][column] > 0) {
                if (row < yMin) {
                    yMin = row;
                }

                if (row > yMax) {
                    yMax = row;
                }
                
                if (column < xMin) {
                    xMin = column;
                }

                if (column > xMax) {
                    xMax = column;
                }
            }
        }
    }
    
    auto index = static_cast<int>(rotation);
    Pht::Vec2 buttonSize {static_cast<float>(xMax - xMin + 1), static_cast<float>(yMax - yMin + 1)};
    
    Pht::Vec2 centerPosition {
        static_cast<float>(xMin) + buttonSize.x / 2.0f,
        static_cast<float>(yMin) + buttonSize.y / 2.0f
    };
    
    mButtonSizes[index] = buttonSize;
    mButtonCenterPositions[index] = centerPosition;
}
