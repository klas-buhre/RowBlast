#include "PreviewPiecesRotationAnimation.hpp"

// Engine includes.
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 0.215f;

    Pht::StaticVector<Pht::Vec2, 20> slidePoints {
        {0.0f, 0.0f},
        {0.1f, 0.005f},
        {0.2f, 0.01f},
        {0.3f, 0.02f},
        {0.35f, 0.035f},
        {0.4f, 0.05f},
        {0.45f, 0.065f},
        {0.5f, 0.08f},
        {0.55f, 0.115f},
        {0.6f, 0.15f},
        {0.65f, 0.225f},
        {0.7f, 0.3f},
        {0.75f, 0.41f},
        {0.8f, 0.52f},
        {0.85f, 0.62f},
        {0.9f, 0.7f},
        {0.95f, 0.87f},
        {1.0f, 1.0f},
    };
    
    class ResetPreviewPieceAnimationToStartGuard {
    public:
        ResetPreviewPieceAnimationToStartGuard(GameLogic& gameLogic) :
            mGameLogic {gameLogic} {}
        
        ~ResetPreviewPieceAnimationToStartGuard() {
            mGameLogic.ResetPreviewPieceAnimationToStart();
        }
        
    private:
        GameLogic& mGameLogic;
    };
    
    void SetPreviewPieceAngle(Pht::SceneObject& sceneObject, const Piece* pieceType, float angle) {
        if (pieceType) {
            if (!pieceType->CanRotateAroundZ()) {
                angle = 0.0f;
            }
        }
        
        sceneObject.GetTransform().SetRotation({0.0f, 0.0f, angle});
    }
    
    bool ShouldResetRotation(const Piece* pieceType, Rotation rotation) {
        if (pieceType == nullptr) {
            return false;
        }
        
        return pieceType->GetNumRotations() == 2 && rotation == Rotation::Deg180;
    }
}

PreviewPiecesRotationAnimation::PreviewPiecesRotationAnimation(GameScene& scene,
                                                               GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}

void PreviewPiecesRotationAnimation::Init() {
    mActivePreviewPiece = nullptr;
    mSelectable0PreviewPiece = nullptr;
    mSelectable1PreviewPiece = nullptr;

    mTargetRotations = mGameLogic.GetPreviewPieceHudRotations();
    GoToInactiveState();
}

void PreviewPiecesRotationAnimation::Update(float dt) {
    {
        ResetPreviewPieceAnimationToStartGuard guard {mGameLogic};
        mGameLogic.GetPreviewPieceAnimationToStart();
        
        return;

#if 0
        auto previewPieceAnimationToStart = mGameLogic.GetPreviewPieceAnimationToStart();
        switch (previewPieceAnimationToStart) {
            case PreviewPieceAnimationToStart::NextPieceAndSwitch:
            case PreviewPieceAnimationToStart::SwitchPiece:
                HandleNextPieceOrSwitch();
                break;
            default:
                break;
        }
#endif
    }

#if 0
    auto& rotations = mGameLogic.GetPreviewPieceHudRotations();
    if (rotations != mTargetRotations) {
        mTargetRotations = rotations;
        mStartAnglesDeg = mAnglesDeg;
        mState = State::Active;
        mElapsedTime = 0.0f;
    }
    
    switch (mState) {
        case State::Active:
            UpdateInActiveState(dt);
            break;
        case State::Inactive:
            UpdateInInactiveState();
            break;
    }
    
    auto& hud = mScene.GetHud();
    mActivePreviewPiece = hud.GetActivePreviewPieceSceneObject();
    mSelectable0PreviewPiece = hud.GetSelectable0PreviewPieceSceneObject();
    mSelectable1PreviewPiece = hud.GetSelectable1PreviewPieceSceneObject();
#endif
}

