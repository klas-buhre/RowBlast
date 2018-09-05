#include "ComboTextAnimation.hpp"

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
#include "GameScene.hpp"
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 startPosition {0.0f, 5.0f, 0.0f};
    constexpr auto scaleInDuration {0.27f};
    constexpr auto displayTextDuration {0.6f};
    constexpr auto slideOutDuration {0.25f};
    constexpr auto slideDistance {12.0f};
    constexpr auto acceleration {2.0f * slideDistance / (slideOutDuration * slideOutDuration)};
    // constexpr auto textAlpha {0.93f};
    constexpr auto textAlpha {0.94f};
    constexpr auto textShadowAlpha {0.5f};
    constexpr auto textScale {0.9f};
    
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

ComboTextAnimation::ComboTextAnimation(Pht::IEngine& engine,
                                       GameScene& scene,
                                       const CommonResources& commonResources) :
    mScene {scene} {
    
    auto& font {commonResources.GetHussarFontSize52PotentiallyZoomedScreen()};
    CreateText(font, {-3.0f, -0.5f}, "COMBO!");
    CreateText(font, {-3.0f, -0.5f}, "AWESOME!");
    
    CreateTwinkleParticleEffect(engine);
}

void ComboTextAnimation::CreateText(const Pht::Font& font,
                                    const Pht::Vec2& position,
                                    const std::string& text) {
    Pht::TextProperties textProperties {
        font,
        textScale,
        {1.0f, 1.0f, 1.0f, textAlpha},
        Pht::TextShadow::Yes,
        {0.08f, 0.08f},
        {0.2f, 0.2f, 0.2f, textShadowAlpha},
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.15f;
    // textProperties.mGradientBottomColorSubtraction = Pht::Vec3 {0.2f, 0.2f, 0.0f};
    textProperties.mGradientBottomColorSubtraction = Pht::Vec3 {0.0f, 0.2f, 0.2f};

    auto textSceneObject {std::make_unique<Pht::SceneObject>()};
    textSceneObject->GetTransform().SetPosition({position.x, position.y, UiLayer::text});
    
    auto textComponent {
        std::make_unique<Pht::TextComponent>(*textSceneObject, text, textProperties)
    };
    textSceneObject->SetComponent(std::move(textComponent));
    
    mTextSceneObjects.push_back(std::move(textSceneObject));
}

void ComboTextAnimation::CreateTwinkleParticleEffect(Pht::IEngine& engine) {
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
        .mTimeToLive = displayTextDuration,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = 100.0f,
        .mSize = Pht::Vec2{4.5f, 4.5f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.3f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Triangles);
}

void ComboTextAnimation::Init() {
    mContainerSceneObject = &mScene.GetScene().CreateSceneObject();
    mScene.GetHudContainer().AddChild(*mContainerSceneObject);
    
    for (auto& textSceneObject: mTextSceneObjects) {
        mContainerSceneObject->AddChild(*textSceneObject);
    }
    
    mContainerSceneObject->AddChild(*mTwinkleParticleEffect);
    
    HideAllTextObjects();
}

void ComboTextAnimation::Start(Message message) {
    HideAllTextObjects();
    
    mActiveTextSceneObject = mTextSceneObjects[static_cast<int>(message)].get();
    mActiveTextSceneObject->SetIsVisible(true);
    mActiveTextSceneObject->SetIsStatic(false);
    
    auto& textProperties {
        mActiveTextSceneObject->GetComponent<Pht::TextComponent>()->GetProperties()
    };
    textProperties.mColor.w = textAlpha;
    textProperties.mShadowColor.w = textShadowAlpha;
    
    mState = State::ScalingIn;
    mElapsedTime = 0.0f;
    mContainerSceneObject->GetTransform().SetPosition(startPosition);
    
    switch (message) {
        case Message::Combo:
            mTwinkleParticleEffect->GetTransform().SetPosition({-2.7f, 0.3f, UiLayer::text});
            break;
        case Message::Awesome:
            mTwinkleParticleEffect->GetTransform().SetPosition({-2.7f, 0.3f, UiLayer::text});
            break;
    }
}

void ComboTextAnimation::Update(float dt) {
    switch (mState) {
        case State::ScalingIn:
            UpdateInScalingInState(dt);
            break;
        case State::DisplayingText:
            UpdateInDisplayingTextState(dt);
            break;
        case State::SlidingOut:
            UpdateInSlidingOutState(dt);
            break;
        case State::Inactive:
            break;
    }
}

void ComboTextAnimation::UpdateInScalingInState(float dt) {
    mElapsedTime += dt;
    
    if (mElapsedTime > scaleInDuration) {
        mState = State::DisplayingText;
        mElapsedTime = 0.0f;
        Pht::SceneObjectUtils::ScaleRecursively(*mContainerSceneObject, textScale);
        
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    } else {
        auto reversedNormalizedTime {1.0f - (mElapsedTime / scaleInDuration)};
        auto scale {(1.0f - Pht::Lerp(reversedNormalizedTime, scalePoints)) * textScale};

        Pht::SceneObjectUtils::ScaleRecursively(*mContainerSceneObject, scale);
    }
}

void ComboTextAnimation::UpdateInDisplayingTextState(float dt) {
    mElapsedTime += dt;
    
    mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    
    if (mElapsedTime > displayTextDuration) {
        mState = State::SlidingOut;
        mElapsedTime = 0.0f;
        mVelocity = -25.0f;
        mTextSlide = 0.0f;
        
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    }
}

void ComboTextAnimation::UpdateInSlidingOutState(float dt) {
    mElapsedTime += dt;
    
    if (mElapsedTime > slideOutDuration) {
        mState = State::Inactive;
        HideAllTextObjects();
    } else {
        mVelocity += acceleration * dt;
        mTextSlide += mVelocity * dt;
        
        Pht::Vec3 position {startPosition.x, startPosition.y + mTextSlide, startPosition.z};
        mContainerSceneObject->GetTransform().SetPosition(position);
        
        auto alpha {(slideOutDuration - mElapsedTime) / slideOutDuration};
        auto& textProperties {
            mActiveTextSceneObject->GetComponent<Pht::TextComponent>()->GetProperties()
        };
        textProperties.mColor.w = alpha * textAlpha;
        textProperties.mShadowColor.w = alpha * textShadowAlpha;
    }
}

void ComboTextAnimation::HideAllTextObjects() {
    for (auto& textSceneObject: mTextSceneObjects) {
        textSceneObject->SetIsVisible(false);
        textSceneObject->SetIsStatic(true);
    }
}
