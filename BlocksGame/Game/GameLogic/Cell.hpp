#ifndef Cell_hpp
#define Cell_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class SceneObject;
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
    
    enum class BlockRenderableKind {
        LowerRightHalf,
        UpperRightHalf,
        UpperLeftHalf,
        LowerLeftHalf,
        Full,
        None
    };
    
    BlockRenderableKind ToBlockRenderableKind(Fill fill);
    
    enum class BlockColor {
        Red,
        Green,
        Blue,
        Yellow,
        None
    };

    float RotationToDeg(Rotation rotation);
    
    enum class ScanDirection {
        None,
        Right,
        Left
    };
    
    struct WeldAnimation {
        enum class State {
            WeldAppearing,
            WeldDisappearing,
            Inactive
        };
        
        bool IsActive() const {
            return mState != State::Inactive;
        }
        
        State mState {State::Inactive};
        float mScale {1.0f};
    };
    
    struct WeldAnimations {
        WeldAnimation mUp;
        WeldAnimation mRight;
        WeldAnimation mUpRight;
        WeldAnimation mUpLeft;
        WeldAnimation mDiagonal;
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
        bool mDiagonal {false};
        WeldAnimations mAnimations;
    };
    
    enum class BlockBrightness {
        Normal,
        Flashing,
        BlueprintFillFlashing
    };
    
    namespace Quantities {
        extern const int numBlockRenderables;
        extern const int numBlockColors;
        extern const int numBlockBrightness;
    };
    
    struct FlashingBlockAnimation {
        bool mIsActive {false};
        float mElapsedTime {0.0f};
        BlockBrightness mBrightness {BlockBrightness::Normal};
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
        BlockRenderableKind mBlockRenderableKind {BlockRenderableKind::None};
        BlockColor mColor {BlockColor::None};
        Rotation mRotation {Rotation::Deg0};
        FlashingBlockAnimation mFlashingBlockAnimation;
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