void PreviewPiecesRotationAnimation::UpdateInActiveState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime >= animationDuration) {
        GoToInactiveState();
        return;
    }

    auto normalizedElapsedTime = mElapsedTime / animationDuration;
    auto reversedTime = 1.0f - normalizedElapsedTime;
    auto progress = 1.0f - Pht::Lerp(reversedTime, slidePoints);
    
    SetActivePieceAngle(UpdateAngle(mStartAnglesDeg.mActive, mTargetRotations.mActive, progress));
    SetSelectable0PieceAngle(UpdateAngle(mStartAnglesDeg.mSelectable0,
                                         mTargetRotations.mSelectable0,
                                         progress));
    SetSelectable1PieceAngle(UpdateAngle(mStartAnglesDeg.mSelectable1,
                                         mTargetRotations.mSelectable1,
                                         progress));
}

float PreviewPiecesRotationAnimation::UpdateAngle(float startAngle,
                                                  Rotation targetRotation,
                                                  float rotationProgress) {
    auto targetAngle = RotationToDeg(targetRotation);
    if (startAngle - targetAngle < -180.0f) {
        targetAngle -= 360.0f;
    }
    
    auto diff = targetAngle - startAngle;
    auto angle = startAngle + diff * rotationProgress;
    if (angle <= -360.0f) {
        return angle + 360.0f;
    }
    
    return angle;
}

void PreviewPiecesRotationAnimation::SetActivePieceAngle(float angle) {
    auto* activePreviewPieceSceneObject = mScene.GetHud().GetActivePreviewPieceSceneObject();
    if (activePreviewPieceSceneObject) {
        SetPreviewPieceAngle(*activePreviewPieceSceneObject, mGameLogic.GetPieceType(), angle);
    }
        
    mAnglesDeg.mActive = angle;
}

void PreviewPiecesRotationAnimation::SetSelectable0PieceAngle(float angle) {
    auto* selectable0PieceSceneObject = mScene.GetHud().GetSelectable0PreviewPieceSceneObject();
    if (selectable0PieceSceneObject) {
        SetPreviewPieceAngle(*selectable0PieceSceneObject,
                             mGameLogic.GetSelectablePieces()[0],
                             angle);
    }

    mAnglesDeg.mSelectable0 = angle;
}

void PreviewPiecesRotationAnimation::SetSelectable1PieceAngle(float angle) {
    auto* selectable1PieceSceneObject = mScene.GetHud().GetSelectable1PreviewPieceSceneObject();
    if (selectable1PieceSceneObject) {
        SetPreviewPieceAngle(*selectable1PieceSceneObject,
                             mGameLogic.GetSelectablePieces()[1],
                             angle);
    }

    mAnglesDeg.mSelectable1 = angle;
}

void PreviewPiecesRotationAnimation::HandleNextPieceOrSwitch() {
    if (ShouldResetRotation(mGameLogic.GetPieceType(), mTargetRotations.mActive)) {
        SetActivePieceAngle(0.0f);
    }
    
    if (ShouldResetRotation(mGameLogic.GetSelectablePieces()[0], mTargetRotations.mSelectable0)) {
        SetSelectable0PieceAngle(0.0f);
    }

    if (ShouldResetRotation(mGameLogic.GetSelectablePieces()[1], mTargetRotations.mSelectable1)) {
        SetSelectable1PieceAngle(0.0f);
    }
}

void PreviewPiecesRotationAnimation::UpdateInInactiveState() {
    auto& hud = mScene.GetHud();
    if (mActivePreviewPiece != hud.GetActivePreviewPieceSceneObject() ||
        mSelectable0PreviewPiece != hud.GetSelectable0PreviewPieceSceneObject() ||
        mSelectable1PreviewPiece != hud.GetSelectable1PreviewPieceSceneObject()) {
        
        // At least one preview piece has moved to another scene object. Need to set the correct
        // angles on the new scene objects.
        SetAnglesAccordingTotargets();
    }
}

void PreviewPiecesRotationAnimation::GoToInactiveState() {
    mState = State::Inactive;
    mElapsedTime = 0.0f;
    SetAnglesAccordingTotargets();
    mStartAnglesDeg = mAnglesDeg;
}

void PreviewPiecesRotationAnimation::SetAnglesAccordingTotargets() {
    SetActivePieceAngle(RotationToDeg(mTargetRotations.mActive));
    SetSelectable0PieceAngle(RotationToDeg(mTargetRotations.mSelectable0));
    SetSelectable1PieceAngle(RotationToDeg(mTargetRotations.mSelectable1));
}
