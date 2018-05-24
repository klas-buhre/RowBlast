#include "StarsAnimation.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "ObjMesh.hpp"
#include "ISceneManager.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto scaleTime {0.38f};
    constexpr auto initialScale {7.0f};
    constexpr auto starRadius {3.0f};
    constexpr auto farZPosition {UiLayer::block - starRadius * initialScale};
    constexpr auto zRotationSpeed {12.0f};
    constexpr auto yRotation {90.0f};
    constexpr auto flashTime {0.22f};
    constexpr auto glowTime {2.0f};
    constexpr auto glowScaleDownTime {0.3f};
    const Pht::Color flashingColorAdd {0.2f, 0.2f, 0.2f};
    const Pht::Vec3 leftMostStarPosition {-3.8f, 1.3f, farZPosition};
    const Pht::Vec3 leftStarPosition {-2.0f, 1.3f, farZPosition};
    const Pht::Vec3 middleStarPosition {0.0f, 1.8f, farZPosition};
    const Pht::Vec3 rightStarPosition {2.0f, 1.3f, farZPosition};
    const Pht::Vec3 rightMostStarPosition {3.8f, 1.3f, farZPosition};
    const Pht::Vec3 shadowOffset {-0.3f, -0.3f, -0.3f};
}

StarsAnimation::StarsAnimation(Pht::IEngine& engine,
                               GameScene& scene,
                               const CommonResources& commonResources) :
    mEngine {engine},
    mScene {scene} {
    
    auto& sceneManager {mEngine.GetSceneManager()};
    Pht::Material shaddowMaterial {Pht::Color{0.4f, 0.4f, 0.4f}};
    shaddowMaterial.SetOpacity(0.075f);
    mShadowRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"star_1428.obj", 0.26f},
                                                            shaddowMaterial);

    for (auto& star: mStarAnimations) {
        star = std::make_unique<StarAnimation>(engine, *mShadowRenderable, commonResources);
    }
}

void StarsAnimation::Init() {
    auto& starsContainer {mScene.GetStarsContainer()};
    starsContainer.SetIsVisible(false);
    starsContainer.SetIsStatic(true);

    auto& shadowsContainer {mScene.GetStarShadowsContainer()};
    shadowsContainer.SetIsVisible(false);
    shadowsContainer.SetIsStatic(true);
    
    for (auto& star: mStarAnimations) {
        star->Init(starsContainer, shadowsContainer);
    }
}

void StarsAnimation::Start(int numStars) {
    mNumStars = numStars;

    auto& starsContainer {mScene.GetStarsContainer()};
    starsContainer.SetIsVisible(true);
    starsContainer.SetIsStatic(false);

    auto& shadowsContainer {mScene.GetStarShadowsContainer()};
    shadowsContainer.SetIsVisible(true);
    shadowsContainer.SetIsStatic(false);

    switch (numStars) {
        case 1:
            mStarAnimations[0]->Start(middleStarPosition, 0.0f);
            break;
        case 2:
            mStarAnimations[0]->Start(leftStarPosition, 0.0f);
            mStarAnimations[1]->Start(rightStarPosition, scaleTime);
            break;
        case 3:
            mStarAnimations[0]->Start(leftMostStarPosition, 0.0f);
            mStarAnimations[1]->Start(middleStarPosition, scaleTime);
            mStarAnimations[2]->Start(rightMostStarPosition, scaleTime * 2.0f);
            break;
        default:
            assert(!"Unsupported number of stars");
            break;
    }
}

StarsAnimation::State StarsAnimation::Update() {
    State state {State::Rotating};
    auto dt {mEngine.GetLastFrameSeconds()};
    
    for (auto i {0}; i < mNumStars; ++i) {
        if (mStarAnimations[i]->Update(dt) != StarAnimation::State::Rotating) {
            state = State::Ongoing;
        }
    }
    
    return state;
}

void StarsAnimation::ShowStarShadows() {
    for (auto i {0}; i < mNumStars; ++i) {
        mStarAnimations[i]->ShowShadow();
    }
}

StarsAnimation::StarAnimation::StarAnimation(Pht::IEngine& engine,
                                             Pht::RenderableObject& shadowRenderable,
                                             const CommonResources& commonResources) :
    mGoldStarMaterial {commonResources.GetMaterials().GetGoldMaterial()} {

    auto& sceneManager {engine.GetSceneManager()};
    mStarRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"star_1428.obj", 0.26f},
                                                          mGoldStarMaterial);
    mStar = std::make_unique<Pht::SceneObject>(mStarRenderable.get());
    mShadow = std::make_unique<Pht::SceneObject>(&shadowRenderable);
    
    CreateGlowParticleEffect(engine);
}

void StarsAnimation::StarAnimation::CreateGlowParticleEffect(Pht::IEngine& engine) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mBurst = 1
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.5f, 0.5f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "glow_lines.png",
        .mTimeToLive = 3.0f,
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 20.0f,
        .mSize = Pht::Vec2{11.5f, 11.5f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.5f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                 particleEmitterSettings,
                                                                 Pht::RenderMode::Triangles);
    mGlowEffect->GetRenderable()->GetMaterial().GetDepthState().mDepthTest = true;
}

void StarsAnimation::StarAnimation::Init(Pht::SceneObject& starsContainer,
                                         Pht::SceneObject& shadowsContainer) {
    mStar->SetIsVisible(false);
    starsContainer.AddChild(*mStar);
    starsContainer.AddChild(*mGlowEffect);
    mShadow->SetIsVisible(false);
    shadowsContainer.AddChild(*mShadow);
    
    auto* particleEffect {mGlowEffect->GetComponent<Pht::ParticleEffect>()};
    assert(particleEffect);

    particleEffect->Stop();
}

