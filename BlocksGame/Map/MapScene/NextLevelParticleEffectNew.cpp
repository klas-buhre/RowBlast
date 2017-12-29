#include "NextLevelParticleEffectNew.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "IRenderer.hpp"
#include "MathUtils.hpp"
#include "SceneObject.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSystem.hpp"

using namespace BlocksGame;

void BlocksGame::CreateNextLevelParticleEffect(Pht::IEngine& engine,
                                               Pht::Scene& scene,
                                               const Pht::Vec3& position,
                                               int layer) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = {0.0f, 0.7f, 0.0f},
        .mSize = {1.7f, 0.85f, 1.7f},
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mFrequency = 5.0f
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 1.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "star_particle.png",
        .mTimeToLive = 2.0f,
        .mTimeToLiveRandomPart = 0.4f,
        .mFadeOutDuration = 1.2f,
        .mZAngularVelocityRandomPart = 200.0f,
        .mSize = 1.0f,
        .mSizeRandomPart = 0.5f,
        .mShrinkDuration = 1.2f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    auto sceneObject = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                      particleEmitterSettings,
                                                                      Pht::RenderMode::Triangles);
    sceneObject->GetTransform().SetPosition(position);
    sceneObject->SetLayer(layer);
    
    auto* particleEffect {sceneObject->GetComponent<Pht::ParticleEffect>()};
    assert(particleEffect);
    particleSystem.AddParticleEffect(*particleEffect);
    particleEffect->Start();
    
    scene.GetRoot().AddChild(*sceneObject);
    scene.AddSceneObject(std::move(sceneObject));
}
