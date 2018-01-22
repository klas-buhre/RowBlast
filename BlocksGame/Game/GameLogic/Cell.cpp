#include "Cell.hpp"

using namespace BlocksGame;

const int Quantities::numBlockRenderables {5};
const int Quantities::numBlockColors {4};
const int Quantities::numBlockBrightness {3};

const float BlueprintSlotFillAnimation::mInitialOpacity {0.6f};

BlockRenderableKind BlocksGame::ToBlockRenderableKind(Fill fill) {
    switch (fill) {
        case Fill::Empty:
            return BlockRenderableKind::None;
        case Fill::LowerRightHalf:
            return BlockRenderableKind::LowerRightHalf;
        case Fill::UpperRightHalf:
            return BlockRenderableKind::UpperRightHalf;
        case Fill::UpperLeftHalf:
            return BlockRenderableKind::UpperLeftHalf;
        case Fill::LowerLeftHalf:
            return BlockRenderableKind::LowerLeftHalf;
        case Fill::Full:
            return BlockRenderableKind::Full;
    }
}

float BlocksGame::RotationToDeg(Rotation rotation) {
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
