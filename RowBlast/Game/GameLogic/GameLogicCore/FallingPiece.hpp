#ifndef FallingPiece_hpp
#define FallingPiece_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"

namespace RowBlast {
    class Piece;

    class FallingPiece {
    public:
        enum class State {
            Falling,
            BeingDraggedDown,
            Landing
        };
        
        void Spawn(const Piece& pieceType, const Pht::Vec2& position, float speed);
        void ResetBetweenGames();
        void SetX(float x);
        void SetY(float y);
        void SetRotation(Rotation rotation);
        void IncreaseTimeSinceMovement(float dt);
        void IncreaseTimeSinceTouchdown(float dt);
        void UpdateTime(float dt);
        void GoToFallingState(float speed);
        void GoToBeingDraggedDownState();
        void GoToLandingState(int yPosition);
        Pht::IVec2 GetIntPosition() const;
        Pht::Vec2 GetRenderablePosition() const;
        Pht::Vec2 GetRenderablePositionSmoothY() const;
        
        const Piece& GetPieceType() const {
            return *mPieceType;
        }
        
        bool IsBeingDraggedDown() const {
            return mState == State::BeingDraggedDown;
        }
        
        bool IsLanding() const {
            return mState == State::Landing;
        }
        
        const Pht::Vec2& GetPosition() const {
            return mPosition;
        }
        
        float GetSpeed() const {
            return mSpeed;
        }
        
        State GetState() const {
            return mState;
        }
        
        State GetPreviousState() const {
            return mPreviousState;
        }
        
        int GetId() const {
            return mId;
        }
        
        Rotation GetRotation() const {
            return mRotation;
        }
        
        float GetPreviousYStep() const {
            return mPreviousYStep;
        }
        
        int GetPreviousIntY() const {
            return mPreviousIntY;
        }
        
        float GetTimeSinceMovement() const {
            return mTimeSinceMovement;
        }
        
        float GetTimeSinceTouchdown() const {
            return mTimeSinceTouchdown;
        }
        
        float GetTimeInDraggedDownState() const {
            return mTimeInDraggedDownState;
        }
        
    private:
        const Piece* mPieceType {nullptr};
        Pht::Vec2 mPosition;
        Rotation mRotation {Rotation::Deg0};
        State mState {State::Falling};
        State mPreviousState {State::Falling};
        int mId {0};
        float mSpeed {0.0f};
        float mPreviousYStep {0.0f};
        int mPreviousIntY {0};
        float mTimeSinceMovement {0.0f};
        float mTimeSinceTouchdown {0.0f};
        float mTimeInDraggedDownState {0.0f};
    };
}

#endif
