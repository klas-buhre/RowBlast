#include "SlidingTextAnimation.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "TextComponent.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

/*

D = (V0 - Vd) * Ts / 2 + Vd * (Ts + Td / 2) = V0 * Ts / 2 - Vd * Ts / 2 + Vd * Ts + Vd * Td / 2 =
  = V0 * Ts / 2 + Vd * Ts / 2 + Vd * Td / 2
Dd = Vd * Td
Constants:
D
Dd
Ts
Td

Variables:
Vd = Dd / Td
V0 = D * 2 / Ts - Vd - Vd * Td / Ts

*/

namespace {
    constexpr auto slideTime {0.2f};
    constexpr auto displayDistance {0.65f};
    constexpr auto textWidth {8.0f};
    const Pht::Vec3 centerPosition {0.0f, 0.0f, 0.0f};
    constexpr auto alpha {0.88f};
    
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
}

SlidingTextAnimation::SlidingTextAnimation(Pht::IEngine& engine,
                                           GameScene& scene,
                                           const CommonResources& commonResources) :
    mEngine {engine},
    mScene {scene},
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {
    
    auto& font {commonResources.GetHussarFontSize52PotentiallyZoomedScreen()};

    mTexts.reserve(4);
    CreateText(font, 2.5f, {{-3.1f, 1.0f}, "Clear all"}, {{-4.0f, -1.0f}, "gray blocks!"});
    CreateText(font, 2.5f, {{-4.0f, 1.0f}, "You cleared"}, {{-3.55f, -1.0f}, "all blocks!"});
    CreateText(font, 2.5f, {{-2.5f, 1.0f}, "Fill all"}, {{-3.5f, -1.0f}, "gray slots!"});
    CreateText(font, 2.5f, {{-3.3f, 1.0f}, "You filled"}, {{-3.05f, -1.0f}, "all slots!"});
}

void SlidingTextAnimation::CreateText(const Pht::Font& font,
                                      float displayTime,
                                      const TextLine& upperTextLine,
                                      const TextLine& lowerTextLine) {
    Pht::TextProperties textProperties {font, 1.0f, {1.0f, 1.0f, 1.0f, alpha}};
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;

    auto upperTextLineSceneObject {std::make_unique<Pht::SceneObject>()};
    auto upperTextComponent {
        std::make_unique<Pht::TextComponent>(*upperTextLineSceneObject,
                                             upperTextLine.mText,
                                             textProperties)
    };
    upperTextLineSceneObject->SetComponent<Pht::TextComponent>(std::move(upperTextComponent));
    mContainerSceneObject->AddChild(*upperTextLineSceneObject);
    
    auto lowerTextLineSceneObject {std::make_unique<Pht::SceneObject>()};
    auto lowerTextComponent {
        std::make_unique<Pht::TextComponent>(*lowerTextLineSceneObject,
                                             lowerTextLine.mText,
                                             textProperties)
    };
    lowerTextLineSceneObject->SetComponent<Pht::TextComponent>(std::move(lowerTextComponent));
    mContainerSceneObject->AddChild(*lowerTextLineSceneObject);
    
    mTexts.push_back(
        Text {
            displayTime,
            Pht::Vec3{upperTextLine.mPosition.x, upperTextLine.mPosition.y, 0.0f},
            std::move(upperTextLineSceneObject),
            Pht::Vec3{lowerTextLine.mPosition.x, lowerTextLine.mPosition.y, 0.0f},
            std::move(lowerTextLineSceneObject)
        }
    );
}

void SlidingTextAnimation::Init() {
    mScene.GetHudContainer().AddChild(*mContainerSceneObject);

    for (auto& text: mTexts) {
        text.mUpperTextLineSceneObject->SetIsVisible(false);
        text.mLowerTextLineSceneObject->SetIsVisible(false);
    }

    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    mLeftPosition = {-frustumSize.x / 2.0f - textWidth / 2.0f, 0.0f, 0.0f};
    mRightPosition = {frustumSize.x / 2.0f + textWidth / 2.0f, 0.0f, 0.0f};
}

/*
Vd = Dd / Td
V0 = D * 2 / Ts - Vd - Vd * Td / Ts
*/
void SlidingTextAnimation::Start(Message message) {
    mState = State::SlidingIn;
    
    mText = &mTexts[static_cast<int>(message)];
    mText->mUpperTextLineSceneObject->SetIsVisible(true);
    mText->mLowerTextLineSceneObject->SetIsVisible(true);

    mElapsedTime = 0.0f;
    
    mTextPosition = centerPosition;
    
    mDisplayVelocity = displayDistance / mText->mDisplayTime;
    mInitialVelocity = mRightPosition.x * 2.0f / slideTime - mDisplayVelocity -
                       mDisplayVelocity * mText->mDisplayTime / slideTime;
    mVelocity = mInitialVelocity;
    
    UpdateTextLineSceneObjectPositions();
}

SlidingTextAnimation::State SlidingTextAnimation::Update() {
    switch (mState) {
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::DisplayingText:
            UpdateInDisplayingTextState();
            break;
        case State::SlidingOut:
            UpdateInSlidingOutState();
            break;
        case State::Inactive:
            break;
    }
    
    return mState;
}

void SlidingTextAnimation::UpdateInSlidingInState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mTextPosition.x += mVelocity * dt;
    mVelocity -= dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    mElapsedTime += dt;
    
    if (mVelocity < mDisplayVelocity || mTextPosition.x >= mRightPosition.x - displayDistance / 2.0f) {
        mState = State::DisplayingText;
        mElapsedTime = 0.0f;
        mTextPosition.x = mRightPosition.x - displayDistance / 2.0f;
        
        mEngine.GetInput().EnableInput();
    }
    
    UpdateTextLineSceneObjectPositions();
}

void SlidingTextAnimation::UpdateTextLineSceneObjectPositions() {
    auto upperTextLinePosition {mLeftPosition + mTextPosition + mText->mUpperTextLinePosition};
    mText->mUpperTextLineSceneObject->GetTransform().SetPosition(upperTextLinePosition);
    
    auto lowerTextLinePosition {mRightPosition - mTextPosition + mText->mLowerTextLinePosition};
    mText->mLowerTextLineSceneObject->GetTransform().SetPosition(lowerTextLinePosition);
}

void SlidingTextAnimation::UpdateInDisplayingTextState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mTextPosition.x += mDisplayVelocity * dt;
    mElapsedTime += dt;
    
    if (mElapsedTime > mText->mDisplayTime || mEngine.GetInput().ConsumeWholeTouch()) {
        mState = State::SlidingOut;
        mElapsedTime = 0.0f;
        mVelocity = mDisplayVelocity;
    }
    
    UpdateTextLineSceneObjectPositions();
}

void SlidingTextAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mTextPosition.x += mVelocity * dt;
    mVelocity += dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    mElapsedTime += dt;

    UpdateTextLineSceneObjectPositions();

    if (mTextPosition.x >= mRightPosition.x * 2.0f) {
        mState = State::Inactive;

        mText->mUpperTextLineSceneObject->SetIsVisible(false);
        mText->mLowerTextLineSceneObject->SetIsVisible(false);
    }
}
