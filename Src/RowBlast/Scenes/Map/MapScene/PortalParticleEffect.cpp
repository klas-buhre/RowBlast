#include "PortalParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSystem.hpp"

using namespace RowBlast;

void RowBlast::CreatePortalParticleEffect(Pht::IEngine& engine,
                                          Pht::Scene& scene,
                                          const Pht::Vec3& position,
                                          int layer) {
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
        .mTextureFilename = "portal.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = -70.0f,
        .mSize = Pht::Vec2{3.0f, 3.0f},
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    auto& particleSystem = engine.GetParticleSystem();

    auto sceneObject =
        particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                       particleEmitterSettings,
                                                       Pht::RenderMode::Triangles);

    sceneObject->GetRenderable()->GetMaterial().SetShaderType(Pht::ShaderType::ParticleTextureColor);
    sceneObject->GetTransform().SetPosition(position);
    sceneObject->SetLayer(layer);

    auto* particleEffect = sceneObject->GetComponent<Pht::ParticleEffect>();
    assert(particleEffect);
    particleSystem.AddParticleEffect(*particleEffect);
    particleEffect->Start();

    scene.GetRoot().AddChild(*sceneObject);
    scene.AddSceneObject(std::move(sceneObject));
}
