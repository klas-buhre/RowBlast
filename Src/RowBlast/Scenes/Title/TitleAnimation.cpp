#include "TitleAnimation.hpp"

#include <array>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IAudio.hpp"
#include "IInput.hpp"
#include "TextComponent.hpp"
#include "Scene.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto waitTime = 1.0f;
    constexpr auto slideTime = 0.23f;
    constexpr auto displayDistance = 0.65f;
    constexpr auto displayTime = 1.75f;
    constexpr auto textWidth = 11.0f;
    constexpr auto textHeight = 1.65f;
    const Pht::Vec3 centerPosition {0.0f, 8.0f, 0.0f};

    const std::array<Pht::Vec3, 3> twinklePositions {
        Pht::Vec3 {-3.5f, 1.45f, UiLayer::buttonOverlayObject1},
        Pht::Vec3 {1.7f, -0.55f, UiLayer::buttonOverlayObject1},
        Pht::Vec3 {-2.85f, -0.4f, UiLayer::buttonOverlayObject1}
    };
}

TitleAnimation::TitleAnimation(Pht::IEngine& engine) :
    mEngine {engine},
    mFont {"ethnocentric_rg_it.ttf", engine.GetRenderer().GetAdjustedNumPixels(100)},
    mTextPosition {0.0f, 0.0f, 0.0f} {
    
    CreateText();
    CreateTwinkleParticleEffect();
}

void TitleAnimation::Init(Pht::Scene& scene, Pht::SceneObject& parentObject) {
    auto& containerSceneObject = scene.CreateSceneObject();
    containerSceneObject.GetTransform().SetPosition(centerPosition);
    parentObject.AddChild(containerSceneObject);
    
    containerSceneObject.AddChild(*(mUpperTextLine.mTextSceneObject));
    containerSceneObject.AddChild(*(mUpperTextLine.mFlareEffect));
    containerSceneObject.AddChild(*(mLowerTextLine.mTextSceneObject));
    containerSceneObject.AddChild(*(mLowerTextLine.mFlareEffect));
    
    containerSceneObject.AddChild(*mTwinkleParticleEffect);

    auto& frustumSize = mEngine.GetRenderer().GetHudFrustumSize();
    mLeftPosition = {-frustumSize.x / 2.0f - textWidth / 2.0f, 0.0f, 0.0f};
    mRightPosition = {frustumSize.x / 2.0f + textWidth / 2.0f, 0.0f, 0.0f};
    
    mDisplayVelocity = displayDistance / displayTime;
    mInitialVelocity = mRightPosition.x * 2.0f / slideTime - mDisplayVelocity -
                       mDisplayVelocity * displayTime / slideTime;
    mVelocity = mInitialVelocity;
    
    UpdateTextLineSceneObjectPositions();
    Pht::SceneObjectUtils::ScaleRecursively(containerSceneObject, 1.1f);
}

void TitleAnimation::CreateText() {
    Pht::TextProperties textProperties {
        mFont,
        1.0f,
        {0.93f, 0.93f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.51f, 0.51f, 0.67f, 1.0f},
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mMidGradientColorSubtraction = Pht::Vec3 {0.57f, 0.57f, 0.375f};
    textProperties.mTopGradientColorSubtraction = Pht::Vec3 {0.6f, 0.3f, 0.1f};
    textProperties.mSpecular = Pht::TextSpecular::Yes;
    textProperties.mSpecularOffset = {0.04f, 0.04f};
    textProperties.mSecondShadow = Pht::TextShadow::Yes;
    textProperties.mSecondShadowColor = Pht::Vec4 {0.21f, 0.21f, 0.21f, 1.0f};
    textProperties.mSecondShadowOffset = Pht::Vec2 {0.05f, 0.05f};
    
    mUpperTextLine = CreateTextLine("ROW", {-4.0f, 0.2f, UiLayer::text}, textProperties);
    mLowerTextLine = CreateTextLine("BLAST", {-5.6f, -2.0f, UiLayer::text}, textProperties);
}

TitleAnimation::TextLine TitleAnimation::CreateTextLine(const std::string& text,
                                                        const Pht::Vec3& textPosition,
                                                        Pht::TextProperties textProperties) {
    TextLine textLine;
    textLine.mTextPosition = textPosition;
    textLine.mFlareEffect = CreateFlareParticleEffect(textPosition.y + textHeight / 2.0f);
    textLine.mTextSceneObject = std::make_unique<Pht::SceneObject>();
    textLine.mTextSceneObject->SetIsVisible(false);
    
    auto textComponent =
        std::make_unique<Pht::TextComponent>(*textLine.mTextSceneObject, text, textProperties);
    textLine.mTextSceneObject->SetComponent<Pht::TextComponent>(std::move(textComponent));
    
    return textLine;
}

std::unique_ptr<Pht::SceneObject> TitleAnimation::CreateFlareParticleEffect(float yPosition) {
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
        .mColor = Pht::Vec4{0.8f, 0.8f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare03.png",
        .mTimeToLive = 1.5f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 1.35f,
        .mZAngularVelocity = 0.0f,
        .mSize = Pht::Vec2{35.0f, 40.0f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 1.35f
    };

    auto& particleSystem = mEngine.GetParticleSystem();
    auto flareEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                      particleEmitterSettings,
                                                                      Pht::RenderMode::Triangles);
    flareEffect->GetTransform().SetPosition({0.0f, yPosition, UiLayer::background});
    
    auto& material = flareEffect->GetRenderable()->GetMaterial();
    material.SetShaderId(Pht::ShaderId::ParticleNoAlphaTexture);
    
    return flareEffect;
}

void TitleAnimation::CreateTwinkleParticleEffect() {
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
        .mTimeToLive = 1.4f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = 100.0f,
        .mSize = Pht::Vec2{6.0f, 6.0f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.5f
    };
    
    auto& particleSystem = mEngine.GetParticleSystem();
    mTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Triangles);
    mTwinkleParticleEffect->GetTransform().SetPosition(twinklePositions[0]);
}

