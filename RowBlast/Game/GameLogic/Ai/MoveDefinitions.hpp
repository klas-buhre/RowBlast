#ifndef MoveDefinitions_hpp
#define MoveDefinitions_hpp

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

// Game includes.
#include "Field.hpp"
#include "Level.hpp"

namespace RowBlast {
    class Piece;
    
    class Movement {
    public:
        Movement() {}
        Movement(const Pht::Vec2& position, Rotation rotation, const Movement* previous);
        
        const Pht::Vec2& GetPosition() const {
            return mPosition;
        }
        
        Rotation GetRotation() const {
            return mRotation;
        }
        
        const Movement* GetPrevious() const {
            return mPrevious;
        }
        
    private:
        Pht::Vec2 mPosition;
        Rotation mRotation;
        const Movement* mPrevious {nullptr};
    };
    
    class MoveButton;
    
    struct Move {
        bool operator==(const Move& other) const {
            return mPosition == other.mPosition && mRotation == other.mRotation;
        }
        
        Pht::IVec2 mPosition;
        Rotation mRotation;
        const Movement* mLastMovement {nullptr};
        float mScore {0.0f};
        bool mHasBeenPresented {false};
        bool mIsReachable {true};
        bool mIsHidden {false};
        MoveButton* mButton {nullptr};
    };
    
    using Moves = Pht::StaticVector<Move, Field::maxNumColumns * Field::maxNumRows * 4>;
    using Movements = Pht::StaticVector<Movement, Field::maxNumColumns * Field::maxNumRows * 4 * 2>;
    
    struct MovingPiece {
        void RotateClockwise();
        void RotateAntiClockwise();
        bool IsAtTutorialMove(const Level::TutorialMove& tutorialMove) const;
        
        Pht::IVec2 mPosition;
        Rotation mRotation;
        const Piece& mPieceType;
    };
    
    struct ValidMoves {
        void Clear();
        
        Moves mMoves;
        Movements mMovements;
    };
}

#endif
