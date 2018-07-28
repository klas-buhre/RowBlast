#include "HandAnimation.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "SceneObject.hpp"
#include "Scene.hpp"
#include "ParticleEffect.hpp"
#include "IParticleSystem.hpp"

// Game includes.
#include "GameScene.hpp"

namespace {
    constexpr auto moveTime {0.19f};
    constexpr auto stillTime {1.0f};
    constexpr auto handSize {2.2f};
    const Pht::Vec3 backwardPosition {0.0f, -1.2f, 0.0f};
    const Pht::Vec3 forwardPosition {0.0f, 0.0f, 0.0f};
    const Pht::Vec3 circlePosition {forwardPosition + Pht::Vec3{-0.17f, 0.75f, -0.1f}};
}

using namespace RowBlast;

HandAnimation::HandAnimation(Pht::IEngine& engine, GameScene& scene) :
    mEngine {engine},
    mScene {scene} {}

void HandAnimation::Init() {
    mContainerSceneObject = &mScene.GetScene().CreateSceneObject();
    mScene.GetUiViewsContainer().AddChild(*mContainerSceneObject);
    
    Pht::Material handMaterial {"hand10.png"};
    handMaterial.SetBlend(Pht::Blend::Yes);
    mHandSceneObject = &mScene.GetScene().CreateSceneObject(Pht::QuadMesh {handSize, handSize},
                                                            handMaterial);
    mContainerSceneObject->AddChild(*mHandSceneObject);
    
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
    mHandSceneObject->GetTransform().SetPosition(backwardPosition.Lerp(t, forwardPosition));
    
    if (mElapsedTime > moveTime) {
        mState = State::GoingBackward;
        mHandSceneObject->GetTransform().SetPosition(forwardPosition);
        mElapsedTime = 0.0f;
        
        mCircleEffect->Start();
    }
}

void HandAnimation::UpdateInGoingBackwardState() {
    mElapsedTime += mEngine.GetLastFrameSeconds();
    
    auto t {mElapsedTime / moveTime};
    mHandSceneObject->GetTransform().SetPosition(forwardPosition.Lerp(t, backwardPosition));
    
    if (mElapsedTime > moveTime) {
        mState = State::Still;
        mHandSceneObject->GetTransform().SetPosition(backwardPosition);
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
    mHandSceneObject->GetTransform().SetPosition(backwardPosition);
    mElapsedTime = 0.0f;
}

void HandAnimation::Stop() {
    mState = State::Inactive;
    mContainerSceneObject->SetIsVisible(false);
    mContainerSceneObject->SetIsStatic(true);
    mCircleEffect->Stop();
}
