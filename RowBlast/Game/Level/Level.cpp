#include "Level.hpp"

using namespace RowBlast;

Level::Level(int index,
             Objective objective,
             int numColumns,
             int numRows,
             float speed,
             int numMoves,
             const StarLimits& starLimits,
             const std::vector<const Piece*>& pieceTypes,
             Color color) :
    mIndex {index},
    mObjective {objective},
    mNumColumns {numColumns},
    mNumRows {numRows},
    mSpeed {speed},
    mNumMoves {numMoves},
    mStarLimits {starLimits},
    mPieceTypes {pieceTypes},
    mColor {color} {}
