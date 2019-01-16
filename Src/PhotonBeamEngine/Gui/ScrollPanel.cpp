#include "ScrollPanel.hpp"

#include "IEngine.hpp"
#include "InputEvent.hpp"
#include "MathUtils.hpp"

using namespace Pht;

ScrollPanel::ScrollPanel(IEngine& engine, float dampingCoefficient, float cutoffVelocity) :
    mEngine {engine},
    mDampingCoefficient {dampingCoefficient},
    mCutoffVelocity {cutoffVelocity},
    mRoot {std::make_unique<Pht::SceneObject>()},
    mPanel {std::make_unique<Pht::SceneObject>()} {
    
    mRoot->AddChild(*mPanel);
}

void ScrollPanel::SetPosition(const Vec3& position) {
    mRoot->GetTransform().SetPosition(position);
}

void ScrollPanel::AddSceneObject(SceneObject& sceneObject) {
    mPanel->AddChild(sceneObject);
}

void ScrollPanel::OnTouch(const TouchEvent& touch) {
    UpdateIsTouchingState(touch);

    switch (touch.mState) {
        case Pht::TouchState::Begin:
            mTouchLocationAtScrollBegin = touch.mLocation;
            mPanelYPositionAtScrollBegin = mPanel->GetTransform().GetPosition().y;
            break;
        case Pht::TouchState::Ongoing: {
            auto translation = touch.mLocation - mTouchLocationAtScrollBegin;
            auto newPanelYPosition {mPanelYPositionAtScrollBegin - translation.y * 0.047f};
            
            mPanelYVelocity = (newPanelYPosition - mPanel->GetTransform().GetPosition().y) /
                               mEngine.GetLastFrameSeconds();
            
            SetYPosition(newPanelYPosition);
            break;
        }
        default:
            break;
    }
}

void ScrollPanel::UpdateIsTouchingState(const Pht::TouchEvent& touch) {
    switch (touch.mState) {
        case Pht::TouchState::Begin:
        case Pht::TouchState::Ongoing:
            mIsTouching = true;
            break;
        default:
            mIsTouching = false;
            break;
    }
}

void ScrollPanel::Update() {
    if (mIsTouching || mPanelYVelocity == 0.0f) {
        return;
    }
 
    auto panelYPosition {mPanel->GetTransform().GetPosition().y};
    auto deacceleration {mDampingCoefficient * mPanelYVelocity};
    auto dt {mEngine.GetLastFrameSeconds()};
    auto previousVelocity {mPanelYVelocity};
    mPanelYVelocity -= deacceleration * dt;
 
    if (std::fabs(mPanelYVelocity) < mCutoffVelocity ||
        Pht::Sign(mPanelYVelocity) != Pht::Sign(previousVelocity)) {

        mPanelYVelocity = 0.0f;
    }

    panelYPosition += mPanelYVelocity * dt;
    SetYPosition(panelYPosition);
}

void ScrollPanel::SetYPosition(float yPosition) {
    Pht::Vec3 position {0.0f, yPosition, 0.0f};
    mPanel->GetTransform().SetPosition(position);
}
