#include "Cell.hpp"

using namespace BlocksGame;

const float BlueprintSlotFillAnimation::mInitialOpacity {0.6f};
 
float BlocksGame::RotationToDeg(Rotation rotation) {
    switch (rotation) {
        case Rotation::Deg0:
            return 0.0f;
        case Rotation::Deg90:
            return 90.0f;
        case Rotation::Deg180:
            return 180.0f;
        case Rotation::Deg270:
            return 270.0f;
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
