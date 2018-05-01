#include "SlidingTextAnimation.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "TextComponent.hpp"
#include "MathUtils.hpp"
#include "Scene.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"
#include "GradientRectangle.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto rectangleFadeInTime {0.3f};
    constexpr auto slideTime {0.2f};
    constexpr auto displayDistance {0.65f};
    constexpr auto textWidth {8.0f};
    const Pht::Vec3 centerPosition {0.0f, 1.0f, 0.0f};
    
    Pht::StaticVector<Pht::Vec2, 20> scalePoints {
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
    
    void SetAlpha(Pht::SceneObject& sceneObject, float alpha) {
        for (auto& child: sceneObject.GetChildren()) {
            SetAlpha(*child, alpha);
        }
        
        if (auto* renderable {sceneObject.GetRenderable()}) {
            renderable->GetMaterial().SetOpacity(alpha);
        }
    }
}

SlidingTextAnimation::SlidingTextAnimation(Pht::IEngine& engine,
                                           GameScene& scene,
                                           const CommonResources& commonResources) :
    mEngine {engine},
    mScene {scene} {
    
    auto& font {commonResources.GetHussarFontSize52PotentiallyZoomedScreen()};

    mTexts.reserve(4);
    // CreateText(font, 2.5f, {{-3.1f, 0.5}, "Clear all"}, {{-4.0f, -1.0f}, "gray blocks!"});
    CreateText(font, 2.5f, {{-3.1f, 1.0f}, "CLEAR ALL"}, {{-4.0f, -1.0f}, "GRAY BLOCKS"});
    CreateText(font, 2.5f, {{-4.0f, 1.0f}, "You cleared"}, {{-3.55f, -1.0f}, "all blocks!"});
    CreateText(font, 2.5f, {{-2.5f, 1.0f}, "Fill all"}, {{-3.5f, -1.0f}, "gray slots!"});
    CreateText(font, 2.5f, {{-3.3f, 1.0f}, "You filled"}, {{-3.05f, -1.0f}, "all slots!"});
}

void SlidingTextAnimation::CreateText(const Pht::Font& font,
                                      float displayTime,
                                      const TextLine& upperTextLine,
                                      const TextLine& lowerTextLine) {
    // Pht::TextProperties textProperties {font, 1.0f, {1.0f, 1.0f, 1.0f, alpha}};
    // Pht::TextProperties textProperties {font, 1.0f, {0.88f, 0.88f, 0.88f, 1.0f}};
    // Pht::TextProperties textProperties {font, 1.0f, {1.0f, 0.9f, 0.8f, 1.0f}};
    Pht::TextProperties textProperties {
        font,
        1.0f,
        {1.0f, 0.95f, 0.9f, 1.0f}, // {1.0f, 0.9f, 0.8f, 1.0f},
        Pht::TextShadow::Yes,
        {0.1f, 0.1f},
        {0.2f, 0.2f, 0.2f, 0.5f}
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.1f;

    auto upperTextLineSceneObject {std::make_unique<Pht::SceneObject>()};
    auto upperTextComponent {
        std::make_unique<Pht::TextComponent>(*upperTextLineSceneObject,
                                             upperTextLine.mText,
                                             textProperties)
    };
    upperTextLineSceneObject->SetComponent<Pht::TextComponent>(std::move(upperTextComponent));
    
    auto lowerTextLineSceneObject {std::make_unique<Pht::SceneObject>()};
    auto lowerTextComponent {
        std::make_unique<Pht::TextComponent>(*lowerTextLineSceneObject,
                                             lowerTextLine.mText,
                                             textProperties)
    };
    lowerTextLineSceneObject->SetComponent<Pht::TextComponent>(std::move(lowerTextComponent));
    
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
    auto& containerSceneObject {mScene.GetScene().CreateSceneObject()};
    containerSceneObject.GetTransform().SetPosition(centerPosition);
    mScene.GetHudContainer().AddChild(containerSceneObject);
    
    for (auto& text: mTexts) {
        text.mUpperTextLineSceneObject->SetIsVisible(false);
        containerSceneObject.AddChild(*(text.mUpperTextLineSceneObject));
        text.mLowerTextLineSceneObject->SetIsVisible(false);
        containerSceneObject.AddChild(*(text.mLowerTextLineSceneObject));
    }
    
    CreateGradientRectangles(containerSceneObject);

    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    mLeftPosition = {-frustumSize.x / 2.0f - textWidth / 2.0f, 0.0f, 0.0f};
    mRightPosition = {frustumSize.x / 2.0f + textWidth / 2.0f, 0.0f, 0.0f};
}

void SlidingTextAnimation::CreateGradientRectangles(Pht::SceneObject& containerSceneObject) {
    auto& scene {mScene.GetScene()};
    mGradientRectanglesSceneObject = &scene.CreateSceneObject();
    containerSceneObject.AddChild(*mGradientRectanglesSceneObject);
    mGradientRectanglesSceneObject->SetIsVisible(false);
    
    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    
    auto height {4.0f};
    auto stripeHeight {0.11f};
    auto stripeOffset {0.25f};
    Pht::Vec2 size {frustumSize.x, height};
    auto leftQuadWidth {frustumSize.x / 3.0f};
    auto rightQuadWidth {frustumSize.x / 3.0f};

    GradientRectangleColors colors {
        .mLeft = {0.9f, 0.45f, 0.0f, 0.8f},
        .mMid = {0.3f, 0.3f, 0.35f, 0.8f},
        .mRight = {0.9f, 0.45f, 0.0f, 0.8f}
    };

    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, 0.0f, UiLayer::textRectangle},
                            size,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);
    
    Pht::Vec2 stripeSize {frustumSize.x, stripeHeight};
    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, height / 2.0f + stripeOffset, UiLayer::textRectangle},
                            stripeSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);
    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, -height / 2.0f - stripeOffset, UiLayer::textRectangle},
                            stripeSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);
}

