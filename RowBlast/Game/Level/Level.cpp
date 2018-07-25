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
             const std::vector<PredeterminedMove>& predeterminedMoves,
             const std::string& backgroundTextureFilename,
             bool isDark,
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
    mBackgroundTextureFilename {backgroundTextureFilename},
    mIsDark {isDark},
    mIsPartOfTutorial {isPartOfTutorial} {}

LevelInfo::LevelInfo(int id,
                     Level::Objective objective,
                     const std::vector<const Piece*>& pieceTypes) :
    mId {id},
    mObjective {objective},
    mPieceTypes {pieceTypes} {}
