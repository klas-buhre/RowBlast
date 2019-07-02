#ifndef PreviewPiecesRotationAnimation_hpp
#define PreviewPiecesRotationAnimation_hpp

// Game includes.
#include "GameLogic.hpp"

namespace Pht {
    class SceneObject;
}

namespace RowBlast {
    class GameScene;
    class GameLogic;
    
    class PreviewPiecesRotationAnimation {
    public:
        PreviewPiecesRotationAnimation(GameScene& scene, GameLogic& gameLogic);
        
        void Init();
        void Update(float dt);
        
    private:
        void UpdateInActiveState(float dt);
        float UpdateAngle(float startAngle, Rotation targetRotation, float rotationProgress);
        void SetActivePieceAngle(float angle);
        void SetSelectable0PieceAngle(float angle);
        void SetSelectable1PieceAngle(float angle);
        void HandleNextPieceOrSwitch();
        void UpdateInInactiveState();
        void GoToInactiveState();
        void SetAnglesAccordingTotargets();
        
        struct AnglesDeg {
            float mActive {0.0f};
            float mSelectable0 {0.0f};
            float mSelectable1 {0.0f};
        };
        
        enum class State {
            Active,
            Inactive
        };
        
        GameScene& mScene;
        GameLogic& mGameLogic;
        State mState {State::Inactive};
        PreviewPieceRotations mTargetRotations;
        AnglesDeg mAnglesDeg;
        AnglesDeg mStartAnglesDeg;
        float mElapsedTime {0.0f};
        Pht::SceneObject* mActivePreviewPiece {nullptr};
        Pht::SceneObject* mSelectable0PreviewPiece {nullptr};
        Pht::SceneObject* mSelectable1PreviewPiece {nullptr};
    };
}

#endif
