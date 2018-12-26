#include "SlidingTextAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "TextComponent.hpp"
#include "MathUtils.hpp"
#include "Scene.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"
#include "GradientRectangle.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto rectangleFadeInTime {0.3f};
    constexpr auto slideTime {0.2f};
    constexpr auto ufoHeadStartTime {0.2f};
    constexpr auto displayDistance {0.65f};
    constexpr auto textWidth {8.8f};
    const Pht::Vec3 centerPosition {0.0f, 1.0f, 0.0f};
    const Pht::Vec3 leftUfoPosition {-13.0f, 4.6f, UiLayer::slidingTextUfo};
    const Pht::Vec3 centerUfoPosition {0.0f, 4.6f, UiLayer::slidingTextUfo};
    const Pht::Vec3 rightUfoPosition {13.0f, 4.6f, UiLayer::slidingTextUfo};
    
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

SlidingTextAnimation::SlidingTextAnimation(Pht::IEngine& engine,
                                           GameScene& scene,
                                           const CommonResources& commonResources) :
    mEngine {engine},
    mScene {scene},
    mUfo {engine, commonResources, 3.2f},
    mUfoAnimation {engine, mUfo} {
    
    auto& font {commonResources.GetHussarFontSize52PotentiallyZoomedScreen()};

    mTexts.reserve(6);
    CreateText(font, 2.5f, {{-3.96f, 0.33f}, "CLEAR ALL"}, {{-5.17f, -1.58f}, "GRAY BLOCKS"});
    CreateText(font, 1.6f, {{-1.21f, 0.33f}, "ALL"}, {{-3.3f, -1.58f}, "CLEARED!"});
    CreateText(font, 2.5f, {{-2.97f, 0.33f}, "FILL ALL"}, {{-4.73f, -1.58f}, "GRAY SLOTS"});
    CreateText(font, 2.5f, {{-3.85f, 0.33f}, "ALL SLOTS"}, {{-2.31f, -1.58f}, "FILLED!"});
    CreateText(font, 2.5f, {{-5.0f, 0.33f}, "BRING DOWN"}, {{-5.2f, -1.58f}, "THE ASTEROID"});
    CreateText(font, 2.5f, {{-5.1f, 0.33f}, "THE ASTEROID"}, {{-3.6f, -1.58f}, "IS DOWN!"});

    CreateTwinkleParticleEffect();
}

void SlidingTextAnimation::CreateText(const Pht::Font& font,
                                      float displayTime,
                                      const TextLine& upperTextLine,
                                      const TextLine& lowerTextLine) {
    Pht::TextProperties textProperties {
        font,
        1.1f,
        {1.0f, 0.95f, 0.9f, 1.0f},
        Pht::TextShadow::Yes,
        {0.03f, 0.03f},
        {0.8f, 0.6f, 0.4f, 1.0f}
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.15f;
    textProperties.mMidGradientColorSubtraction = Pht::Vec3 {0.1f, 0.2f, 0.3f};
    textProperties.mSpecular = Pht::TextSpecular::Yes;
    textProperties.mSpecularOffset = {0.02f, 0.02f};
    textProperties.mSecondShadow = Pht::TextShadow::Yes;
    textProperties.mSecondShadowColor = Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f};
    textProperties.mSecondShadowOffset = Pht::Vec2 {0.075f, 0.075f};

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
            Pht::Vec3{upperTextLine.mPosition.x, upperTextLine.mPosition.y, 0.5f},
            std::move(upperTextLineSceneObject),
            Pht::Vec3{lowerTextLine.mPosition.x, lowerTextLine.mPosition.y, 0.5f},
            std::move(lowerTextLineSceneObject)
        }
    );
}

