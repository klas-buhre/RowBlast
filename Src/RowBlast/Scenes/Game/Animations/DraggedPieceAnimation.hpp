#ifndef DraggedPieceAnimation_hpp
#define DraggedPieceAnimation_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "IGameLogic.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class DraggedPiece;
    class IGameLogic;
    class GameScene;
    
    class DraggedPieceAnimation {
    public:
        enum class State {
            DraggedPieceGoingUp,
            DraggedPieceGoingBack,
            Inactive
        };
        
        DraggedPieceAnimation(Pht::IEngine& engine,
                              GameScene& scene,
                              IGameLogic& gameLogic,
                              DraggedPiece& draggedPiece);
        
        void Init();
        void StartGoUpAnimation();
        void StartGoBackAnimation(PreviewPieceIndex draggedPieceIndex);
        State Update();
        
        State GetState() const {
            return mState;
        }
        
        float GetYOffsetInCells() const {
            return mYOffset;
        }
        
        static constexpr auto targetYOffsetInCells = 1.5f;
        
    private:
        void UpdateInDraggedPieceGoingUpState();
        void UpdateInDraggedPieceGoingBackState();
        Pht::Vec3 GetPreviewPiecePosition(PreviewPieceIndex pieceIndex) const;
        
        Pht::IEngine& mEngine;
        GameScene& mScene;
        IGameLogic& mGameLogic;
        DraggedPiece& mDraggedPiece;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        Pht::Vec2 mStartPosition;
        Pht::Vec2 mStopPosition;
        float mStartScale {0.0f};
        float mStopScale {0.0f};
        float mYOffset {0.0f};
    };
}

#endif
