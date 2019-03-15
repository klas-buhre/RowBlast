#include "HandAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "ISceneManager.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"
#include "MathUtils.hpp"
#include "QuadMesh.hpp"

namespace {
    constexpr auto moveDuration {0.19f};
    constexpr auto handSize {1.65f};
    constexpr auto handUpScaleAdd {0.5f};
    constexpr auto handShadowSize {2.15f};
    const Pht::Vec3 backwardPosition {0.2f, -1.3f, 0.0f};
    const Pht::Vec3 forwardPosition {0.1f, 0.05f, 0.0f};
    const Pht::Vec3 circlePosition {-0.17f, 0.75f, -0.1f};
    const Pht::Vec3 shadowOffset {-0.5f, -0.5f, -0.1f};
    const Pht::Vec3 shadowForwardOffset {-0.03f, -0.03f, -0.1f};
    
    const Pht::Vec3 shadowForwardPosition {
        forwardPosition.x,
        forwardPosition.y - 0.2f,
        forwardPosition.z
    };
    
    Pht::Vec3 RotateZ(const Pht::Vec3& v, float theta) {
        auto thetaRadians {Pht::ToRadians(theta)};
        auto sinTheta {std::sin(thetaRadians)};
        auto cosTheta {std::cos(thetaRadians)};

        return {v.x * cosTheta - v.y * sinTheta, v.x * sinTheta + v.y * cosTheta, v.z};
    }
}

using namespace RowBlast;

HandAnimation::HandAnimation(Pht::IEngine& engine, float scale, bool useShadow) :
    mEngine {engine} {
    
    auto& sceneManager {mEngine.GetSceneManager()};
    
    mContainerSceneObject = std::make_unique<Pht::SceneObject>();
    mContainerSceneObject->GetTransform().SetScale(scale);
    
    Pht::Material handMaterial {"hand48.png"};
    handMaterial.SetBlend(Pht::Blend::Yes);
    mHandSceneObject = sceneManager.CreateSceneObject(Pht::QuadMesh {handSize, handSize},
                                                      handMaterial,
                                                      mSceneResources);
    mContainerSceneObject->AddChild(*mHandSceneObject);

    if (useShadow) {
        Pht::Material handShadowMaterial {"hand48.png", 0.0f, 0.0f, 0.0f, 0.0f};
        handShadowMaterial.SetOpacity(0.29f);
        handShadowMaterial.SetBlend(Pht::Blend::Yes);
        mHandShadowSceneObject = sceneManager.CreateSceneObject(Pht::QuadMesh {handShadowSize, handShadowSize},
                                                                handShadowMaterial,
                                                                mSceneResources);
        mContainerSceneObject->AddChild(*mHandShadowSceneObject);
    }

    CreateCircleParticleEffect();
}

void HandAnimation::CreateCircleParticleEffect() {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 1
    };

    Pht::ParticleSettings particleSettings {
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "particle_sprite_halo.png",
        .mTimeToLive = 0.35f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.35f,
        .mSize = Pht::Vec2{1.3f, 1.3f},
        .mSizeRandomPart = 0.0f,
        .mInitialSize = Pht::Vec2{0.65f, 0.65f},
        .mGrowDuration = 0.35f,
        .mShrinkDuration = 0.0f
    };

    auto& particleSystem {mEngine.GetParticleSystem()};
    mCircleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                   particleEmitterSettings,
                                                                   Pht::RenderMode::Triangles);
    mCircleEffect->GetTransform().SetPosition(circlePosition);
    mContainerSceneObject->AddChild(*mCircleEffect);
}

void HandAnimation::Start(const Pht::Vec3& position, float angle, State initialState) {
    mContainerSceneObject->SetIsVisible(true);
    mContainerSceneObject->SetIsStatic(false);
    
    auto& transform {mContainerSceneObject->GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, angle});
    
    mHandShadowBackwardPosition = backwardPosition + RotateZ(shadowOffset, -angle);
    mHandShadowForwardPosition = shadowForwardPosition + RotateZ(shadowForwardOffset, -angle);
    
    mState = initialState;
    mElapsedTime = 0.0f;
    
    SetHandAtBackwardPosition();
}

void HandAnimation::Init(Pht::SceneObject& parentObject) {
    parentObject.AddChild(*mContainerSceneObject);
    Stop();
}

void HandAnimation::Start(const Pht::Vec3& position, float angle) {
    Start(position, angle, State::GoingForwardToScreen);
}