void SlidingTextAnimation::Start(Message message) {
    mState = State::RectangleAppearing;
    
    mText = &mTexts[static_cast<int>(message)];
    mText->mUpperTextLineSceneObject->SetIsVisible(true);
    mText->mLowerTextLineSceneObject->SetIsVisible(true);

    mElapsedTime = 0.0f;
    
    mTextPosition = {0.0f, 0.0f, 0.0f};
    
    mDisplayVelocity = displayDistance / mText->mDisplayTime;
    mInitialVelocity = mRightPosition.x * 2.0f / slideTime - mDisplayVelocity -
                       mDisplayVelocity * mText->mDisplayTime / slideTime;
    mVelocity = mInitialVelocity;
    
    UpdateTextLineSceneObjectPositions();
    
    mGradientRectanglesSceneObject->SetIsVisible(true);
    mGradientRectanglesSceneObject->SetIsStatic(false);
    mGradientRectanglesSceneObject->GetTransform().SetPosition({0.0f, 0.0f, 0.0f});
    SetAlpha(*mGradientRectanglesSceneObject, 0.0f);
}

SlidingTextAnimation::State SlidingTextAnimation::Update() {
    switch (mState) {
        case State::RectangleAppearing:
            UpdateInRectangleAppearingState();
            break;
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::DisplayingText:
            UpdateInDisplayingTextState();
            break;
        case State::SlidingOut:
            UpdateInSlidingOutState();
            break;
        case State::RectangleDisappearing:
            UpdateInRectangleDisappearingState();
            break;
        case State::Inactive:
            break;
    }
    
    return mState;
}

void SlidingTextAnimation::UpdateInRectangleAppearingState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mElapsedTime += dt;
    
    SetAlpha(*mGradientRectanglesSceneObject, mElapsedTime / rectangleFadeInTime);

    auto normalizedTime {(rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime};
    auto scale {1.0f + 3.0f * Pht::Lerp(normalizedTime, scalePoints)};
    
    mGradientRectanglesSceneObject->GetTransform().SetScale(scale);
    
    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::SlidingIn;
        SetAlpha(*mGradientRectanglesSceneObject, 1.0f);
        mGradientRectanglesSceneObject->GetTransform().SetScale(1.0f);
    }
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
        mState = State::RectangleDisappearing;
        mElapsedTime = 0.0f;

        mText->mUpperTextLineSceneObject->SetIsVisible(false);
        mText->mLowerTextLineSceneObject->SetIsVisible(false);
    }
}

void SlidingTextAnimation::UpdateInRectangleDisappearingState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mElapsedTime += dt;
    
    SetAlpha(*mGradientRectanglesSceneObject,
             (rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime);

    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::Inactive;
        mGradientRectanglesSceneObject->SetIsVisible(false);
        mGradientRectanglesSceneObject->SetIsStatic(true);
    }
}
