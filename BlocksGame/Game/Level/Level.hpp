#ifndef Level_hpp
#define Level_hpp

#include <memory>

// Game includes.
#include "Cell.hpp"
#include "Piece.hpp"
#include "ProgressManager.hpp"

namespace RowBlast {
    class Level {
    public:
        enum class Objective {
            Clear,
            Build
        };
        
        enum class Color {
            Pink,
            Green
        };
        
        Level(int index,
              Objective objective,
              int numColumns,
              int numRows,
              float speed,
              int numMoves,
              const StarLimits& starLimits,
              const std::vector<const Piece*>& pieceTypes,
              Color color);
        
        void SetClearGrid(std::unique_ptr<CellGrid> clearGrid) {
            mClearGrid = std::move(clearGrid);
        }

        void SetBlueprintGrid(std::unique_ptr<BlueprintCellGrid> blueprintGrid) {
            mBlueprintGrid = std::move(blueprintGrid);
        }
        
        int GetIndex() const {
            return mIndex;
        }
        
        Objective GetObjective() const {
            return mObjective;
        }
        
        int GetNumColumns() const {
            return mNumColumns;
        }

        int GetNumRows() const {
            return mNumRows;
        }
        
        float GetSpeed() const {
            return mSpeed;
        }
        
        float GetNumMoves() const {
            return mNumMoves;
        }
        
        const StarLimits& GetStarLimits() const {
            return mStarLimits;
        }
        
        const std::vector<const Piece*>& GetPieceTypes() const {
            return mPieceTypes;
        }
        
        const CellGrid* GetClearGrid() const {
            return mClearGrid.get();
        }

        const BlueprintCellGrid* GetBlueprintGrid() const {
            return mBlueprintGrid.get();
        }
        
        Color GetColor() const {
            return mColor;
        }

    private:
        int mIndex {0};
        Objective mObjective;
        int mNumColumns {0};
        int mNumRows {0};
        float mSpeed {0.0f};
        int mNumMoves {0};
        StarLimits mStarLimits;
        std::vector<const Piece*> mPieceTypes;
        std::unique_ptr<CellGrid> mClearGrid;
        std::unique_ptr<BlueprintCellGrid> mBlueprintGrid;
        Color mColor {Color::Pink};
    };
}

#endif
