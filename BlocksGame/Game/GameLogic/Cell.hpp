#ifndef Cell_hpp
#define Cell_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "Material.hpp"

namespace Pht {
    class SceneObject;
    class RenderableObject;
}

namespace BlocksGame {
    enum class Fill {
        Empty,
        LowerRightHalf,
        UpperRightHalf,
        UpperLeftHalf,
        LowerLeftHalf,
        Full
    };

    enum class Rotation {
        Deg0 = 0,
        Deg90,
        Deg180,
        Deg270
    };

    float RotationToDeg(Rotation rotation);
    
    enum class ScanDirection {
        None,
        Right,
        Left
    };
    
    struct Welds {
        bool mUp {false};
        bool mUpRight {false};
        bool mRight {false};
        bool mDownRight {false};
        bool mDown {false};
        bool mDownLeft {false};
        bool mLeft {false};
        bool mUpLeft {false};
    };
    
    struct FlashingBlockAnimation {
        bool mIsActive {false};
        float mElapsedTime {0.0f};
        Pht::Color mColorAdd;
    };

    struct SubCell {
        bool IsFull() const {
            return mFill == Fill::Full;
        }

        bool IsEmpty() const {
            return mFill == Fill::Empty;
        }
        
        Fill mFill {Fill::Empty};
        Welds mWelds;
        int mPieceId {0};
        Pht::Vec2 mPosition {0.0f, 0.0f};
        Pht::RenderableObject* mRenderableObject {nullptr};
        Pht::RenderableObject* mWeldRenderableObject {nullptr};
        FlashingBlockAnimation mFlashingBlockAnimation;
        Rotation mRotation {Rotation::Deg0};
        bool mIsLevel {false};
        bool mIsPartOfIndivisiblePiece {false};
        bool mIsFound {false};
        ScanDirection mTriedScanDirection {ScanDirection::None};
    };
    
    struct Cell {
        bool IsFull() const;
        
        bool IsEmpty() const {
            return mFirstSubCell.IsEmpty() && mSecondSubCell.IsEmpty();
        }
        
        SubCell mFirstSubCell;
        SubCell mSecondSubCell;
        bool mIsInFilledRow {false};
    };
    
    using CellGrid = std::vector<std::vector<Cell>>;
    
    struct BlueprintSlotRenderables {
        Pht::RenderableObject* mSlot {nullptr};
        Pht::RenderableObject* mAnimation {nullptr};
    };
    
    struct BlueprintSlotFillAnimation {
        bool mIsActive {false};
        float mElapsedTime {0.0f};
        float mScale {1.0f};
        float mOpacity {mInitialOpacity};
        Pht::SceneObject* mSceneObject {nullptr};
        
        static const float mInitialOpacity;
    };
    
    struct BlueprintCell {
        Fill mFill {Fill::Empty};
        Pht::SceneObject* mSceneObject {nullptr};
        BlueprintSlotFillAnimation mAnimation;
    };
    
    using BlueprintCellGrid = std::vector<std::vector<BlueprintCell>>;
}

#endif
