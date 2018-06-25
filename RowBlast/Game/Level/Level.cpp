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
             const std::string& backgroundTextureFilename) :
    mId {id},
    mObjective {objective},
    mNumColumns {numColumns},
    mNumRows {numRows},
    mSpeed {speed},
    mNumMoves {numMoves},
    mStarLimits {starLimits},
    mPieceTypes {pieceTypes},
    mBackgroundTextureFilename {backgroundTextureFilename} {}

LevelInfo::LevelInfo(int id,
                     Level::Objective objective,
                     const std::vector<const Piece*>& pieceTypes) :
    mId {id},
    mObjective {objective},
    mPieceTypes {pieceTypes} {}
