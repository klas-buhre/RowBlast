#ifndef Field_hpp
#define Field_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

// Game includes.
#include "Cell.hpp"
#include "Piece.hpp"

namespace RowBlast {
    class Piece;
    class FallingPiece;
    class Level;

    struct RemovedSubCell {
        struct Flags {
            bool mIsGrayLevelBlock : 1;
            bool mIsAsteroidFragment : 1;
            bool mIsPulledDown : 1;
        };

        Pht::Vec2 mExactPosition;
        Pht::IVec2 mGridPosition;
        Rotation mRotation;
        BlockKind mBlockKind {BlockKind::None};
        BlockColor mColor;
        FlashingBlockAnimationComponent::State mFlashingBlockAnimationState {FlashingBlockAnimationComponent::State::Inactive};
        int mPieceId {0};
        Flags mFlags;
    };
    
    enum class PieceFilledSlotKind {
        Blueprint,
        OutsideBlueprint
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
    
    struct FilledRowsResultWithPieceCells {
        FilledRowIndices mFilledRowIndices;
        float mPieceCellsInFilledRows {0.0f};
    };

    struct FilledRowsResultWithGrayLevelCells {
        FilledRowIndices mFilledRowIndices;
        float mGrayLevelCellsInFilledRows {0.0f};
    };
    
    static constexpr int validCell {1};
    static constexpr int invalidCell {-1};
    
    using ValidArea = std::vector<std::vector<int>>;

    class Field {
    public:
        static constexpr int maxNumColumns {9};
        static constexpr int maxNumRows {18};
        static constexpr int numRowsUpToSpawningArea {12};
        
        using CollisionPoints = Pht::StaticVector<Pht::IVec2, maxNumColumns * maxNumRows>;

        struct CollisionResult {
            IsCollision mIsCollision {IsCollision::No};
            CollisionPoints mCollisionPoints;
        };

        struct ImpactedBomb {
            Pht::IVec2 mPosition;
            BlockKind mKind;
        };
        
        struct PieceFilledSlot {
            Pht::IVec2 mPosition;
            PieceFilledSlotKind mKind {PieceFilledSlotKind::OutsideBlueprint};
        };
        
        using ImpactedBombs = Pht::StaticVector<ImpactedBomb, maxNumColumns>;
        using RemovedSubCells = Pht::StaticVector<RemovedSubCell, maxNumColumns * maxNumRows>;
        using PieceFilledSlots = Pht::StaticVector<PieceFilledSlot, Piece::maxColumns * Piece::maxRows>;
        