void StarsAnimation::StarAnimation::Start(const Pht::Vec3& position, float waitTime) {
    auto& starTransform {mStar->GetTransform()};
    starTransform.SetPosition(position);
    starTransform.SetRotation({yRotation, 0.0f, 0.0f});
    
    auto glowPosition {position};
    glowPosition.z -= starRadius;
    auto& glowTransform {mGlowEffect->GetTransform()};
    glowTransform.SetPosition(glowPosition);
    glowTransform.SetScale(1.0f);

    auto shadowPosition {position};
    shadowPosition.z = UiLayer::block;
    mShadow->GetTransform().SetPosition(shadowPosition + shadowOffset);

    mState = State::Waiting;
    mElapsedTime = 0.0f;
    mWaitTime = waitTime;
    mStarZAngle = 0.0f;
    
    SetIsFlashing(false);
}

void StarsAnimation::StarAnimation::ShowShadow() {
    mShadow->SetIsVisible(true);
    
    auto& transform {mStar->GetTransform()};
    auto starPosition {transform.GetPosition()};
    starPosition.z = UiLayer::block;
    transform.SetPosition(starPosition);
}

StarsAnimation::StarAnimation::State StarsAnimation::StarAnimation::Update(float dt) {
    switch (mState) {
        case State::Waiting:
            UpdateInWaitingState(dt);
            break;
        case State::ScalingIn:
            UpdateInScalingInState(dt);
            break;
        case State::Flashing:
            UpdateInFlashingState(dt);
            break;
        case State::RotatingAndGlowing:
            UpdateInRotatingAndGlowingState(dt);
            break;
        case State::Rotating:
            UpdateInRotatingState(dt);
            break;
    }
    
    return mState;
}

void StarsAnimation::StarAnimation::UpdateInWaitingState(float dt) {
    mElapsedTime += dt;
    if (mElapsedTime > mWaitTime) {
        mState = State::ScalingIn;
        mElapsedTime = 0.0f;
        mStar->SetIsVisible(true);
        mStar->GetRenderable()->GetMaterial().SetOpacity(0.0f);
    }
}

void StarsAnimation::StarAnimation::UpdateInScalingInState(float dt) {
    mElapsedTime += dt;
    auto normalizedTime {(scaleTime - mElapsedTime) / scaleTime};
    
    auto& transform {mStar->GetTransform()};
    transform.SetScale(1.0f + initialScale * normalizedTime * normalizedTime);
    transform.SetRotation({0.0f, yRotation * normalizedTime, 0.0f});
    
    auto position {transform.GetPosition()};
    position.z = UiLayer::block - starRadius * initialScale * normalizedTime * normalizedTime;
    transform.SetPosition(position);
    
    mStar->GetRenderable()->GetMaterial().SetOpacity(mElapsedTime / scaleTime);
    
    if (mElapsedTime > scaleTime) {
        mState = State::Flashing;
        mElapsedTime = 0.0f;
        SetIsFlashing(true);
        
        mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
        
        transform.SetScale(1.0f);
        transform.SetRotation({0.0f, 0.0f, 0.0f});
        position.z = farZPosition;
        transform.SetPosition(position);
        
        auto& material {mStar->GetRenderable()->GetMaterial()};
        material.SetOpacity(1.0f);
        material.GetDepthState().mDepthWrite = true;
    }
}

void StarsAnimation::StarAnimation::UpdateInFlashingState(float dt) {
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    mElapsedTime += dt;

    if (mElapsedTime > flashTime) {
        mState = State::RotatingAndGlowing;
        mElapsedTime = 0.0f;
        SetIsFlashing(false);
    }
}

void StarsAnimation::StarAnimation::UpdateInRotatingAndGlowingState(float dt) {
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    UpdateRotation(dt);
    mElapsedTime += dt;

    if (mElapsedTime > glowTime - mWaitTime) {
        mState = State::Rotating;
        mElapsedTime = 0.0f;
    }
}

void StarsAnimation::StarAnimation::UpdateInRotatingState(float dt) {
    mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    UpdateRotation(dt);
    mElapsedTime += dt;
    
    if (mElapsedTime < glowScaleDownTime) {
        mGlowEffect->GetTransform().SetScale((glowScaleDownTime - mElapsedTime) / glowScaleDownTime);
    }
}

void StarsAnimation::StarAnimation::UpdateRotation(float dt) {
    Pht::Vec3 rotation {0.0f, 0.0f, mStarZAngle};
    mStar->GetTransform().SetRotation(rotation);
    mShadow->GetTransform().SetRotation(rotation);

    mStarZAngle -= zRotationSpeed * dt;
}

void StarsAnimation::StarAnimation::SetIsFlashing(bool isFlashing) {
    auto& renderableMaterial {mStar->GetRenderable()->GetMaterial()};
    const auto& ambient {mGoldStarMaterial.GetAmbient()};
    const auto& diffuse {mGoldStarMaterial.GetDiffuse()};
    const auto& specular {mGoldStarMaterial.GetSpecular()};
    
    if (isFlashing) {
        renderableMaterial.SetAmbient(ambient + flashingColorAdd);
        renderableMaterial.SetDiffuse(diffuse + flashingColorAdd);
        renderableMaterial.SetSpecular(specular + flashingColorAdd);
    } else {
        renderableMaterial.SetAmbient(ambient);
        renderableMaterial.SetDiffuse(diffuse);
        renderableMaterial.SetSpecular(specular);
    }
}
