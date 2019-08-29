#ifndef FallingPieceAnimation_hpp
#define FallingPieceAnimation_hpp

// Engine includes.
#include "StaticVector.hpp"

// Game includes.
#include "Field.hpp"

namespace RowBlast {
    class FallingPiece;
    class Movement;
    class IGameLogic;
    
    class FallingPieceAnimation {
    public:
        enum class State {
            Active,
            Landing,
            Inactive
        };
        
        FallingPieceAnimation(IGameLogic& gameLogic, FallingPiece& fallingPiece);
    
        void Init();
        void Start(const Movement& lastMovement);
        State Update(float dt);
        
        State GetState() const {
            return mState;
        }
        
    private:
        void RemoveFirstMovementIfDetour();
        void Animate(float dt);
        void NextMovement();
        void LandFallingPiece();
        
        using MovementPtrs =
            Pht::StaticVector<const Movement*, Field::maxNumColumns * Field::maxNumRows * 4>;
        
        IGameLogic& mGameLogic;
        FallingPiece& mFallingPiece;
        State mState {State::Inactive};
        MovementPtrs mMovements;
        int mMovementIndex {0};
        float mSpeed {0.0f};
    };
}

#endif
