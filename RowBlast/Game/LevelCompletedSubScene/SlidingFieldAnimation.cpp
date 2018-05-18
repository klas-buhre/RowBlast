#include "SlidingFieldAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "RenderPass.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto slideTime {0.4f};
    constexpr auto padding {1.0f};
    constexpr auto hudYSize {3.0f};
}

SlidingFieldAnimation::SlidingFieldAnimation(Pht::IEngine& engine, GameScene& scene) :
    mEngine {engine},
    mScene {scene} {}

void SlidingFieldAnimation::Start() {
    mState = State::Ongoing;
    mElapsedTime = 0.0f;
    
    auto& orthographicFrustumSize {mEngine.GetRenderer().GetOrthographicFrustumSize()};
    auto& fieldSceneObject {mScene.GetFieldQuadSceneObject()};
    
    mFieldInitialPosition = fieldSceneObject.GetTransform().GetPosition();
    mFieldFinalPosition = Pht::Vec3 {
        orthographicFrustumSize.x / 2.0f + mScene.GetFieldWidth() / 2.0f + padding,
        mFieldInitialPosition.y,
        mFieldInitialPosition.z
    };
    
    auto& scissorBoxPosition {mScene.GetFieldScissorBox().mLowerLeft};
    mScissorBoxRelativePosition = Pht::Vec2 {
        scissorBoxPosition.x - mFieldInitialPosition.x,
        scissorBoxPosition.y - mFieldInitialPosition.y
    };
    
    auto& hud {mScene.GetHud()};
    auto& hudFrustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    
    mUpperHudInitialYPosition = hud.GetProgressContainer().GetTransform().GetPosition().y;
    mUpperHudFinalYPosition = hudFrustumSize.y / 2.0f + hudYSize + padding;
    mLowerHudInitialYPosition = hud.GetNextPiecesContainer().GetTransform().GetPosition().y;
    mLowerHudFinalYPosition = -hudFrustumSize.y / 2.0f - hudYSize - padding;
}

SlidingFieldAnimation::State SlidingFieldAnimation::Update() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    UpdateField();
    UpdateHud();
    
    if (mElapsedTime >= slideTime) {
        mState = State::Inactive;
    }
    
    return mState;
}

void SlidingFieldAnimation::UpdateField() {
    auto& transform {mScene.GetFieldQuadSceneObject().GetTransform()};
    auto position {transform.GetPosition()};
    auto distance = mFieldFinalPosition.x - mFieldInitialPosition.x;
    auto normalizedTime {mElapsedTime / slideTime};
    
    position.x = mFieldInitialPosition.x +
                 distance * normalizedTime * normalizedTime * normalizedTime;
    transform.SetPosition(position);
    mScene.GetFieldScissorBox().mLowerLeft.x = position.x + mScissorBoxRelativePosition.x;
}

void SlidingFieldAnimation::UpdateHud() {
    auto normalizedTime {mElapsedTime / slideTime};
    auto& hud {mScene.GetHud()};

    auto upperDistance = mUpperHudFinalYPosition - mUpperHudInitialYPosition;

    auto upperHudYPosition {
        mUpperHudInitialYPosition + upperDistance * normalizedTime * normalizedTime * normalizedTime
    };
    
    auto& progressTransform {hud.GetProgressContainer().GetTransform()};
    auto progressPosition {progressTransform.GetPosition()};
    progressPosition.y = upperHudYPosition;
    progressTransform.SetPosition(progressPosition);

    auto& movesTransform {hud.GetMovesContainer().GetTransform()};
    auto movesPosition {movesTransform.GetPosition()};
    movesPosition.y = upperHudYPosition;
    movesTransform.SetPosition(movesPosition);

    auto lowerDistance = mLowerHudInitialYPosition - mLowerHudFinalYPosition;

    auto lowerHudYPosition {
        mLowerHudInitialYPosition - lowerDistance * normalizedTime * normalizedTime * normalizedTime
    };
    
    auto& nextTransform {hud.GetNextPiecesContainer().GetTransform()};
    auto nextPosition {nextTransform.GetPosition()};
    nextPosition.y = lowerHudYPosition;
    nextTransform.SetPosition(nextPosition);

    auto& selectablesTransform {hud.GetSelectablePiecesContainer().GetTransform()};
    auto selectablesPosition {selectablesTransform.GetPosition()};
    selectablesPosition.y = lowerHudYPosition;
    selectablesTransform.SetPosition(selectablesPosition);
}
