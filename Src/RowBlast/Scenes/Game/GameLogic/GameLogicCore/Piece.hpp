#ifndef Piece_hpp
#define Piece_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "Optional.hpp"
#include "RenderableObject.hpp"

// Game includes.
#include "Cell.hpp"

namespace RowBlast {
    using ClickGrid = std::vector<std::vector<int>>;
    
    class Piece {
    public:
        struct Dimensions {
            int mXmin {0};
            int mXmax {0};
            int mYmin {0};
            int mYmax {0};
        };
        
        struct DuplicateMoveCheck {
            Pht::IVec2 mRelativePosition;
            Rotation mRotation;
        };
        
        struct TiltedBondCheck {
            enum class Kind {
                DownLeftToUpRight,
                DownRightToUpLeft
            };
            
            Kind mKind;
            Pht::IVec2 mPosition;
        };

        Piece();
        virtual ~Piece() {}
        
        const CellGrid& GetGrid(Rotation rotation) const;
        const ClickGrid& GetClickGrid(Rotation rotation) const;
        const Pht::IVec2& GetRightOverhangCheckPosition(Rotation rotation) const;
        const Pht::IVec2& GetLeftOverhangCheckPosition(Rotation rotation) const;
        const Pht::Optional<Pht::IVec2>& GetRightExtremityCheckPosition(Rotation rotation) const;
        const Pht::Optional<Pht::IVec2>& GetLeftExtremityCheckPosition(Rotation rotation) const;
        const Dimensions& GetDimensions(Rotation rotation) const;
        const Pht::Optional<DuplicateMoveCheck>& GetDuplicateMoveCheck(Rotation rotation) const;
        const Pht::Optional<TiltedBondCheck>& GetTiltedBondCheck(Rotation rotation) const;
        Pht::Vec2 GetCenterPosition(Rotation rotation) const;
        const Pht::Vec2& GetButtonCenterPosition(Rotation rotation) const;
        const Pht::Vec2& GetButtonSize(Rotation rotation) const;
        Pht::RenderableObject* GetDraggedPieceRenderable() const;
        Pht::RenderableObject* GetHighlightedDraggedPieceRenderable() const;
        Pht::RenderableObject* GetShadowRenderable() const;
        Pht::RenderableObject* GetGhostPieceRenderable() const;
        Pht::RenderableObject* GetHighlightedGhostPieceRenderable() const;
        
        virtual bool IsBomb() const;
        virtual bool IsRowBomb() const;
        virtual Rotation GetSpawnRotation() const;
        virtual int GetSpawnYPositionAdjustment() const;
        virtual bool PositionCanBeAdjusteInMovesSearch() const;
        virtual bool NeedsRightAdjustmentInHud() const;
        virtual bool NeedsLeftAdjustmentInHud() const;
        virtual bool NeedsDownAdjustmentInHud() const;

        BlockColor GetColor() const {
            return mColor;
        }

        int GetGridNumRows() const {
            return mGridNumRows;
        }

        int GetGridNumColumns() const {
            return mGridNumColumns;
        }

        int GetClickGridNumRows() const {
            return mClickGridNumRows;
        }

        int GetClickGridNumColumns() const {
            return mClickGridNumColumns;
        }
        
        float GetPreviewCellSize() const {
            return mPreviewCellSize;
        }
        
        int GetNumRotations() const {
            return mNumRotations;
        }
        
        bool CanRotateAroundZ() const {
            return mNumRotations > 1;
        }
        
        using FillGrid = std::vector<std::vector<Fill>>;
        
        static constexpr int maxRows {5};
        static constexpr int maxColumns {5};
        static constexpr int numPieceTypes {22};

    protected:
        void InitGrids(const FillGrid& fillGrid,
                       const ClickGrid& clickGrid,
                       BlockColor blockColor,
                       bool isIndivisible = false);
        void SetPreviewCellSize(float previewCellSize);
        void SetNumRotations(int numRotations);
        void SetDuplicateMoveCheck(Rotation rotation, const DuplicateMoveCheck& duplicateMoveCheck);
        void SetDraggedPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetHighlightedDraggedPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetShadowRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetHighlightedGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        
    private:
        void InitCellGrids(const Piece::FillGrid& fillGrid,
                           BlockColor blockColor,
                           bool isIndivisible);
        CellGrid InitCellGrid(const Piece::FillGrid& fillGrid,
                              BlockColor blockColor,
                              bool isIndivisible);
        CellGrid RotateGridClockwise90Deg(const CellGrid& grid, Rotation newRotation);
        Bonds MakeBonds(int row, int column, const Piece::FillGrid& fillGrid);
        bool IsBlock(int row, int column, const Piece::FillGrid& fillGrid);
        void CalculateMinMax(int& yMax, int& xMin, int& xMax, const CellGrid& grid) const;
        void AddOverhangCheckPositions(Rotation rotation);
        void AddExtremityCheckPositions(Rotation rotation);
        void AddTiltedBondCheck(Rotation rotation);
        void AddDimensions(Rotation rotation);
        void InitClickGrids(const ClickGrid& clickGrid);
        ClickGrid RotateClickGridClockwise90Deg(const ClickGrid& grid, Rotation newRotation);
        void AddButtonPositionAndSize(Rotation rotation);
        
        int mGridNumRows {0};
        int mGridNumColumns {0};
        std::vector<CellGrid> mGrids;
        int mClickGridNumRows {0};
        int mClickGridNumColumns {0};
        BlockColor mColor {BlockColor::None};
        std::vector<ClickGrid> mClickGrids;
        int mNumRotations {4};
        std::unique_ptr<Pht::RenderableObject> mDraggedPieceRenderable;
        std::unique_ptr<Pht::RenderableObject> mHighlightedDraggedPieceRenderable;
        std::unique_ptr<Pht::RenderableObject> mShadowRenderable;
        std::unique_ptr<Pht::RenderableObject> mGhostPieceRenderable;
        std::unique_ptr<Pht::RenderableObject> mHighlightedGhostPieceRenderable;
        float mPreviewCellSize {1.0f};
        std::vector<Pht::IVec2> mRightOverhangCheckPositions;
        std::vector<Pht::IVec2> mLeftOverhangCheckPositions;
        std::vector<Pht::Optional<Pht::IVec2>> mRightExtremityCheckPositions;
        std::vector<Pht::Optional<Pht::IVec2>> mLeftExtremityCheckPositions;
        std::vector<Dimensions> mDimensions;
        std::vector<Pht::Optional<DuplicateMoveCheck>> mDuplicateMoveChecks;
        std::vector<Pht::Optional<TiltedBondCheck>> mTiltedBondChecks;
        std::vector<Pht::Vec2> mButtonCenterPositions;
        std::vector<Pht::Vec2> mButtonSizes;
    };
}

#endif
