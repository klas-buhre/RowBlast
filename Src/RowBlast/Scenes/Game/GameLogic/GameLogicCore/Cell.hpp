#ifndef Cell_hpp
#define Cell_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class SceneObject;
}

namespace RowBlast {
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

    enum class BlockKind {
        LowerRightHalf,
        UpperRightHalf,
        UpperLeftHalf,
        LowerLeftHalf,
        Full,
        Bomb,
        RowBomb,
        BigAsteroid,
        BigAsteroidMainCell,
        SmallAsteroid,
        ClearedRowBlock,
        None
    };

    BlockKind ToBlockKind(Fill fill);
    
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
            WeldAppearingAndSemiFlashing,
            WeldAtFullScale,
            WeldDisappearing,
            Inactive
        };
        
        bool IsActive() const {
            return mState != State::Inactive;
        }
        
        bool IsSemiFlashing() const {
            return mState == State::WeldAppearingAndSemiFlashing;
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
        BlueprintFillFlashing,
        SemiFlashing
    };
    
    namespace Quantities {
        extern const int numBlockRenderables;
        extern const int numBlockColors;
        extern const int numBlockBrightness;
        extern const int numWeldBrightness;
    };
    
    struct FlashingBlockAnimation {
        enum class State {
            Waiting,
            Active,
            Inactive
        };
        
        bool IsActive() const {
            return mState == State::Active;
        }
        
        State mState {State::Inactive};
        BlockBrightness mBrightness {BlockBrightness::Normal};
    };
    
    struct FallingBlockAnimation {
        enum class State {
            Falling,
            Bouncing,
            Inactive
        };
        
        static constexpr float fallingPieceBounceVelocity {-12.5f};
        
        float mVelocity {0.0f};
        State mState {State::Inactive};
        bool mShouldBounce {true};
    };

    struct SubCell {
        bool FillsLowerCellSide() const;
        bool FillsUpperCellSide() const;
        bool FillsRightCellSide() const;
        bool FillsLeftCellSide() const;
        bool IsBomb() const;
        bool IsNonBlockObject() const;
        bool IsAsteroid() const;
        bool IsBigAsteroid() const;

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
        BlockKind mBlockKind {BlockKind::None};
        BlockColor mColor {BlockColor::None};
        Rotation mRotation {Rotation::Deg0};
        FallingBlockAnimation mFallingBlockAnimation;
        FlashingBlockAnimation mFlashingBlockAnimation;
        bool mIsGrayLevelBlock {false};
        bool mIsPartOfIndivisiblePiece {false};
        bool mIsFound {false};
        ScanDirection mTriedScanDirection {ScanDirection::None};
        bool mIsPulledDown {false};
    };
    
    struct Cell {
        bool IsFull() const;

        bool IsEmpty() const {
            return mFirstSubCell.IsEmpty() && mSecondSubCell.IsEmpty();
        }
        
        SubCell mFirstSubCell;
        SubCell mSecondSubCell;
        bool mIsInFilledRow {false};
        bool mIsShiftedDown {false};
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