void SlidingTextAnimation::CreateTwinkleParticleEffect() {
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
        .mSize = Pht::Vec2{5.5f, 5.5f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.5f
    };
    
    auto& particleSystem {mEngine.GetParticleSystem()};
    mTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Triangles);
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
    
    containerSceneObject.AddChild(*mTwinkleParticleEffect);
    
    mUfo.Init(mScene.GetUiViewsContainer());
    mUfo.Hide();
    mEngine.GetRenderer().DisableShader(Pht::ShaderType::TexturedEnvMapLighting);

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
    
    auto height {4.1f};
    auto stripeHeight {0.09f};
    auto stripeOffset {stripeHeight / 2.0f};
    Pht::Vec2 size {frustumSize.x, height};
    auto leftQuadWidth {frustumSize.x / 3.0f};
    auto rightQuadWidth {frustumSize.x / 3.0f};

    GradientRectangleColors colors {
        .mLeft = {0.93f, 0.5f, 0.0f, 0.8f},
        .mMid = {0.3f, 0.3f, 0.35f, 0.8f},
        .mRight = {0.93f, 0.5f, 0.0f, 0.8f}
    };

    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, 0.0f, UiLayer::slidingTextRectangle},
                            size,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);

    GradientRectangleColors stripeColors {
        .mLeft = {0.93f, 0.5f, 0.0f, 0.8f},
        .mMid = {1.0f, 0.95f, 0.9f, 0.95f},
        .mRight = {0.93f, 0.5f, 0.0f, 0.8f}
    };

    Pht::Vec2 stripeSize {frustumSize.x, stripeHeight};
    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, height / 2.0f + stripeOffset, UiLayer::slidingTextRectangle},
                            stripeSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            stripeColors,
                            stripeColors);
    CreateGradientRectangle(scene,
                            *mGradientRectanglesSceneObject,
                            {0.0f, -height / 2.0f - stripeOffset, UiLayer::slidingTextRectangle},
                            stripeSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            stripeColors,
                            stripeColors);
}

void SlidingTextAnimation::Start(Message message) {
    mState = State::RectangleAppearing;
    
    mText = &mTexts[static_cast<int>(message)];

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
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject, 0.0f);
    
    mUfo.SetPosition(rightUfoPosition);
    mUfo.Show();
    mUfoAnimation.Init();
    mEngine.GetRenderer().EnableShader(Pht::ShaderType::TexturedEnvMapLighting);
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
    
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject,
                                               mElapsedTime / rectangleFadeInTime);

    auto normalizedTime {(rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime};
    auto scale {1.0f + 3.0f * Pht::Lerp(normalizedTime, scalePoints)};
    
    mGradientRectanglesSceneObject->GetTransform().SetScale(scale);
    
    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::SlidingIn;
        Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject, 1.0f);
        mGradientRectanglesSceneObject->GetTransform().SetScale(1.0f);
        
        mText->mUpperTextLineSceneObject->SetIsVisible(true);
        mText->mLowerTextLineSceneObject->SetIsVisible(true);
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
        
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
        
        mUfoAnimation.StartShortWarpSpeed(centerUfoPosition);
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
    
    mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mUfoAnimation.Update();
    
    if (mElapsedTime > mText->mDisplayTime || mEngine.GetInput().ConsumeWholeTouch()) {
        mState = State::SlidingOut;
        mElapsedTime = 0.0f;
        mVelocity = mDisplayVelocity;
        
        if (!mUfoAnimation.IsActive()) {
            mUfoAnimation.StartShortWarpSpeed(leftUfoPosition);
        }
    }
    
    if (mElapsedTime > mText->mDisplayTime - ufoHeadStartTime && !mUfoAnimation.IsActive()) {
        mUfoAnimation.StartShortWarpSpeed(leftUfoPosition);
    }
    
    UpdateTextLineSceneObjectPositions();
}

void SlidingTextAnimation::UpdateInSlidingOutState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    mTextPosition.x += mVelocity * dt;
    mVelocity += dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    mElapsedTime += dt;

    UpdateTextLineSceneObjectPositions();
    
    mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mUfoAnimation.Update();

    if (mTextPosition.x >= mRightPosition.x * 2.0f) {
        mState = State::RectangleDisappearing;
        mElapsedTime = 0.0f;

        mText->mUpperTextLineSceneObject->SetIsVisible(false);
        mText->mLowerTextLineSceneObject->SetIsVisible(false);
        
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    }
}

void SlidingTextAnimation::UpdateInRectangleDisappearingState() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mElapsedTime += dt;
    
    Pht::SceneObjectUtils::SetAlphaRecursively(*mGradientRectanglesSceneObject,
                                               (rectangleFadeInTime - mElapsedTime) / rectangleFadeInTime);
    mUfoAnimation.Update();

    if (mElapsedTime > rectangleFadeInTime) {
        mState = State::Inactive;
        mGradientRectanglesSceneObject->SetIsVisible(false);
        mGradientRectanglesSceneObject->SetIsStatic(true);
        mUfo.Hide();
        mEngine.GetRenderer().DisableShader(Pht::ShaderType::TexturedEnvMapLighting);
    }
}
