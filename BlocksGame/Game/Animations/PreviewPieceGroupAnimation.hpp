#ifndef PreviewPieceGroupAnimation_hpp
#define PreviewPieceGroupAnimation_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "GameHud.hpp"

namespace BlocksGame {
    struct PreviewPiecePositionsConfig {
        Pht::Vec3 mLeft;
        Pht::Vec3 mSlot1;
        Pht::Vec3 mSlot2;
        Pht::Vec3 mRight;
    };
    
    class PreviewPieceGroupAnimation {
    public:
        enum class Kind {
            NextPiece,
            SwitchDuringNextPiece,
            Switch,
            None
        };
        
        void Start(Kind kind,
                   ThreePreviewPieces& previewPieces,
                   const PreviewPiecePositionsConfig& piecePositionsConfig);
        void Update(float normalizedElapsedTime);
        
    private:
        enum class Scaling {
            ScaleUp,
            ScaleDown,
            NoScaling
        };
        
        void UpdateNextPieceAnimation(float slideValue);
        void UpdateSwitchDuringNextPieceAnimation(float slideValue);
        void UpdateSwitchPieceAnimation(float slideValue);
        void AnimatePiece(int previewPieceIndex,
                          float xStart,
                          float xStop,
                          float slideFunctionValue,
                          Scaling scaling);
        PreviewPiece& GetPreviewPiece(int previewPieceIndex);
        Pht::SceneObject& GetSceneObject(int previewPieceIndex);
        
        Kind mKind {Kind::None};
        PreviewPiecePositionsConfig mPiecePositionsConfig;
        ThreePreviewPieces* mPreviewPieces {nullptr};
    };
}

#endif
