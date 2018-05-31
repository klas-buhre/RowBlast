#include "TitleAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "TextComponent.hpp"
#include "MathUtils.hpp"
#include "Scene.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GradientRectangle.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto waitTime {1.0f};
    constexpr auto rectangleFadeInTime {0.3f};
    constexpr auto slideTime {0.23f};
    constexpr auto displayDistance {0.65f};
    constexpr auto displayTime {1.75f};
    constexpr auto textWidth {10.0f};
    const Pht::Vec3 centerPosition {0.0f, 5.0f, 0.0f};
    
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
}

TitleAnimation::TitleAnimation(Pht::IEngine& engine,
                               Pht::Scene& scene,
                               Pht::SceneObject& parentObject) :
    mEngine {engine},
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(100)},
    mTextPosition {0.0f, 0.0f, 0.0f} {
    
    CreateText();
    CreateTwinkleParticleEffect(engine);
    
    auto& containerSceneObject {scene.CreateSceneObject()};
    containerSceneObject.GetTransform().SetPosition(centerPosition);
    parentObject.AddChild(containerSceneObject);
    
    containerSceneObject.AddChild(*(mText.mUpperTextLineSceneObject));
    containerSceneObject.AddChild(*(mText.mLowerTextLineSceneObject));
    
    CreateGradientRectangles(scene, containerSceneObject);
    
    containerSceneObject.AddChild(*mTwinkleParticleEffect);

    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    mLeftPosition = {-frustumSize.x / 2.0f - textWidth / 2.0f, 0.0f, 0.0f};
    mRightPosition = {frustumSize.x / 2.0f + textWidth / 2.0f, 0.0f, 0.0f};
    
    mDisplayVelocity = displayDistance / displayTime;
    mInitialVelocity = mRightPosition.x * 2.0f / slideTime - mDisplayVelocity -
                       mDisplayVelocity * displayTime / slideTime;
    mVelocity = mInitialVelocity;
    
    UpdateTextLineSceneObjectPositions();
}

void TitleAnimation::CreateText() {
    Pht::TextProperties textProperties {
        mFont,
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.1f, 0.1f},
        {0.3f, 0.3f, 0.3f, 0.5f}
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mGradientBottomColorSubtraction = Pht::Vec3 {0.15f, 0.15f, 0.0f};

    mText.mUpperTextLineSceneObject = std::make_unique<Pht::SceneObject>();
    auto upperTextComponent {
        std::make_unique<Pht::TextComponent>(*mText.mUpperTextLineSceneObject,
                                             "ROW",
                                             textProperties)
    };
    mText.mUpperTextLineSceneObject->SetComponent<Pht::TextComponent>(std::move(upperTextComponent));
    
    mText.mLowerTextLineSceneObject = std::make_unique<Pht::SceneObject>();
    auto lowerTextComponent {
        std::make_unique<Pht::TextComponent>(*mText.mLowerTextLineSceneObject,
                                             "BLAST",
                                             textProperties)
    };
    mText.mLowerTextLineSceneObject->SetComponent<Pht::TextComponent>(std::move(lowerTextComponent));
    
    mText.mUpperTextLinePosition = {-4.0f, 0.2f, UiLayer::text};
    mText.mLowerTextLinePosition = {-5.6f, -2.0f, UiLayer::text};
    
    mText.mUpperTextLineSceneObject->SetIsVisible(false);
    mText.mLowerTextLineSceneObject->SetIsVisible(false);
}

void TitleAnimation::CreateTwinkleParticleEffect(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "particle_sprite_twinkle_blurred.png",
        .mTimeToLive = 1.2f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = 100.0f,
        .mSize = Pht::Vec2{6.0f, 6.0f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.5f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Triangles);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.5f, 1.45f, UiLayer::text});
}

void TitleAnimation::CreateGradientRectangles(Pht::Scene& scene,
                                              Pht::SceneObject& containerSceneObject) {
    mGradientRectanglesSceneObject = &scene.CreateSceneObject();
    containerSceneObject.AddChild(*mGradientRectanglesSceneObject);
    
    auto& frustumSize {mEngine.GetRenderer().GetHudFrustumSize()};
    
    auto height {4.7f};
    auto stripeHeight {0.11f};
    auto stripeOffset {0.25f};
    Pht::Vec2 size {frustumSize.x, height};
    auto leftQuadWidth {frustumSize.x / 2.2f};
    auto rightQuadWidth {frustumSize.x / 2.2f};

    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.8f},
        .mMid = {0.6f, 0.3f, 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
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
    
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject, 0.0f);
}

void TitleAnimation::Update() {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState();
            break;
        case State::RectangleAppearing:
            UpdateInRectangleAppearingState();
            break;
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::SlidingSlowly:
            UpdateInSlidingSlowlyState();
            break;
        case State::Inactive: {
            auto dt {mEngine.GetLastFrameSeconds()};
            mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
            break;
        }
    }
}

void TitleAnimation::UpdateInWaitingState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mElapsedTime += dt;
    
    if (mElapsedTime > waitTime) {
        mState = State::RectangleAppearing;
        mElapsedTime = 0.0f;
    }
}

void TitleAnimation::UpdateInRectangleAppearingState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mElapsedTime += dt;
    
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject,
                                               mElapsedTime / rectangleFadeInTime);

    auto normalizedTime {(rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime};
    auto scale {1.0f + 3.0f * Pht::Lerp(normalizedTime, scalePoints)};
    
    mGradientRectanglesSceneObject->GetTransform().SetScale(scale);
    
    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::SlidingIn;
        Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject, 1.0f);
        mGradientRectanglesSceneObject->GetTransform().SetScale(1.0f);
        
        mText.mUpperTextLineSceneObject->SetIsVisible(true);
        mText.mLowerTextLineSceneObject->SetIsVisible(true);
    }
}

void TitleAnimation::UpdateInSlidingInState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mTextPosition.x += mVelocity * dt;
    mVelocity -= dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    mElapsedTime += dt;
    
    if (mVelocity < mDisplayVelocity ||
        mTextPosition.x >= mRightPosition.x - displayDistance / 2.0f) {

        mState = State::SlidingSlowly;
        mElapsedTime = 0.0f;
        mTextPosition.x = mRightPosition.x - displayDistance / 2.0f;
    }
    
    UpdateTextLineSceneObjectPositions();
}

void TitleAnimation::UpdateTextLineSceneObjectPositions() {
    auto upperTextLinePosition {mLeftPosition + mTextPosition + mText.mUpperTextLinePosition};
    mText.mUpperTextLineSceneObject->GetTransform().SetPosition(upperTextLinePosition);
    
    auto lowerTextLinePosition {mRightPosition - mTextPosition + mText.mLowerTextLinePosition};
    mText.mLowerTextLineSceneObject->GetTransform().SetPosition(lowerTextLinePosition);
}

void TitleAnimation::UpdateInSlidingSlowlyState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mTextPosition.x += mDisplayVelocity * dt;
    mElapsedTime += dt;
    
    if (mTextPosition.x > mRightPosition.x) {
        mState = State::Inactive;
        mTextPosition.x = mRightPosition.x;
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    }
    
    UpdateTextLineSceneObjectPositions();
}

bool TitleAnimation::IsDone() const {
    switch (mState) {
        case State::Inactive:
            return true;
        default:
            return false;
    }
}
