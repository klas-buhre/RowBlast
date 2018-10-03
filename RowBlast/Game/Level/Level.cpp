#include "Level.hpp"

using namespace RowBlast;

Level::Level(int id,
             Objective objective,
             int numColumns,
             int numRows,
             float speed,
             int numMoves,
             const StarLimits& starLimits,
             const std::vector<const Piece*>& pieceTypes,
             const std::vector<const Piece*>& pieceSequence,
             const std::vector<TutorialMove>& predeterminedMoves,
             const std::vector<TutorialMove>& suggestedMoves,
             const std::string& backgroundTextureFilename,
             LightIntensity lightIntensity,
             bool isPartOfTutorial) :
    mId {id},
    mObjective {objective},
    mNumColumns {numColumns},
    mNumRows {numRows},
    mSpeed {speed},
    mNumMoves {numMoves},
    mStarLimits {starLimits},
    mPieceTypes {pieceTypes},
    mPieceSequence {pieceSequence},
    mPredeterminedMoves {predeterminedMoves},
    mSuggestedMoves {suggestedMoves},
    mBackgroundTextureFilename {backgroundTextureFilename},
    mLightIntensity {lightIntensity},
    mIsPartOfTutorial {isPartOfTutorial} {}

LevelInfo::LevelInfo(int id,
                     Level::Objective objective,
                     const std::vector<const Piece*>& pieceTypes) :
    mId {id},
    mObjective {objective},
    mPieceTypes {pieceTypes} {}
