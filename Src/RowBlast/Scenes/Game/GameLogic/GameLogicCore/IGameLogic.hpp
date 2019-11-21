#ifndef IGameLogic_hpp
#define IGameLogic_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "NextPieceGenerator.hpp"

namespace Pht {
    class TouchEvent;
}

namespace RowBlast {
    class Move;
    class Piece;
    class PreviewPieceRotations;
    
    enum class PreviewPieceIndex {
        Active,
        Selectable0,
        Selectable1,
        None
    };
    
    class IGameLogic {
    public:
        virtual ~IGameLogic() {}
        
        virtual void DropFallingPiece() = 0;
        virtual void SelectMove(const Move& move) = 0;
        virtual void OnFallingPieceAnimationFinished(bool finalMovementWasADrop) = 0;
        virtual void RotatePreviewPiece(PreviewPieceIndex previewPieceIndex) = 0;
        virtual void RotatePreviewPieces() = 0;
        virtual void RotatePiece(const Pht::TouchEvent& touchEvent) = 0;
        virtual void SwitchPiece() = 0;
        virtual void SetFallingPieceXPosWithCollisionDetection(float fallingPieceNewX) = 0;
        virtual int GetGhostPieceRow() const = 0;
        virtual bool IsInFieldExplosionsState() const = 0;
        virtual void StartBlastArea(const Pht::IVec2& position) = 0;
        virtual void StopBlastArea() = 0;
        virtual bool BeginDraggingPiece(PreviewPieceIndex draggedPieceIndex) = 0;
        virtual void OnDraggedPieceMoved() = 0;
        virtual void StopDraggingPiece() = 0;
        virtual void CancelDraggingPiece() = 0;
        virtual void OnDraggedPieceAnimationFinished() = 0;
        virtual void IncreaseScore(int points, const Pht::Vec2& scoreTextPosition) = 0;
        virtual const Piece* GetPieceType() const = 0;
        virtual const TwoPieces& GetSelectablePieces() const = 0;
        virtual const PreviewPieceRotations& GetPreviewPieceRotations() const = 0;
        virtual const PreviewPieceRotations& GetPreviewPieceHudRotations() const = 0;
    };
}

#endif
