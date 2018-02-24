#include "SlidingTextAnimation.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "GameScene.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto slideTime {0.5f};
    const Pht::Vec3 centerPosition {0.0f, 0.0f, 0.0f};
    constexpr auto textHeight {3.0f};
    constexpr auto alpha {0.88f};
}

SlidingTextAnimation::SlidingTextAnimation(Pht::IEngine& engine, GameScene& scene) :
    mEngine {engine},
    mScene {scene},
    mFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(52)},
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {

    auto& frustumSize {engine.GetRenderer().GetHudFrustumSize()};
    mSlideInStartPosition = {0.0f, -frustumSize.y / 4.0f - textHeight / 2.0f, 0.0f};
    mSlideOutFinalPosition = {0.0f, frustumSize.y / 4.0f + textHeight / 2.0f, 0.0f};
    
    mTexts.reserve(4);
    CreateText(2.7f, {{{-3.1f, 1.0f}, "Clear all"}, {{-4.0f, -1.0f}, "gray blocks!"}});
    CreateText(2.7f, {{{-4.0f, 1.0f}, "You cleared"}, {{-3.55f, -1.0f}, "all blocks!"}});
    CreateText(2.7f, {{{-2.5f, 1.0f}, "Fill all"}, {{-3.5f, -1.0f}, "gray slots!"}});
    CreateText(2.7f, {{{-3.3f, 1.0f}, "You filled"}, {{-3.05f, -1.0f}, "all slots!"}});
}

void SlidingTextAnimation::CreateText(float displayTime, const std::vector<TextLine>& textLines) {
    auto sceneObject {std::make_unique<Pht::SceneObject>()};
    std::vector<std::unique_ptr<Pht::SceneObject>> textLineSceneObjects;
    
    for (auto& textLine: textLines) {
        auto textLineSceneObject {std::make_unique<Pht::SceneObject>()};
        Pht::Vec3 textLinePosition {textLine.mPosition.x, textLine.mPosition.y, 0.0f};
        textLineSceneObject->GetTransform().SetPosition(textLinePosition);
        
        auto textComponent {
            std::make_unique<Pht::TextComponent>(*textLineSceneObject,
                                                 textLine.mText,
                                                 Pht::TextProperties{mFont})
        };
    
        textLineSceneObject->SetComponent<Pht::TextComponent>(std::move(textComponent));
        sceneObject->AddChild(*textLineSceneObject);
        textLineSceneObjects.push_back(std::move(textLineSceneObject));
    }
    
    mContainerSceneObject->AddChild(*sceneObject);
    mTexts.push_back(Text {displayTime, std::move(sceneObject), std::move(textLineSceneObjects)});
}

void SlidingTextAnimation::Init() {
    mScene.GetHudContainer().AddChild(*mContainerSceneObject);
    
    for (auto& text: mTexts) {
        text.mSceneObject->SetIsVisible(false);
    }
}

void SlidingTextAnimation::Start(Message message) {
    mState = State::SlidingIn;
    
    mText = &mTexts[static_cast<int>(message)];
    mText->mSceneObject->SetIsVisible(true);

    auto distance = centerPosition - mSlideInStartPosition;
    mVelocity = distance * 2.0f / slideTime;
    mAcceleration = -mVelocity / slideTime;
    mText->mSceneObject->GetTransform().SetPosition(mSlideInStartPosition);
    mElapsedTime = 0.0f;
    SetAlpha(0.0f);
}

void SlidingTextAnimation::SetAlpha(float newAlhpa) {
    for (auto& textLineSceneObject: mText->mTextLines) {
        auto* textComponent {textLineSceneObject->GetComponent<Pht::TextComponent>()};
        textComponent->GetProperties().mColor.w = newAlhpa;
    }
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
    auto& transform {mText->mSceneObject->GetTransform()};
    transform.Translate(mVelocity * dt);
    mVelocity += mAcceleration * dt;
    mElapsedTime += dt;
    
    SetAlpha(alpha * mElapsedTime / slideTime);
    
    if (transform.GetPosition().y >= 0.0f || mElapsedTime > slideTime) {
        transform.SetPosition({0.0f, 0.0f, 0.0f});
        mState = State::DisplayingText;
        mElapsedTime = 0.0f;
        SetAlpha(alpha);
        
        mEngine.GetInput().EnableInput();
    }
}

void SlidingTextAnimation::UpdateInDisplayingTextState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedTime > mText->mDisplayTime || mEngine.GetInput().ConsumeWholeTouch()) {
        StartSlideOut();
    }
}

void SlidingTextAnimation::StartSlideOut() {
    mState = State::SlidingOut;
    
    auto distance = mSlideOutFinalPosition - centerPosition;
    auto finalVelocity {distance * 2.0f / slideTime};
    mVelocity = {0.0f, 0.0f, 0.0f};
    mAcceleration = finalVelocity / slideTime;
    mElapsedTime = 0.0f;
}

void SlidingTextAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    auto& transform {mText->mSceneObject->GetTransform()};
    transform.Translate(mVelocity * dt);
    mVelocity += mAcceleration * dt;
    mElapsedTime += dt;
    
    SetAlpha(alpha * (slideTime - mElapsedTime) / slideTime);
    
    if (transform.GetPosition().y >= mSlideOutFinalPosition.y || mElapsedTime > slideTime) {
        transform.SetPosition(mSlideOutFinalPosition);
        mState = State::Inactive;
        mText->mSceneObject->SetIsVisible(false);
    }
}
