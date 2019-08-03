#ifndef IGameLogic_hpp
#define IGameLogic_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class TouchEvent;
}

namespace RowBlast {
    class Move;
    
    class IGameLogic {
    public:
        virtual ~IGameLogic() {}
        
        virtual void DropFallingPiece() = 0;
        virtual void SelectMove(const Move& move) = 0;
        virtual void OnFallingPieceAnimationFinished(bool finalMovementWasADrop) = 0;
        virtual void RotatePreviewPieces() = 0;
        virtual void RotatePiece(const Pht::TouchEvent& touchEvent) = 0;
        virtual void SwitchPiece() = 0;
        virtual void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) = 0;
        virtual int GetGhostPieceRow() const = 0;
        virtual bool IsInFieldExplosionsState() const = 0;
        virtual void StartBlastRadiusAnimation(const Pht::IVec2& position) = 0;
        virtual void StopBlastRadiusAnimation() = 0;
        virtual void ShowDraggedPiece() = 0;
    };
}

#endif
