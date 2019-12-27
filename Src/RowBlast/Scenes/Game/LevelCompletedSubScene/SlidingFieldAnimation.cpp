#include "SlidingFieldAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "RenderPass.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace RowBlast;

namespace {
    constexpr auto slideTime = 0.4f;
    constexpr auto padding = 1.0f;
    constexpr auto upperHudYSize = 3.0f;
    constexpr auto lowerHudYSize = 4.0f;
}

SlidingFieldAnimation::SlidingFieldAnimation(Pht::IEngine& engine, GameScene& scene) :
    mEngine {engine},
    mScene {scene} {}

void SlidingFieldAnimation::Start() {
    mState = State::Ongoing;
    mElapsedTime = 0.0f;
    
    auto& orthographicFrustumSize = mEngine.GetRenderer().GetOrthographicFrustumSize();
    auto& fieldSceneObject = mScene.GetFieldQuadContainer();
    
    mFieldInitialPosition = fieldSceneObject.GetTransform().GetPosition();
    mFieldFinalPosition = Pht::Vec3 {
        orthographicFrustumSize.x / 2.0f + mScene.GetFieldWidth() / 2.0f + padding,
        mFieldInitialPosition.y,
        mFieldInitialPosition.z
    };
    
    auto& fieldContainerPosition = mScene.GetFieldContainer().GetTransform().GetPosition();
    mFieldContainerRelativePosition = fieldContainerPosition - mFieldInitialPosition;
    
    auto& scissorBoxPosition = mScene.GetFieldScissorBox().mLowerLeft;
    mScissorBoxRelativePosition = Pht::Vec2 {
        scissorBoxPosition.x - mFieldInitialPosition.x,
        scissorBoxPosition.y - mFieldInitialPosition.y
    };
    
    mUpperHudInitialYPosition = 0.0f;
    mUpperHudFinalYPosition = upperHudYSize + padding;
    mLowerHudInitialYPosition = 0.0f;
    mLowerHudFinalYPosition = -lowerHudYSize - padding;
}

SlidingFieldAnimation::State SlidingFieldAnimation::Update() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    UpdateField();
    UpdateHud();
    
    if (mElapsedTime >= slideTime) {
        mState = State::Inactive;
        mScene.GetFieldQuadContainer().SetIsVisible(false);
    }
    
    return mState;
}

void SlidingFieldAnimation::UpdateField() {
    auto& fieldQuadTransform = mScene.GetFieldQuadContainer().GetTransform();
    auto fieldQuadPosition = fieldQuadTransform.GetPosition();
    auto distance = mFieldFinalPosition.x - mFieldInitialPosition.x;
    auto normalizedTime = mElapsedTime / slideTime;
    
    fieldQuadPosition.x = mFieldInitialPosition.x +
                          distance * normalizedTime * normalizedTime * normalizedTime;
    fieldQuadTransform.SetPosition(fieldQuadPosition);
    
    auto& fieldContainerTransform = mScene.GetFieldContainer().GetTransform();
    auto fieldContainerPosition = fieldContainerTransform.GetPosition();
    fieldContainerPosition.x = fieldQuadPosition.x + mFieldContainerRelativePosition.x;
    fieldContainerTransform.SetPosition(fieldContainerPosition);
    
    auto scissorBox = mScene.GetFieldScissorBox();
    scissorBox.mLowerLeft.x = fieldQuadPosition.x + mScissorBoxRelativePosition.x;
    mScene.SetScissorBox(scissorBox);
}

void SlidingFieldAnimation::UpdateHud() {
    auto normalizedTime = mElapsedTime / slideTime;
    auto& hud = mScene.GetHud();

    auto upperDistance = mUpperHudFinalYPosition - mUpperHudInitialYPosition;

    auto upperHudYPosition =
        mUpperHudInitialYPosition + upperDistance * normalizedTime * normalizedTime * normalizedTime;

    auto& upperHudTransform = hud.GetUpperContainer().GetTransform();
    auto upperHudPosition = upperHudTransform.GetPosition();
    upperHudPosition.y = upperHudYPosition;
    upperHudTransform.SetPosition(upperHudPosition);

    auto lowerDistance = mLowerHudInitialYPosition - mLowerHudFinalYPosition;

    auto lowerHudYPosition =
        mLowerHudInitialYPosition - lowerDistance * normalizedTime * normalizedTime * normalizedTime;

    auto& lowerHudTransform = hud.GetLowerContainer().GetTransform();
    auto lowerHudPosition = lowerHudTransform.GetPosition();
    lowerHudPosition.y = lowerHudYPosition;
    lowerHudTransform.SetPosition(lowerHudPosition);
}
