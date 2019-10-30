#include "Level.hpp"

using namespace RowBlast;

namespace {
    int CalculateNumMovesForGestureControls(int numMovesForClickControls, float gestureMovesFactor) {
        return static_cast<int>(std::ceil(numMovesForClickControls * gestureMovesFactor));
    }
}

Level::Level(int id,
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
             bool isPartOfTutorial) :
    mId {id},
    mObjective {objective},
    mNumColumns {numColumns},
    mNumRows {numRows},
    mSpeed {speed},
    mNumMovesClickControls {numMoves},
    mNumMovesGestureControls {CalculateNumMovesForGestureControls(numMoves, gestureMovesFactor)},
    mStarLimitsClickControls {starLimits},
    mStarLimitsGestureControls {
        CalculateNumMovesForGestureControls(starLimits.mTwo, gestureMovesFactor),
        CalculateNumMovesForGestureControls(starLimits.mThree, gestureMovesFactor)
    },
    mPieceTypes {pieceTypes},
    mPieceSequence {pieceSequence},
    mPredeterminedMoves {predeterminedMoves},
    mSuggestedMoves {suggestedMoves},
    mMusicTrack {musicTrack},
    mBackgroundTextureFilename {backgroundTextureFilename},
    mFloatingBlocksSet {floatingBlocksSet},
    mLightIntensity {lightIntensity},
    mIsPartOfTutorial {isPartOfTutorial} {}

int Level::GetNumMoves(ControlType controlType) const {
    return mNumMovesClickControls;
#if 0
    switch (controlType) {
        case ControlType::Click:
            return mNumMovesClickControls;
        case ControlType::Gesture:
            return mNumMovesGestureControls;
    }
#endif
}

const StarLimits& Level::GetStarLimits(ControlType controlType) const {
    switch (controlType) {
        case ControlType::Click:
            return mStarLimitsClickControls;
        case ControlType::Gesture:
            return mStarLimitsGestureControls;
    }
}

LevelInfo::LevelInfo(int id,
                     Level::Objective objective,
                     const std::vector<const Piece*>& pieceTypes) :
    mId {id},
    mObjective {objective},
    mPieceTypes {pieceTypes} {}
