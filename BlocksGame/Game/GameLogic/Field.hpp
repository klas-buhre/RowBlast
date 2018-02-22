#ifndef Field_hpp
#define Field_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

// Game includes.
#include "Cell.hpp"
#include "Piece.hpp"

namespace BlocksGame {
    class Piece;
    class FallingPiece;
    class Level;

    struct RemovedSubCell {
        Pht::IVec2 mPosition;
        Rotation mRotation;
        BlockRenderableKind mRenderableKind {BlockRenderableKind::None};
        BlockColor mColor;
        bool mIsLevel {false};
    };
    
    enum class IsCollision {
        Yes,
        NextWillBe,
        No
    };
    
    struct PieceBlocks {
        const CellGrid& mGrid;
        int mNumRows {0};
        int mNumColumns {0};
    };
    
    using FilledRowIndices = Pht::StaticVector<int, Piece::maxRows>;
    
    struct FilledRowsResult {
        FilledRowIndices mFilledRowIndices;
        float mPieceCellsInFilledRows {0.0f};
    };
    
    class Field {
    public:
        static constexpr int maxNumColumns {9};
        static constexpr int maxNumRows {18};
        
        struct PieceBlockCoord {
            Pht::IVec2 mPosition;
            bool mIsFirstSubCell {true};
        };

        using PieceBlockCoords = Pht::StaticVector<PieceBlockCoord, maxNumColumns * maxNumRows * 2>;
        using CollisionPoints = Pht::StaticVector<Pht::IVec2, maxNumColumns * maxNumRows>;

        struct CollisionResult {
            IsCollision mIsCollision {IsCollision::No};
            CollisionPoints mCollisionPoints;
        };
    
        Field();
        
