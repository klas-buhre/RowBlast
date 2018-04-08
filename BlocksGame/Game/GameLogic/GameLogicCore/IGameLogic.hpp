#ifndef IGameLogic_hpp
#define IGameLogic_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class TouchEvent;
}

namespace BlocksGame {
    class Movement;
    
    class IGameLogic {
    public:
        virtual ~IGameLogic() {}
        
        virtual void DropFallingPiece() = 0;
        virtual void StartFallingPieceAnimation(const Movement& lastMovement) = 0;
        virtual void OnFallingPieceAnimationFinished(bool startParticleEffect) = 0;
        virtual void RotatePieceOrDetonateBomb(const Pht::TouchEvent& touchEvent) = 0;
        virtual void SwitchPiece() = 0;
        virtual void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) = 0;
        virtual int GetGhostPieceRow() const = 0;
        virtual bool IsInFieldExplosionsState() const = 0;
        virtual void StartBlastRadiusAnimation(const Pht::IVec2& position) = 0;
        virtual void StopBlastRadiusAnimation() = 0;
    };
}

#endif