void HandAnimation::StartInNotTouchingScreenState(const Pht::Vec3& position,
                                                  float angle,
                                                  float notTouchingDuration) {
    mNotTouchingDuration = notTouchingDuration;
    Start(position, angle, State::NotTouchingScreen);
}

void HandAnimation::BeginTouch(float touchDuration) {
    mTouchDuration = touchDuration;
    mState = State::GoingForwardToScreen;
    mElapsedTime = 0.0f;
    SetHandAtBackwardPosition();
}

void HandAnimation::Update() {
    switch (mState) {
        case State::Inactive:
            return;
        case State::GoingForwardToScreen:
            UpdateInGoingForwardState();
            break;
        case State::TouchingScreen:
            UpdateInTouchingScreenState();
            break;
        case State::GoingBackwardFromScreen:
            UpdateInGoingBackwardState();
            break;
        case State::NotTouchingScreen:
            UpdateInNotTouchingScreenState();
            break;
    }
    
    mCircleEffect->GetComponent<Pht::ParticleEffect>()->Update(mEngine.GetLastFrameSeconds());
}

void HandAnimation::UpdateInGoingForwardState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    auto t {mElapsedTime / moveDuration};
    auto& transform {mHandSceneObject->GetTransform()};
    transform.SetPosition(backwardPosition.Lerp(t, forwardPosition));
    transform.SetScale(1.0f + handUpScaleAdd * (1.0f - t));
    
    if (mHandShadowSceneObject) {
        auto handShadowPosition {mHandShadowBackwardPosition.Lerp(t, mHandShadowForwardPosition)};
        mHandShadowSceneObject->GetTransform().SetPosition(handShadowPosition);
    }
    
    if (mElapsedTime > moveDuration) {
        mState = State::TouchingScreen;
        transform.SetPosition(forwardPosition);
        transform.SetScale(1.0f);
        
        if (mHandShadowSceneObject) {
            mHandShadowSceneObject->GetTransform().SetPosition(mHandShadowForwardPosition);
        }
        
        mElapsedTime = 0.0f;
        
        mCircleEffect->GetComponent<Pht::ParticleEffect>()->Start();
    }
}

void HandAnimation::UpdateInTouchingScreenState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    auto& circleEffect {*mCircleEffect->GetComponent<Pht::ParticleEffect>()};
    if (!circleEffect.IsActive()) {
        circleEffect.Start();
    }
    
    if (mElapsedTime > mTouchDuration) {
        mState = State::GoingBackwardFromScreen;
        mElapsedTime = 0.0f;
    }
}

void HandAnimation::UpdateInGoingBackwardState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    auto t {mElapsedTime / moveDuration};
    auto& transform {mHandSceneObject->GetTransform()};
    transform.SetPosition(forwardPosition.Lerp(t, backwardPosition));
    transform.SetScale(1.0f + handUpScaleAdd * t);
    
    if (mHandShadowSceneObject) {
        auto handShadowPosition {mHandShadowForwardPosition.Lerp(t, mHandShadowBackwardPosition)};
        mHandShadowSceneObject->GetTransform().SetPosition(handShadowPosition);
    }

    if (mElapsedTime > moveDuration) {
        mState = State::NotTouchingScreen;
        transform.SetPosition(backwardPosition);
        transform.SetScale(1.0 + handUpScaleAdd);
        
        if (mHandShadowSceneObject) {
            mHandShadowSceneObject->GetTransform().SetPosition(mHandShadowBackwardPosition);
        }
        
        mElapsedTime = 0.0f;
    }
}

void HandAnimation::UpdateInNotTouchingScreenState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedTime > mNotTouchingDuration) {
        mState = State::GoingForwardToScreen;
        mElapsedTime = 0.0f;
        SetHandAtBackwardPosition();
    }
}

void HandAnimation::SetHandAtBackwardPosition() {
    auto& transform {mHandSceneObject->GetTransform()};
    transform.SetPosition(backwardPosition);
    transform.SetScale(1.0 + handUpScaleAdd);
    
    if (mHandShadowSceneObject) {
        mHandShadowSceneObject->GetTransform().SetPosition(mHandShadowBackwardPosition);
    }
}

void HandAnimation::Stop() {
    mState = State::Inactive;
    mContainerSceneObject->SetIsVisible(false);
    mContainerSceneObject->SetIsStatic(true);
    mCircleEffect->GetComponent<Pht::ParticleEffect>()->Stop();
}

void HandAnimation::Hide() {
    mContainerSceneObject->SetIsVisible(false);
}

void HandAnimation::Unhide() {
    if (mState != State::Inactive) {
        mContainerSceneObject->SetIsVisible(true);
    }
}
