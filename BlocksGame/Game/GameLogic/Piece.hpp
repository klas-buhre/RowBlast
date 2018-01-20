#ifndef Piece_hpp
#define Piece_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "RenderableObject.hpp"
#include "Vector.hpp"
#include "Optional.hpp"

// Game includes.
#include "Cell.hpp"

#define HIGH_DETAIL

namespace BlocksGame {
    using ClickGrid = std::vector<std::vector<int>>;
    
    class Piece {
    public:
        Piece();
        virtual ~Piece() {}
        
        const CellGrid& GetGrid(Rotation rotation) const;
        const ClickGrid& GetClickGrid(Rotation rotation) const;
        const Pht::IVec2& GetRightOverhangCheckPosition(Rotation rotation) const;
        const Pht::IVec2& GetLeftOverhangCheckPosition(Rotation rotation) const;
        const Pht::Optional<Pht::IVec2>& GetRightExtremityCheckPosition(Rotation rotation) const;
        const Pht::Optional<Pht::IVec2>& GetLeftExtremityCheckPosition(Rotation rotation) const;
        Pht::Vec2 GetCenterPosition(Rotation rotation) const;
        const Pht::Vec2& GetButtonCenterPosition(Rotation rotation) const;
        const Pht::Vec2& GetButtonSize(Rotation rotation) const;
        const Pht::RenderableObject& GetFirstRenderable() const;
        const Pht::RenderableObject* GetGhostPieceRenderable() const;
        const Pht::RenderableObject* GetFilledGhostPieceRenderable() const;
        const Pht::RenderableObject* GetPressedGhostPieceRenderable() const;
        
        virtual bool CanRotateAroundZ() const;
        virtual bool IsBomb() const;
        virtual bool IsRowBomb() const;
        virtual int GetNumEmptyTopRows() const;
        
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
        
        using FillGrid = std::vector<std::vector<Fill>>;
        using RenderableGrid = std::vector<std::vector<Pht::RenderableObject*>>;
        
        static constexpr int maxRows {5};
        static constexpr int maxColumns {5};
        static constexpr int numPieceTypes {16};

    protected:
        void InitGrids(const RenderableGrid& renderableGrid,
                       const FillGrid& fillGrid,
                       const ClickGrid& clickGrid,
                       BlockColor blockColor,
                       std::unique_ptr<Pht::RenderableObject> weldRenderable,
                       bool isIndivisible = false);
        void SetPreviewCellSize(float previewCellSize);
        void SetNumRotations(int numRotations);
        void AddRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetFilledGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        void SetPressedGhostPieceRenderable(std::unique_ptr<Pht::RenderableObject> renderable);
        
    private:
        void InitCellGrids(const Piece::RenderableGrid& renderableGrid,
                           const Piece::FillGrid& fillGrid,
                           BlockColor blockColor,
                           bool isIndivisible);
        CellGrid InitCellGrid(const Piece::RenderableGrid& renderableGrid,
                              const Piece::FillGrid& fillGrid,
                              BlockColor blockColor,
                              bool isIndivisible);
        CellGrid RotateGridClockwise90Deg(const CellGrid& grid, Rotation newRotation);
        Welds MakeWelds(int row, int column, const Piece::FillGrid& fillGrid);
        bool IsBlock(int row, int column, const Piece::FillGrid& fillGrid);
        void CalculateMinMax(int& yMax, int& xMin, int& xMax, const CellGrid& grid) const;
        void AddOverhangCheckPositions(Rotation rotation);
        void AddExtremityCheckPositions(Rotation rotation);
        void InitClickGrids(const ClickGrid& clickGrid);
        ClickGrid RotateClickGridClockwise90Deg(const ClickGrid& grid, Rotation newRotation);
        void AddButtonPositionAndSize(Rotation rotation);
        
        int mGridNumRows {0};
        int mGridNumColumns {0};
        std::vector<CellGrid> mGrids;
        int mClickGridNumRows {0};
        int mClickGridNumColumns {0};
        std::vector<ClickGrid> mClickGrids;
        int mNumRotations {4};
        std::vector<std::unique_ptr<Pht::RenderableObject>> mRenderables;
        std::unique_ptr<Pht::RenderableObject> mWeldRenderable;
        std::unique_ptr<Pht::RenderableObject> mGhostPieceRenderable;
        std::unique_ptr<Pht::RenderableObject> mPressedGhostPieceRenderable;
        float mPreviewCellSize {1.0f};
        std::vector<Pht::IVec2> mRightOverhangCheckPositions;
        std::vector<Pht::IVec2> mLeftOverhangCheckPositions;
        std::vector<Pht::Optional<Pht::IVec2>> mRightExtremityCheckPositions;
        std::vector<Pht::Optional<Pht::IVec2>> mLeftExtremityCheckPositions;
        std::vector<Pht::Vec2> mButtonCenterPositions;
        std::vector<Pht::Vec2> mButtonSizes;
    };
}

#endif