        void Init(const Level& level);
        void RestorePreviousState();
        void OnEndOfFrame();
        void SetChanged();
        int DetectCollisionDown(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectCollisionRight(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectCollisionLeft(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        void CheckCollision(CollisionResult& result,
                            const PieceBlocks& pieceBlocks,
                            const Pht::IVec2& position,
                            const Pht::IVec2& scanDirection,
                            bool isScanStart) const;
        int DetectFreeSpaceUp(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectFreeSpaceDown(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectFreeSpaceRight(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectFreeSpaceLeft(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        CollisionPoints GetOccupiedArea(const PieceBlocks& pieceBlocks,
                                        const Pht::IVec2& position) const;
        void LandFallingPiece(const FallingPiece& fallingPiece);
        void LandPieceBlocks(const PieceBlocks& pieceBlocks,
                             int pieceId,
                             const Pht::IVec2& position,
                             bool updateCellPosition,
                             bool startBlueprintCellAnimation);
        void ManageWelds();
        void MergeTriangleBlocksIntoCube(const Pht::IVec2& position);
        void PullDownLoosePieces();
        int GetNumRowsInOneScreen() const;
        bool AnyFilledRows() const;
        int CalculateNumLevelBlocks() const;
        int CalculateNumEmptyBlueprintSlots() const;
        int CalculateHighestLevelBlock() const;
        int AccordingToBlueprintHeight() const;
        int GetNumCellsAccordingToBlueprintInVisibleRows() const;
        float GetBuildHolesAreaInVisibleRows() const;
        float GetBuildWellsAreaInVisibleRows() const;
        float GetBurriedHolesAreaInVisibleRows() const;
        float GetWellsAreaInVisibleRows() const;
        int GetNumTransitionsInVisibleRows() const;
        
        using RemovedSubCells = Pht::StaticVector<RemovedSubCell, maxNumColumns * maxNumRows>;
        
        RemovedSubCells RemoveFilledRows();
        RemovedSubCells RemoveRow(int rowIndex);
        RemovedSubCells RemoveAreaOfSubCells(const Pht::IVec2& areaPos,
                                             const Pht::IVec2& areaSize);
        RemovedSubCells RemoveAllNonEmptySubCells();
        void RemovePiece(int pieceId,
                         const Pht::IVec2& position,
                         int pieceNumRows,
                         int pieceNumColumns);
        FilledRowsResult MarkFilledRowsAndCountPieceCellsInFilledRows(int pieceId);
        void UnmarkFilledRows(const FilledRowIndices& filledRowIndices);
        
        const Cell& GetCell(int row, int column) const {
            assert(row >= 0 && column >= 0 && row < mNumRows && column < mNumColumns);
            return mGrid[row][column];
        }

        Cell& GetCell(int row, int column) {
            assert(row >= 0 && column >= 0 && row < mNumRows && column < mNumColumns);
            return mGrid[row][column];
        }
        
        const Cell& GetCell(const Pht::IVec2& position) const {
            return GetCell(position.y, position.x);
        }

        Cell& GetCell(const Pht::IVec2& position) {
            return GetCell(position.y, position.x);
        }

        int GetNumColumns() const {
            return mNumColumns;
        }

        int GetNumRows() const {
            return mNumRows;
        }
        
        void SetLowestVisibleRow(int lowestVisibleRow) {
            mLowestVisibleRow = lowestVisibleRow;
        }
        
        int GetLowestVisibleRow() const {
            return mLowestVisibleRow;
        }
        
        const BlueprintCellGrid* GetBlueprintGrid() const {
            return mBlueprintGrid.get();
        }

        BlueprintCellGrid* GetBlueprintGrid() {
            return mBlueprintGrid.get();
        }
        
        bool HasChanged() const {
            return mHasChanged;
        }
        
    private:
        void SaveState();
        void CopyGridNoAlloc(CellGrid& to, const CellGrid& from);
        bool IsCellAccordingToBlueprint(int row, int column) const;
        int GetNumTransitionsInColumns() const;
        int GetNumTransitionsInRows() const;
        Pht::IVec2 ScanUntilCollision(const PieceBlocks& pieceBlocks,
                                      Pht::IVec2 position,
                                      const Pht::IVec2& step) const;
        Pht::IVec2 ScanUntilNoCollision(const PieceBlocks& pieceBlocks,
                                        Pht::IVec2 position,
                                        const Pht::IVec2& step) const;
        void MakeDiagonalWeld(Cell& cell);
        void MakeWelds(SubCell& subCell, const Pht::IVec2& position);
        bool ShouldBeUpWeld(const SubCell& subCell, const Pht::IVec2& position) const;
        bool ShouldBeRightWeld(const SubCell& subCell, const Pht::IVec2& position) const;
        bool ShouldBeDownWeld(const SubCell& subCell, const Pht::IVec2& position) const;
        bool ShouldBeLeftWeld(const SubCell& subCell, const Pht::IVec2& position) const;
        void BreakRedundantWelds(SubCell& subCell, const Pht::IVec2& position);
        bool UpRightWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        bool DownRightWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        bool DownLeftWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        bool UpLeftWeldWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        void PullDownPiece(int row, int column, ScanDirection scanDirection);
        void PullDownPiece(const SubCell& subCell,
                           const Pht::IVec2& position,
                           ScanDirection scanDirection);
        PieceBlocks ExtractPieceBlocks(Pht::IVec2& piecePosition,
                                       BlockColor color,
                                       const Pht::IVec2& scanPosition,
                                       ScanDirection scanDirection);
        void FindPieceBlocks(BlockColor color, const Pht::IVec2& position);
        void ResetAllCellsTriedScanDirection();
        void ClearPieceBlockGrid();
        void LandPulledDownPieceBlocks(const PieceBlocks& pieceBlocks, const Pht::IVec2& position);
        void RemoveRowImpl(int rowIndex, Field::RemovedSubCells& removedSubCells);
        void BreakCellDownWelds(int row, int column);
        void BreakCellUpWelds(int row, int column);
        void BreakCellRightWelds(int row, int column);
        void BreakCellLeftWelds(int row, int column);
        void BreakLowerLeftWeld(int row, int column);
        void BreakUpperLeftWeld(int row, int column);
        void BreakUpperRightWeld(int row, int column);
        void BreakLowerRightWeld(int row, int column);
        void RemoveWholePiece(int pieceId, Field::RemovedSubCells& removedSubCells);
        void RemoveMatchingSubCell(int pieceId,
                                   Field::RemovedSubCells& removedSubCells,
                                   SubCell& subCell);
        void ProcessSubCell(Field::RemovedSubCells& removedSubCells, const SubCell& subCell);
        void SaveSubCellAndCancelFill(Field::RemovedSubCells& removedSubCells,
                                      const SubCell& subCell);

        CellGrid mGrid;
        CellGrid mPreviousGrid;
        std::unique_ptr<BlueprintCellGrid> mBlueprintGrid;
        int mNumColumns {0};
        int mNumRows {0};
        int mLowestVisibleRow {0};
        bool mHasChanged {false};
        CellGrid mPieceBlockGrid;
        mutable CollisionResult mCollisionResult;
        PieceBlockCoords mPieceBlockCoords;
    };
}

#endif
