#ifndef PreviewPieceGroupAnimation_hpp
#define PreviewPieceGroupAnimation_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "GameHud.hpp"

namespace RowBlast {
    struct NextPreviewPiecesPositionsConfig {
        Pht::Vec3 mLeft;
        Pht::Vec3 mSlot1;
        Pht::Vec3 mSlot2;
        Pht::Vec3 mRight;
    };

    struct SelectablePreviewPiecesPositionsConfig {
        Pht::Vec3 mLeft;
        Pht::Vec3 mSlot1;
        Pht::Vec3 mSlot2;
        Pht::Vec3 mSlot3;
        Pht::Vec3 mRight;
    };

    class PreviewPieceGroupAnimation {
    public:
        enum class Kind {
            NextPiece,
            SwitchDuringNextPiece,
            Switch,
            RemoveActivePiece,
            None
        };
        
        void Init();
        void StartNextPieceAnimation(NextPreviewPieces& previewPieces,
                                     const NextPreviewPiecesPositionsConfig& piecePositionsConfig,
                                     float targetChangedScale);
        void StartSwitchDuringNextPieceAnimation(
            SelectablePreviewPieces& previewPieces,
            const SelectablePreviewPiecesPositionsConfig& piecePositionsConfig);
        void StartSwitchPieceAnimation(
            SelectablePreviewPieces& previewPieces,
            const SelectablePreviewPiecesPositionsConfig& piecePositionsConfig);
        void StartRemoveActivePieceAnimation(
            SelectablePreviewPieces& previewPieces,
            const SelectablePreviewPiecesPositionsConfig& piecePositionsConfig);
        void Update(float normalizedElapsedTime);
        
    private:
        enum class Scaling {
            ScaleUp,
            ScaleDown,
            ScaleChange,
            NoScaling
        };
        
        void UpdateNextPieceAnimation(float slideValue);
        void UpdateSwitchDuringNextPieceAnimation(float slideValue);
        void UpdateSwitchPieceAnimation(float slideValue);
        void UpdateRemoveActivePieceAnimation(float slideValue);
        void AnimatePiece(PreviewPiece& previewPiece,
                          float xStart,
                          float xStop,
                          float slideFunctionValue,
                          Scaling scaling);
        PreviewPiece& GetNextPreviewPiece(int previewPieceIndex);
        PreviewPiece& GetSelectablePreviewPiece(int previewPieceIndex);
        
        Kind mKind {Kind::None};
        NextPreviewPieces* mNextPreviewPieces {nullptr};
        NextPreviewPiecesPositionsConfig mNextPiecePositionsConfig;
        SelectablePreviewPieces* mSelectablePreviewPieces {nullptr};
        SelectablePreviewPiecesPositionsConfig mSelectablePreviewPiecesPositionsConfig;
        float mScaleChangeInitialScale {1.0f};
        float mScaleChangeFinalScale {1.0f};
    };
}

#endif
