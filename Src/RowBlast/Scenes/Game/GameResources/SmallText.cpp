#include "SmallText.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IAudio.hpp"
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
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 startPosition {0.0f, 5.5f, 0.0f};
    constexpr auto scaleInDuration = 0.27f;
    constexpr auto displayTextDuration = 0.6f;
    constexpr auto slideOutDuration = 0.25f;
    constexpr auto slideDistance = 12.0f;
    constexpr auto acceleration = 2.0f * slideDistance / (slideOutDuration * slideOutDuration);
    constexpr auto textShadowAlpha = 0.5f;
    constexpr auto textScale = 0.83f;
    const std::string comboString {"COMBO "};
    const std::string comboDigits {"     "}; // Warning! Must be five spaces to fit digits.
    
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

SmallText::SmallText(Pht::IEngine& engine,
                     GameScene& scene,
                     const CommonResources& commonResources) :
    mEngine {engine},
    mScene {scene} {
    
    auto& font = commonResources.GetHussarFontSize52PotentiallyZoomedScreen();
    mComboTextSceneObject = &CreateText(font, {-3.4f, -0.5f}, comboString + comboDigits);
    mAwesomeTextSceneObject = &CreateText(font, {-3.8f, -0.5f}, "AWESOME!");
    mFantasticTextSceneObject = &CreateText(font, {-3.9f, -0.5f}, "FANTASTIC!");
    mUndoingTextSceneObject = &CreateText(font, {-3.3f, -0.5f}, "UNDOING");
    mWillUndoTextSceneObject = &CreateText(font, {-3.8f, -0.5f}, "WILL UNDO");
    mNoRoomTextSceneObject = &CreateText(font, {-3.3f, -0.5f}, "NO ROOM");
    
    CreateTwinkleParticleEffect(engine);
}

Pht::SceneObject& SmallText::CreateText(const Pht::Font& font,
                                        const Pht::Vec2& position,
                                        const std::string& text) {
    Pht::TextProperties textProperties {
        font,
        textScale,
        {1.0f, 0.906f, 0.906f, 1.0f},
        Pht::TextShadow::Yes,
        {0.04f, 0.04f},
        {0.58f, 0.42f, 0.52f, 1.0f}
    };
    textProperties.mSnapToPixel = Pht::SnapToPixel::No;
    textProperties.mItalicSlant = 0.15f;
    textProperties.mMidGradientColorSubtraction = Pht::Vec3 {0.0f, 0.3f, 0.2f};
    textProperties.mSpecular = Pht::TextSpecular::Yes;
    textProperties.mSpecularOffset = {0.03f, 0.03f};
    textProperties.mSecondShadow = Pht::TextShadow::Yes;
    textProperties.mSecondShadowColor = Pht::Vec4 {0.19f, 0.19f, 0.19f, textShadowAlpha};
    textProperties.mSecondShadowOffset = Pht::Vec2 {0.075f, 0.075f};

    auto textSceneObject = std::make_unique<Pht::SceneObject>();
    textSceneObject->GetTransform().SetPosition({position.x, position.y, UiLayer::text});
    
    auto textComponent =
        std::make_unique<Pht::TextComponent>(*textSceneObject, text, textProperties);
    textSceneObject->SetComponent(std::move(textComponent));
    
    auto& retval = *textSceneObject;
    mTextSceneObjects.push_back(std::move(textSceneObject));
    
    return retval;
}

void SmallText::CreateTwinkleParticleEffect(Pht::IEngine& engine) {
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
        .mSize = Pht::Vec2{4.6f, 4.6f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.3f
    };
    
    auto& particleSystem = engine.GetParticleSystem();
    mTwinkleParticleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                            particleEmitterSettings,
                                                                            Pht::RenderMode::Triangles);
}

void SmallText::Init() {
    mContainerSceneObject = &mScene.GetScene().CreateSceneObject();
    mScene.GetHudContainer().AddChild(*mContainerSceneObject);
    mContainerSceneObject->SetIsVisible(false);
    mContainerSceneObject->SetIsStatic(true);
    
    for (auto& textSceneObject: mTextSceneObjects) {
        mContainerSceneObject->AddChild(*textSceneObject);
    }
    
    mContainerSceneObject->AddChild(*mTwinkleParticleEffect);
    
    HideAllTextObjects();
}

void SmallText::StartComboMessage(int numCombos) {
    if (IsFantasticTextActive() || IsAwesomeTextActive()) {
        return;
    }
    
    auto& audio = mEngine.GetAudio();
    
    if (numCombos >= 9) {
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Fantastic));
    } else if (numCombos >= 7) {
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Awesome));
    } else if (numCombos >= 5) {
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Combo2));
    } else if (numCombos >= 3) {
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Combo1));
    }

    Start(*mComboTextSceneObject);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.15f, 0.3f, UiLayer::text});
    
    const auto bufSize = 64;
    char buffer[bufSize];
    std::snprintf(buffer, bufSize, "%d!  ", numCombos);
    
    auto& text = mComboTextSceneObject->GetComponent<Pht::TextComponent>()->GetText();
    auto textLength = text.size();
    auto comboStringSize = comboString.size();
    assert(textLength >= comboStringSize + comboDigits.size());
    text[comboStringSize] = buffer[0];
    text[comboStringSize + 1] = buffer[1];
    text[comboStringSize + 2] = buffer[2];
    text[comboStringSize + 3] = buffer[3];
}