void TitleAnimation::Update() {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState();
            break;
        case State::SlidingIn:
            UpdateInSlidingInState();
            break;
        case State::SlidingSlowly:
            UpdateInSlidingSlowlyState();
            break;
        case State::Twinkles: {
            UpdateInTwinklesState();
            break;
        }
    }
}

void TitleAnimation::UpdateInWaitingState() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mElapsedTime += dt;
    if (mElapsedTime > waitTime) {
        mState = State::SlidingIn;
        mElapsedTime = 0.0f;
        mUpperTextLine.mTextSceneObject->SetIsVisible(true);
        mLowerTextLine.mTextSceneObject->SetIsVisible(true);
        
        auto& audio = mEngine.GetAudio();
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::SlidingTextWhoosh1));
    }
}

void TitleAnimation::UpdateInSlidingInState() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mTextPosition.x += mVelocity * dt;
    mVelocity -= dt * (mInitialVelocity - mDisplayVelocity) / slideTime;
    if (mVelocity < mDisplayVelocity ||
        mTextPosition.x >= mRightPosition.x - displayDistance / 2.0f) {

        mState = State::SlidingSlowly;
        mTextPosition.x = mRightPosition.x - displayDistance / 2.0f;
        
        mUpperTextLine.mFlareEffect->GetComponent<Pht::ParticleEffect>()->Start();
        mLowerTextLine.mFlareEffect->GetComponent<Pht::ParticleEffect>()->Start();

        mEngine.GetInput().EnableInput();
    }
    
    UpdateTextLineSceneObjectPositions();
}

void TitleAnimation::UpdateTextLineSceneObjectPositions() {
    auto upperTextLinePosition = mLeftPosition + mTextPosition + mUpperTextLine.mTextPosition;
    mUpperTextLine.mTextSceneObject->GetTransform().SetPosition(upperTextLinePosition);
    
    auto lowerTextLinePosition = mRightPosition - mTextPosition + mLowerTextLine.mTextPosition;
    mLowerTextLine.mTextSceneObject->GetTransform().SetPosition(lowerTextLinePosition);
}

void TitleAnimation::UpdateInSlidingSlowlyState() {
    auto dt = mEngine.GetLastFrameSeconds();
    
    mTextPosition.x += mDisplayVelocity * dt;
    if (mTextPosition.x > mRightPosition.x) {
        mState = State::Twinkles;
        mTextPosition.x = mRightPosition.x;
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    }
    
    mUpperTextLine.mFlareEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mLowerTextLine.mFlareEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);

    UpdateTextLineSceneObjectPositions();
}

void TitleAnimation::UpdateInTwinklesState() {
    auto dt = mEngine.GetLastFrameSeconds();
    auto* particleEffect = mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>();
    
    particleEffect->Update(dt);
    
    if (!particleEffect->IsActive()) {
        auto twinklePosition = twinklePositions[std::rand() % twinklePositions.size()];
        mTwinkleParticleEffect->GetTransform().SetPosition(twinklePosition);
        particleEffect->Start();
    }
    
    mUpperTextLine.mFlareEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mLowerTextLine.mFlareEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
}

bool TitleAnimation::IsDone() const {
    switch (mState) {
        case State::Twinkles:
        case State::SlidingSlowly:
            return true;
        default:
            return false;
    }
}
