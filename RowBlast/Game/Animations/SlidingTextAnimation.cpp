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

namespace {
    constexpr auto slideTime {0.5f};
    const Pht::Vec3 centerPosition {0.0f, 0.0f, 0.0f};
    constexpr auto textHeight {3.0f};
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
    CreateText(font, 2.7f, {{{-3.1f, 1.0f}, "Clear all"}, {{-4.0f, -1.0f}, "gray blocks!"}});
    CreateText(font, 2.7f, {{{-4.0f, 1.0f}, "You cleared"}, {{-3.55f, -1.0f}, "all blocks!"}});
    CreateText(font, 2.7f, {{{-2.5f, 1.0f}, "Fill all"}, {{-3.5f, -1.0f}, "gray slots!"}});
    CreateText(font, 2.7f, {{{-3.3f, 1.0f}, "You filled"}, {{-3.05f, -1.0f}, "all slots!"}});
}

void SlidingTextAnimation::CreateText(const Pht::Font& font,
                                      float displayTime,
                                      const std::vector<TextLine>& textLines) {
    auto sceneObject {std::make_unique<Pht::SceneObject>()};
    std::vector<std::unique_ptr<Pht::SceneObject>> textLineSceneObjects;
    
    for (auto& textLine: textLines) {
        auto textLineSceneObject {std::make_unique<Pht::SceneObject>()};
        Pht::Vec3 textLinePosition {textLine.mPosition.x, textLine.mPosition.y, 0.0f};
        textLineSceneObject->GetTransform().SetPosition(textLinePosition);
        
        auto textComponent {
            std::make_unique<Pht::TextComponent>(*textLineSceneObject,
                                                 textLine.mText,
                                                 Pht::TextProperties{font})
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
    
    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    mSlideInStartPosition = {0.0f, -frustumSize.y / 4.0f - textHeight / 2.0f, 0.0f};
    mSlideOutFinalPosition = {0.0f, frustumSize.y / 4.0f + textHeight / 2.0f, 0.0f};
}

void SlidingTextAnimation::Start(Message message) {
    mState = State::SlidingIn;
    
    mText = &mTexts[static_cast<int>(message)];
    mText->mSceneObject->SetIsVisible(true);

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
    auto position {transform.GetPosition()};
    auto distance = centerPosition.y - mSlideInStartPosition.y;
    auto normalizedTime {(slideTime - mElapsedTime) / slideTime};
    
    position.y = centerPosition.y - distance * Pht::Lerp(normalizedTime, slidePoints);
    mElapsedTime += dt;
    
    transform.SetPosition(position);
    
    SetAlpha(alpha * mElapsedTime / slideTime);
    
    if (position.y >= 0.0f || mElapsedTime > slideTime) {
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
    mElapsedTime = 0.0f;
}

void SlidingTextAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    auto& transform {mText->mSceneObject->GetTransform()};
    auto position {transform.GetPosition()};
    auto distance = mSlideOutFinalPosition.y - centerPosition.y;
    auto normalizedTime {mElapsedTime / slideTime};

    position.y = centerPosition.y + distance * normalizedTime * normalizedTime * normalizedTime;
    mElapsedTime += dt;
    
    transform.SetPosition(position);
    
    SetAlpha(alpha * (slideTime - mElapsedTime) / slideTime);
    
    if (transform.GetPosition().y >= mSlideOutFinalPosition.y || mElapsedTime > slideTime) {
        transform.SetPosition(mSlideOutFinalPosition);
        mState = State::Inactive;
        mText->mSceneObject->SetIsVisible(false);
    }
}