        void Init(const Level& level);
        void RestorePreviousState();
        void SaveState();
        void OnEndOfFrame();
        void SetChanged();
        void SaveInTempGrid();
        void RestoreFromTempGrid();
        void SetBlocksYPositionAndBounceFlag();
        int DetectCollisionDown(const PieceBlocks& pieceBlocks,
                                const Pht::IVec2& position,
                                ValidArea* validArea = nullptr) const;
        int DetectCollisionRight(const PieceBlocks& pieceBlocks,
                                 const Pht::IVec2& position,
                                 ValidArea* validArea = nullptr) const;
        int DetectCollisionLeft(const PieceBlocks& pieceBlocks,
                                const Pht::IVec2& position,
                                ValidArea* validArea = nullptr) const;
        void CheckCollision(CollisionResult& result,
                            const PieceBlocks& pieceBlocks,
                            const Pht::IVec2& position,
                            const Pht::IVec2& scanDirection,
                            bool isScanStart,
                            ValidArea* validArea = nullptr) const;
        int DetectFreeSpaceUp(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectFreeSpaceDown(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectFreeSpaceRight(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        int DetectFreeSpaceLeft(const PieceBlocks& pieceBlocks, const Pht::IVec2& position) const;
        CollisionPoints GetOccupiedArea(const PieceBlocks& pieceBlocks,
                                        const Pht::IVec2& position) const;
        ImpactedBombs DetectImpactedBombs(const PieceBlocks& pieceBlocks,
                                          const Pht::IVec2& position) const;
        PieceFilledSlots CalculatePieceFilledSlots(const FallingPiece& fallingPiece);
        void LandFallingPiece(const FallingPiece& fallingPiece, bool startBounceAnimation);
        void LandPieceBlocks(const PieceBlocks& pieceBlocks,
                             int pieceId,
                             const Pht::IVec2& position,
                             bool updateCellPosition,
                             bool startBlueprintCellAnimation,
                             bool startBounceAnimation);
        void ManageBonds();
        void MergeTriangleBlocksIntoCube(const Pht::IVec2& position);
        int GetNumRowsInOneScreen() const;
        bool AnyFilledRows() const;
        int CalculateNumLevelBlocks() const;
        int CalculateNumEmptyBlueprintSlots() const;
        Pht::Optional<int> CalculateHighestLevelBlock() const;
        Pht::Optional<int> CalculateHighestBlockInSpawningArea(int lowestVisibleRow) const;
        Pht::Optional<int> CalculateAsteroidRow() const;
        int AccordingToBlueprintHeight() const;
        RemovedSubCells ClearFilledRows();
        void RemoveClearedRows();
        RemovedSubCells RemoveRow(int rowIndex);
        RemovedSubCells RemoveAreaOfSubCells(const Pht::IVec2& areaPos,
                                             const Pht::IVec2& areaSize,
                                             bool removeCorners);
        RemovedSubCells RemoveAllNonEmptySubCells();
        void RemovePiece(int pieceId,
                         const Pht::IVec2& position,
                         int pieceNumRows,
                         int pieceNumColumns);
        FilledRowsResultWithPieceCells MarkFilledRowsAndCountPieceCellsInFilledRows(int pieceId);
        FilledRowsResultWithGrayLevelCells MarkFilledRowsAndCountGrayLevelCellsInFilledRows();
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
        friend class FieldAnalyzer;
        friend class FieldGravitySystem;
        
        void CopyGridNoAlloc(CellGrid& to, const CellGrid& from);
        bool IsCellAccordingToBlueprint(int row, int column) const;
        Pht::IVec2 ScanUntilCollision(const PieceBlocks& pieceBlocks,
                                      Pht::IVec2 position,
                                      const Pht::IVec2& step,
                                      ValidArea* validArea = nullptr) const;
        Pht::IVec2 ScanUntilNoCollision(const PieceBlocks& pieceBlocks,
                                        Pht::IVec2 position,
                                        const Pht::IVec2& step) const;
        void MakeDiagonalBond(Cell& cell);
        void MakeBonds(SubCell& subCell, const Pht::IVec2& position);
        bool ShouldBeUpBond(const SubCell& subCell, const Pht::IVec2& position) const;
        bool ShouldBeRightBond(const SubCell& subCell, const Pht::IVec2& position) const;
        bool ShouldBeDownBond(const SubCell& subCell, const Pht::IVec2& position) const;
        bool ShouldBeLeftBond(const SubCell& subCell, const Pht::IVec2& position) const;
        void BreakRedundantBonds(SubCell& subCell, const Pht::IVec2& position);
        bool UpRightBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        bool DownRightBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        bool DownLeftBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        bool UpLeftBondWouldBeRedundant(const SubCell& subCell, const Pht::IVec2& position) const;
        void RemoveRowImpl(int rowIndex, Field::RemovedSubCells& removedSubCells);
        void BreakCellDownBonds(int row, int column);
        void BreakCellUpBonds(int row, int column);
        void BreakCellRightBonds(int row, int column);
        void BreakCellLeftBonds(int row, int column);
        void BreakLowerLeftBond(int row, int column);
        void BreakUpperLeftBond(int row, int column);
        void BreakUpperRightBond(int row, int column);
        void BreakLowerRightBond(int row, int column);
        void RemoveWholePiece(int pieceId, Field::RemovedSubCells& removedSubCells);
        void RemoveMatchingSubCell(int pieceId,
                                   Field::RemovedSubCells& removedSubCells,
                                   SubCell& subCell,
                                   int row,
                                   int column);
        void ProcessSubCell(Field::RemovedSubCells& removedSubCells,
                            const SubCell& subCell,
                            int row,
                            int column);
        void SaveSubCellAndCancelFill(Field::RemovedSubCells& removedSubCells,
                                      const SubCell& subCell,
                                      int row,
                                      int column);

        CellGrid mGrid;
        CellGrid mPreviousGrid;
        CellGrid mTempGrid;
        std::unique_ptr<BlueprintCellGrid> mBlueprintGrid;
        int mNumColumns {0};
        int mNumRows {0};
        int mLowestVisibleRow {0};
        bool mHasChanged {false};
        mutable CollisionResult mCollisionResult;
    };
}

#endif
