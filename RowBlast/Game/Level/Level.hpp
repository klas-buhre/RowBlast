#ifndef Level_hpp
#define Level_hpp

#include <memory>
#include <string>

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
        
        Level(int index,
              Objective objective,
              int numColumns,
              int numRows,
              float speed,
              int numMoves,
              const StarLimits& starLimits,
              const std::vector<const Piece*>& pieceTypes,
              const std::string& backgroundTextureFilename);
        
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
        
        const std::string& GetBackgroundTextureFilename() const {
            return mBackgroundTextureFilename;
        }
        
        static constexpr int maxNumPieceTypes {11};

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
        std::string mBackgroundTextureFilename;
    };
    
    struct LevelInfo {
        LevelInfo(int index,
                  Level::Objective mObjective,
                  const std::vector<const Piece*>& pieceTypes);
        
        int mIndex {0};
        Level::Objective mObjective;
        std::vector<const Piece*> mPieceTypes;
    };
}

#endif
