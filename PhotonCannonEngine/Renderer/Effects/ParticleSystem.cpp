#include "ParticleSystem.hpp"

#include "IEngine.hpp"

using namespace Pht;

namespace {
    bool UpdateParticle(Particle& particle, float dt, const ParticleSettings& particleSettings) {
        particle.mAge += dt;
        
        if (particle.mAge > particle.mTimeToLive) {
            particle.mIsActive = false;
            return false;
        }
        
        particle.mVelocity += particleSettings.mAcceleration * dt;
        particle.mPosition += particle.mVelocity * dt;
        
        if (particle.mAge > particle.mTimeToLive - particleSettings.mFadeOutDuration) {
            particle.mColor.w -= dt / particleSettings.mFadeOutDuration;
            if (particle.mColor.w < 0.0f) {
                particle.mColor.w = 0.0f;
            }
        }
        
        if (particle.mAge > particle.mTimeToLive - particleSettings.mShrinkDuration) {
            particle.mSize -= particle.mOriginalSize * dt / particleSettings.mShrinkDuration;
            if (particle.mSize < 0.0f) {
                particle.mSize = 0.0f;
            }
        }

        return true;
    }
    
    int CalculateNumParticles(const EmitterSettings& emitterSettings,
                              const ParticleSettings& particleSettings) {
        return emitterSettings.mFrequency *
               (particleSettings.mTimeToLive + particleSettings.mTimeToLiveRandomPart) * 2.0f +
               emitterSettings.mBurst;
    }
}

ParticleSystem::ParticleSystem(IEngine& engine,
                               const ParticleSettings& particleSettings,
                               const EmitterSettings& emitterSettings) :
    mVertexBuffer {
        CalculateNumParticles(emitterSettings, particleSettings),
        0,
        {.mColors = true, .mPointSizes = true}
    },
    mEngine {engine},
    mEmitter {engine, particleSettings, emitterSettings} {
    
    mParticles.resize(CalculateNumParticles(emitterSettings, particleSettings));
    
    Material material {particleSettings.mTextureFilename};
    material.SetShaderType(ShaderType::Particle);
    mRenderableObject = std::make_unique<RenderableObject>(material);
}

void ParticleSystem::Start() {
    mEmitter.Start();
    mIsActive = true;
    
    for (auto& particle: mParticles) {
        particle.mIsActive = false;
    }
}

bool ParticleSystem::Update() {
    if (!mIsActive) {
        return false;
    }

    mEmitter.Update(mParticles);
    auto dt {mEngine.GetLastFrameSeconds()};
    auto anyActiveParticles {false};
    auto& particleSettings {mEmitter.GetParticleSettings()};
    
    for (auto& particle: mParticles) {
        if (!particle.mIsActive) {
            continue;
        }
        
        if (UpdateParticle(particle, dt, particleSettings)) {
            anyActiveParticles = true;
        }
    }
    
    if (!anyActiveParticles) {
        mIsActive = false;
    } else {
        WriteVertexBuffer();
    }
    
    return mIsActive;
}

const RenderableObject* ParticleSystem::GetRenderableObject() const {
    if (mIsActive) {
        return mRenderableObject.get();
    }
    
    return nullptr;
}

void ParticleSystem::WriteVertexBuffer() {
    mVertexBuffer.Reset();
    
    for (auto& particle: mParticles) {
        if (particle.mIsActive) {
            mVertexBuffer.Write(particle.mPosition, particle.mColor, particle.mSize);
        }
    }
    
    mRenderableObject->UploadPoints(mVertexBuffer);
}
