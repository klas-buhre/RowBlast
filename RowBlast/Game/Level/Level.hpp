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
        
        Level(int id,
              Objective objective,
              int numColumns,
              int numRows,
              float speed,
              int numMoves,
              const StarLimits& starLimits,
              const std::vector<const Piece*>& pieceTypes,
              const std::string& backgroundTextureFilename,
              bool isDark);
        
        void SetClearGrid(std::unique_ptr<CellGrid> clearGrid) {
            mClearGrid = std::move(clearGrid);
        }

        void SetBlueprintGrid(std::unique_ptr<BlueprintCellGrid> blueprintGrid) {
            mBlueprintGrid = std::move(blueprintGrid);
        }
        
        int GetId() const {
            return mId;
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
        
        bool IsDark() const {
            return mIsDark;
        }
        
        static constexpr int maxNumPieceTypes {11};

    private:
        int mId {0};
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
        bool mIsDark {false};
    };
    
    struct LevelInfo {
        LevelInfo(int id, Level::Objective mObjective, const std::vector<const Piece*>& pieceTypes);
        
        int mId {0};
        Level::Objective mObjective;
        std::vector<const Piece*> mPieceTypes;
    };
}

#endif
