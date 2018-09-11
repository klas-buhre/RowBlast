#include "HandAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "Scene.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "GameScene.hpp"

namespace {
    constexpr auto moveTime {0.19f};
    constexpr auto stillTime {1.0f};
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

HandAnimation::HandAnimation(Pht::IEngine& engine, GameScene& scene) :
    mEngine {engine},
    mScene {scene} {}

void HandAnimation::Init() {
    mContainerSceneObject = &mScene.GetScene().CreateSceneObject();
    mScene.GetUiViewsContainer().AddChild(*mContainerSceneObject);
    
    Pht::Material handMaterial {"hand48.png"};
    handMaterial.SetBlend(Pht::Blend::Yes);
    mHandSceneObject = &mScene.GetScene().CreateSceneObject(Pht::QuadMesh {handSize, handSize},
                                                            handMaterial);
    mContainerSceneObject->AddChild(*mHandSceneObject);

    Pht::Material handShadowMaterial {"hand48.png", 0.0f, 0.0f, 0.0f, 0.0f};
    handShadowMaterial.SetOpacity(0.29f);
    handShadowMaterial.SetBlend(Pht::Blend::Yes);
    mHandShadowSceneObject = &mScene.GetScene().CreateSceneObject(Pht::QuadMesh {handShadowSize, handShadowSize},
                                                                  handShadowMaterial);
    mContainerSceneObject->AddChild(*mHandShadowSceneObject);

    CreateCircleParticleEffect();
    Stop();
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
    auto circleEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                       particleEmitterSettings,
                                                                       Pht::RenderMode::Triangles);
    circleEffect->GetTransform().SetPosition(circlePosition);
    mCircleEffect = circleEffect->GetComponent<Pht::ParticleEffect>();
    mContainerSceneObject->AddChild(*circleEffect);
    mScene.GetScene().AddSceneObject(std::move(circleEffect));
}

void HandAnimation::Start(const Pht::Vec3& position, float angle) {
    mContainerSceneObject->SetIsVisible(true);
    mContainerSceneObject->SetIsStatic(false);
    
    auto& transform {mContainerSceneObject->GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, angle});
    
    mHandShadowBackwardPosition = backwardPosition + RotateZ(shadowOffset, -angle);
    mHandShadowForwardPosition = shadowForwardPosition + RotateZ(shadowForwardOffset, -angle);

    GoToForwardState();
}

void HandAnimation::Update() {
    switch (mState) {
        case State::Inactive:
            return;
        case State::GoingForward:
            UpdateInGoingForwardState();
            break;
        case State::GoingBackward:
            UpdateInGoingBackwardState();
            break;
        case State::Still:
            UpdateInStillState();
            break;
    }
    
    mCircleEffect->Update(mEngine.GetLastFrameSeconds());
}

void HandAnimation::UpdateInGoingForwardState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    auto t {mElapsedTime / moveTime};
    auto& transform {mHandSceneObject->GetTransform()};
    transform.SetPosition(backwardPosition.Lerp(t, forwardPosition));
    transform.SetScale(1.0f + handUpScaleAdd * (1.0f - t));
    
    auto handShadowPosition {mHandShadowBackwardPosition.Lerp(t, mHandShadowForwardPosition)};
    mHandShadowSceneObject->GetTransform().SetPosition(handShadowPosition);
    
    if (mElapsedTime > moveTime) {
        mState = State::GoingBackward;
        transform.SetPosition(forwardPosition);
        transform.SetScale(1.0f);
        mHandShadowSceneObject->GetTransform().SetPosition(mHandShadowForwardPosition);
        mElapsedTime = 0.0f;
        
        mCircleEffect->Start();
    }
}

void HandAnimation::UpdateInGoingBackwardState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    auto t {mElapsedTime / moveTime};
    auto& transform {mHandSceneObject->GetTransform()};
    transform.SetPosition(forwardPosition.Lerp(t, backwardPosition));
    transform.SetScale(1.0f + handUpScaleAdd * t);
    
    auto handShadowPosition {mHandShadowForwardPosition.Lerp(t, mHandShadowBackwardPosition)};
    mHandShadowSceneObject->GetTransform().SetPosition(handShadowPosition);
    
    if (mElapsedTime > moveTime) {
        mState = State::Still;
        transform.SetPosition(backwardPosition);
        transform.SetScale(1.0 + handUpScaleAdd);
        mHandShadowSceneObject->GetTransform().SetPosition(mHandShadowBackwardPosition);
        
        mElapsedTime = 0.0f;
    }
}

void HandAnimation::UpdateInStillState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mElapsedTime > stillTime) {
        GoToForwardState();
    }
}

void HandAnimation::GoToForwardState() {
    mState = State::GoingForward;
    mElapsedTime = 0.0f;
    
    auto& transform {mHandSceneObject->GetTransform()};
    transform.SetPosition(backwardPosition);
    transform.SetScale(1.0 + handUpScaleAdd);
    
    mHandShadowSceneObject->GetTransform().SetPosition(mHandShadowBackwardPosition);
}

void HandAnimation::Stop() {
    mState = State::Inactive;
    mContainerSceneObject->SetIsVisible(false);
    mContainerSceneObject->SetIsStatic(true);
    mCircleEffect->Stop();
}

void HandAnimation::Hide() {
    mContainerSceneObject->SetIsVisible(false);
}

void HandAnimation::Unhide() {
    if (mState != State::Inactive) {
        mContainerSceneObject->SetIsVisible(true);
    }
}
