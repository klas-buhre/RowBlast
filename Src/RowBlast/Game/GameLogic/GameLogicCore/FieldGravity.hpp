#ifndef FieldGravity_hpp
#define FieldGravity_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Field.hpp"

namespace RowBlast {
    class FieldGravity {
    public:
        struct PieceBlockCoord {
            Pht::IVec2 mPosition;
            bool mIsFirstSubCell {true};
        };

        using PieceBlockCoords = Pht::StaticVector<PieceBlockCoord, Field::maxNumColumns * Field::maxNumRows * 2>;

        FieldGravity(Field& field);
        
        void PullDownLoosePieces();
        void ShiftFieldDown(int rowIndex);
        void DetectBlocksThatShouldNotBounce();

    private:
        enum class IsFloating {
            Yes,
            No,
            Unknown
        };

        void PullDownPiece(int row, int column, ScanDirection scanDirection);
        void PullDownPiece(const SubCell& subCell,
                           const Pht::IVec2& position,
                           ScanDirection scanDirection);
        PieceBlocks ExtractPieceBlocks(Pht::IVec2& piecePosition,
                                       BlockColor color,
                                       const Pht::IVec2& scanPosition,
                                       ScanDirection scanDirection);
        void FindPieceClusterBlocks(BlockColor color, const Pht::IVec2& position);
        void FindPieceBlocks(BlockColor color, const Pht::IVec2& position);
        void FindAsteroidCells(const Pht::IVec2& position);
        void ResetAllCellsTriedScanDirection();
        void ClearPieceBlockGrid();
        void LandPulledDownPieceBlocks(const PieceBlocks& pieceBlocks, const Pht::IVec2& position);
        bool ShiftGrayBlocksDown(int rowIndex);
        void ResetAllCellsShiftedDownFlag();
        IsFloating IsBlockStructureFloating(const Pht::IVec2& gridPosition);
        bool IsOutsideVisibleField(const Pht::IVec2& gridPosition);
        void SetShouldNotBounce(const Pht::IVec2& gridPosition);
        void SetIsScanned(const Pht::IVec2& gridPosition);
        void ResetAllCellsFoundFlag();

        Field& mField;
        CellGrid mPieceBlockGrid;
        PieceBlockCoords mPieceBlockCoords;
        bool mAnyPiecesPulledDown {false};
    };
}

#endif
