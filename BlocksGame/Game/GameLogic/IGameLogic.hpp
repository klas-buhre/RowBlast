#ifndef IGameLogic_hpp
#define IGameLogic_hpp

namespace BlocksGame {
    class Movement;
    
    class IGameLogic {
    public:
        virtual ~IGameLogic() {}
        
        virtual void DropFallingPiece() = 0;
        virtual void StartFallingPieceAnimation(const Movement& lastMovement) = 0;
        virtual void OnFallingPieceAnimationFinished(bool startParticleEffect) = 0;
        virtual void RotatePieceOrDetonateBomb() = 0;
        virtual void SwitchPiece() = 0;
        virtual void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) = 0;
        virtual int GetGhostPieceRow() const = 0;
    };
}

#endif
