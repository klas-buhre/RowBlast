#include "SunParticleEffect.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "ParticleEffect.hpp"
#include "ParticleSystem.hpp"

using namespace RowBlast;

namespace {
    void CreateParticleEffect(Pht::IEngine& engine,
                              Pht::Scene& scene,
                              const Pht::Vec3& position,
                              int layer,
                              const Pht::ParticleSettings& particleSettings) {
         Pht::EmitterSettings particleEmitterSettings {
            .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
            .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
            .mTimeToLive = 0.0f,
            .mFrequency = 0.0f,
            .mBurst = 1
        };
        
        auto& particleSystem {engine.GetParticleSystem()};
        auto sceneObject {
            particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                           particleEmitterSettings,
                                                           Pht::RenderMode::Triangles)
        };
        sceneObject->GetTransform().SetPosition(position);
        sceneObject->SetLayer(layer);
        
        auto& material {sceneObject->GetRenderable()->GetMaterial()};
        material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);

        auto* particleEffect {sceneObject->GetComponent<Pht::ParticleEffect>()};
        assert(particleEffect);
        particleSystem.AddParticleEffect(*particleEffect);
        particleEffect->Start();

        scene.GetRoot().AddChild(*sceneObject);
        scene.AddSceneObject(std::move(sceneObject));
    }
}

void RowBlast::CreateSunParticleEffect(Pht::IEngine& engine,
                                       Pht::Scene& scene,
                                       const Sun& sun,
                                       int layer) {
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = 5.0f,
        .mSize = sun.mSize,
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    CreateParticleEffect(engine, scene, sun.mPosition, layer, particleSettings);

    Pht::ParticleSettings flareParticleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.5f, 0.5f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare03.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocity = 0.0f,
        .mSize = sun.mSize * 0.5f,
        .mSizeRandomPart = 0.0f,
        .mGrowDuration = 0.0f,
        .mShrinkDuration = 0.0f
    };
    
    CreateParticleEffect(engine, scene, sun.mPosition, layer, flareParticleSettings);
}
