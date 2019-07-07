#ifndef DraggedPiece_hpp
#define DraggedPiece_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"

namespace RowBlast {
    class Piece;

    class DraggedPiece {
    public:
        enum class State {
            Dragged,
            Landing
        };
        
        void BeginDrag(const Piece& pieceType, Rotation rotation);
        void SetPosition(const Pht::Vec2& position);
        Pht::IVec2 GetFieldGridPosition() const;
        
        const Piece& GetPieceType() const {
            return *mPieceType;
        }
        
        bool IsLanding() const {
            return mState == State::Landing;
        }
        
        const Pht::Vec2& GetRenderablePosition() const {
            return mPosition;
        }
        
        Rotation GetRotation() const {
            return mRotation;
        }
        
    private:
        const Piece* mPieceType {nullptr};
        Pht::Vec2 mPosition;
        Rotation mRotation {Rotation::Deg0};
        State mState {State::Dragged};
    };
}

#endif