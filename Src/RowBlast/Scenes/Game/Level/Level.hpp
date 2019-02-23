#ifndef Level_hpp
#define Level_hpp

#include <memory>
#include <string>

// Engine includes.
#include "Optional.hpp"

// Game includes.
#include "Cell.hpp"
#include "Piece.hpp"
#include "ProgressService.hpp"
#include "SettingsService.hpp"

namespace RowBlast {
    class Level {
    public:
        enum class Objective {
            Clear,
            Build,
            BringDownTheAsteroid
        };
        
        enum class LightIntensity {
            Daylight,
            Sunset,
            Dark
        };
        
        enum class FloatingBlocksSet {
            Standard,
            Asteroid
        };
        
        struct TutorialMove {
            Pht::IVec2 mPosition;
            Rotation mRotation;
            const Piece& mPieceType;
            Pht::Optional<float> mScore;
        };
        
        Level(int id,
              Objective objective,
              int numColumns,
              int numRows,
              float speed,
              int numMoves,
              float gestureMovesFactor,
              const StarLimits& starLimits,
              const std::vector<const Piece*>& pieceTypes,
              const std::vector<const Piece*>& pieceSequence,
              const std::vector<TutorialMove>& predeterminedMoves,
              const std::vector<std::vector<TutorialMove>>& suggestedMoves,
              int musicTrack,
              const std::string& backgroundTextureFilename,
              FloatingBlocksSet floatingBlocksSet,
              LightIntensity lightIntensity,
              bool isPartOfTutorial);
        
        int GetNumMoves(ControlType controlType) const;
        const StarLimits& GetStarLimits(ControlType controlType) const;
        
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
        
        const std::vector<const Piece*>& GetPieceTypes() const {
            return mPieceTypes;
        }

        const std::vector<const Piece*>& GetPieceSequence() const {
            return mPieceSequence;
        }
        
        const std::vector<TutorialMove>& GetPredeterminedMoves() const {
            return mPredeterminedMoves;
        }

        const std::vector<std::vector<TutorialMove>>& GetSuggestedMoves() const {
            return mSuggestedMoves;
        }

        const CellGrid* GetClearGrid() const {
            return mClearGrid.get();
        }

        const BlueprintCellGrid* GetBlueprintGrid() const {
            return mBlueprintGrid.get();
        }
        
        int GetMusicTrack() const {
            return mMusicTrack;
        }
        
        const std::string& GetBackgroundTextureFilename() const {
            return mBackgroundTextureFilename;
        }
        
        FloatingBlocksSet GetFloatingBlocksSet() const {
            return mFloatingBlocksSet;
        }
        
        LightIntensity GetLightIntensity() const {
            return mLightIntensity;
        }

        bool IsPartOfTutorial() const {
            return mIsPartOfTutorial;
        }

        static constexpr int maxNumPieceTypes {11};

    private:
        int mId {0};
        Objective mObjective;
        int mNumColumns {0};
        int mNumRows {0};
        float mSpeed {0.0f};
        int mNumMovesClickControls {0};
        int mNumMovesGestureControls {0};
        StarLimits mStarLimitsClickControls;
        StarLimits mStarLimitsGestureControls;
        std::vector<const Piece*> mPieceTypes;
        std::vector<const Piece*> mPieceSequence;
        std::vector<TutorialMove> mPredeterminedMoves;
        std::vector<std::vector<TutorialMove>> mSuggestedMoves;
        std::unique_ptr<CellGrid> mClearGrid;
        std::unique_ptr<BlueprintCellGrid> mBlueprintGrid;
        int mMusicTrack {1};
        std::string mBackgroundTextureFilename;
        FloatingBlocksSet mFloatingBlocksSet {FloatingBlocksSet::Standard};
        LightIntensity mLightIntensity {LightIntensity::Daylight};
        bool mIsPartOfTutorial {false};
    };
    
    struct LevelInfo {
        LevelInfo(int id, Level::Objective mObjective, const std::vector<const Piece*>& pieceTypes);
        
        int mId {0};
        Level::Objective mObjective;
        std::vector<const Piece*> mPieceTypes;
    };
}

#endif