void SmallText::StartAwesomeMessage() {
    if (IsFantasticTextActive() || IsAwesomeTextActive()) {
        return;
    }
    
    mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Awesome));

    Start(*mAwesomeTextSceneObject);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.25f, 0.55f, UiLayer::text});
}

void SmallText::StartFantasticMessage() {
    if (IsFantasticTextActive()) {
        return;
    }
    
    if (!IsAwesomeTextActive()) {
        mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::Fantastic));
    }
    
    Start(*mFantasticTextSceneObject);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.6f, 0.55f, UiLayer::text});
}

void SmallText::StartWillUndoMessage() {
    Start(*mWillUndoTextSceneObject);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.6f, 0.55f, UiLayer::text});
}

void SmallText::StartUndoingMessage() {
    Start(*mUndoingTextSceneObject);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.0f, 0.55f, UiLayer::text});
}

void SmallText::StartNoRoomMessage() {
    if (mState != State::Inactive && mActiveTextSceneObject == mNoRoomTextSceneObject) {
        return;
    }

    Start(*mNoRoomTextSceneObject);
    mTwinkleParticleEffect->GetTransform().SetPosition({-3.0f, 0.55f, UiLayer::text});
}

void SmallText::Start(Pht::SceneObject& textSceneObject) {
    mContainerSceneObject->SetIsVisible(true);
    mContainerSceneObject->SetIsStatic(false);
    
    HideAllTextObjects();
    
    mActiveTextSceneObject = &textSceneObject;
    mActiveTextSceneObject->SetIsVisible(true);
    mActiveTextSceneObject->SetIsStatic(false);
    
    auto& textProperties =
        mActiveTextSceneObject->GetComponent<Pht::TextComponent>()->GetProperties();
    textProperties.mColor.w = 1.0f;
    textProperties.mSpecularColor.w = 1.0f;
    textProperties.mShadowColor.w = 1.0f;
    textProperties.mSecondShadowColor.w = textShadowAlpha;
    
    mState = State::ScalingIn;
    mElapsedTime = 0.0f;
    mContainerSceneObject->GetTransform().SetPosition(startPosition);
    Pht::SceneObjectUtils::ScaleRecursively(*mContainerSceneObject, 0.0f);
}

void SmallText::Update(float dt) {
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

void SmallText::UpdateInScalingInState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > scaleInDuration) {
        mState = State::DisplayingText;
        mElapsedTime = 0.0f;
        Pht::SceneObjectUtils::ScaleRecursively(*mContainerSceneObject, textScale);
        
        mTwinkleParticleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    } else {
        auto reversedNormalizedTime = 1.0f - (mElapsedTime / scaleInDuration);
        auto scale = (1.0f - Pht::Lerp(reversedNormalizedTime, scalePoints)) * textScale;

        Pht::SceneObjectUtils::ScaleRecursively(*mContainerSceneObject, scale);
    }
}

void SmallText::UpdateInDisplayingTextState(float dt) {
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

void SmallText::UpdateInSlidingOutState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > slideOutDuration) {
        mState = State::Inactive;
        mContainerSceneObject->SetIsVisible(false);
        mContainerSceneObject->SetIsStatic(true);
        HideAllTextObjects();
    } else {
        mVelocity += acceleration * dt;
        mTextSlide += mVelocity * dt;
        
        Pht::Vec3 position {startPosition.x, startPosition.y + mTextSlide, startPosition.z};
        mContainerSceneObject->GetTransform().SetPosition(position);
        
        auto alpha = (slideOutDuration - mElapsedTime) / slideOutDuration;
        auto& textProperties =
            mActiveTextSceneObject->GetComponent<Pht::TextComponent>()->GetProperties();
        textProperties.mColor.w = alpha;
        textProperties.mSpecularColor.w = alpha;
        textProperties.mShadowColor.w = alpha;
        textProperties.mSecondShadowColor.w = alpha * textShadowAlpha;
    }
}

void SmallText::HideAllTextObjects() {
    for (auto& textSceneObject: mTextSceneObjects) {
        textSceneObject->SetIsVisible(false);
        textSceneObject->SetIsStatic(true);
    }
}

bool SmallText::IsAwesomeTextActive() const {
    return mState != State::Inactive && mActiveTextSceneObject == mAwesomeTextSceneObject;
}

bool SmallText::IsFantasticTextActive() const {
    return mState != State::Inactive && mActiveTextSceneObject == mFantasticTextSceneObject;
}
