#ifndef Cell_hpp
#define Cell_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "Material.hpp" // TODO: remove

namespace Pht {
    class SceneObject;
    class RenderableObject; // TODO: remove
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
    
    enum class BlockRenderable {
        LowerRightHalf,
        UpperRightHalf,
        UpperLeftHalf,
        LowerLeftHalf,
        Full,
        None
    };
    
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
        Pht::Color mColorAdd; // TODO: remove
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
        Pht::RenderableObject* mRenderableObject {nullptr}; // TODO: remove
        Pht::RenderableObject* mWeldRenderableObject {nullptr}; // TODO: remove
        BlockRenderable mBlockRenderable {BlockRenderable::None};
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
