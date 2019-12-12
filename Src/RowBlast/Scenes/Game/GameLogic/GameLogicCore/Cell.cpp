#include "Cell.hpp"

using namespace RowBlast;

const int Quantities::numBlockRenderables {5};
const int Quantities::numBlockColors {4};
const int Quantities::numBlockBrightness {3};
const int Quantities::numBondBrightness {4};

const float BlueprintSlotFillAnimation::mInitialOpacity {0.6f};

BlockKind RowBlast::ToBlockKind(Fill fill) {
    switch (fill) {
        case Fill::Empty:
            return BlockKind::None;
        case Fill::LowerRightHalf:
            return BlockKind::LowerRightHalf;
        case Fill::UpperRightHalf:
            return BlockKind::UpperRightHalf;
        case Fill::UpperLeftHalf:
            return BlockKind::UpperLeftHalf;
        case Fill::LowerLeftHalf:
            return BlockKind::LowerLeftHalf;
        case Fill::Full:
            return BlockKind::Full;
    }
}

float RowBlast::RotationToDeg(Rotation rotation) {
    switch (rotation) {
        case Rotation::Deg0:
            return 0.0f;
        case Rotation::Deg90:
            return -90.0f;
        case Rotation::Deg180:
            return -180.0f;
        case Rotation::Deg270:
            return -270.0f;
    }
}

bool SubCell::FillsLowerCellSide() const {
    switch (mFill) {
        case Fill::Full:
        case Fill::LowerLeftHalf:
        case Fill::LowerRightHalf:
            return true;
        default:
            return false;
    }
}

bool SubCell::FillsUpperCellSide() const {
    switch (mFill) {
        case Fill::Full:
        case Fill::UpperLeftHalf:
        case Fill::UpperRightHalf:
            return true;
        default:
            return false;
    }
}

bool SubCell::FillsRightCellSide() const {
    switch (mFill) {
        case Fill::Full:
        case Fill::UpperRightHalf:
        case Fill::LowerRightHalf:
            return true;
        default:
            return false;
    }
}

bool SubCell::FillsLeftCellSide() const {
    switch (mFill) {
        case Fill::Full:
        case Fill::UpperLeftHalf:
        case Fill::LowerLeftHalf:
            return true;
        default:
            return false;
    }
}

bool SubCell::IsBomb() const {
    switch (mBlockKind) {
        case BlockKind::Bomb:
        case BlockKind::RowBomb:
            return true;
        default:
            return false;
    }
}

bool SubCell::IsNonBlockObject() const {
    switch (mBlockKind) {
        case BlockKind::Bomb:
        case BlockKind::RowBomb:
        case BlockKind::BigAsteroid:
        case BlockKind::BigAsteroidMainCell:
        case BlockKind::SmallAsteroid:
            return true;
        default:
            return false;
    }
}

bool SubCell::IsAsteroid() const {
    switch (mBlockKind) {
        case BlockKind::BigAsteroid:
        case BlockKind::BigAsteroidMainCell:
        case BlockKind::SmallAsteroid:
            return true;
        default:
            return false;
    }
}

bool SubCell::IsBigAsteroid() const {
    switch (mBlockKind) {
        case BlockKind::BigAsteroid:
        case BlockKind::BigAsteroidMainCell:
            return true;
        default:
            return false;
    }
}

bool Cell::IsFull() const {
    switch (mFirstSubCell.mFill) {
        case Fill::Empty:
            return false;
        case Fill::Full:
            return true;
        default:
            if (mSecondSubCell.IsEmpty()) {
                return false;
            }
            return true;
    }
}
