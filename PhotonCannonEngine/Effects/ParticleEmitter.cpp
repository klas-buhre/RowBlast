#include "ParticleEmitter.hpp"

#include "IEngine.hpp"
#include "MathUtils.hpp"

using namespace Pht;

ParticleEmitter::ParticleEmitter(IEngine& engine,
                                 const ParticleSettings& particleSettings,
                                 const EmitterSettings& emitterSettings) :
    mEngine {engine},
    mParticleSettings {particleSettings},
    mEmitterSettings {emitterSettings} {}

void ParticleEmitter::Start() {
    mAge = 0.0f;
    mTimeSinceLastSpawn = 0.0f;
}

void ParticleEmitter::Update(std::vector<Particle>& particles) {
    if (mAge == 0.0f) {
        if (mEmitterSettings.mBurst > 0) {
            EmitBurst(particles);
        } else {
            auto* particle {FindInactiveParticle(particles)};
            if (particle) {
                EmitParticle(*particle);
            }
        }
    }
    
    auto dt {mEngine.GetLastFrameSeconds()};
    mAge += dt;
    mTimeSinceLastSpawn += dt;
    
    if (mEmitterSettings.mTimeToLive != std::numeric_limits<float>::infinity()) {
        if (mAge > mEmitterSettings.mTimeToLive) {
            return;
        }
    }
    
    if (mTimeSinceLastSpawn > 1.0f / mEmitterSettings.mFrequency) {
        auto* particle {FindInactiveParticle(particles)};
        if (particle) {
            EmitParticle(*particle);
        }
    }
}

Particle* ParticleEmitter::FindInactiveParticle(std::vector<Particle>& particles) const {
    for (auto& particle: particles) {
        if (!particle.mIsActive) {
            return &particle;
        }
    }
    
    return nullptr;
}

void ParticleEmitter::EmitParticle(Particle& particle) {
    const auto& emitterPos {mEmitterSettings.mPosition};
    const auto& emitterSize {mEmitterSettings.mSize};
    
    Vec3 position {
        emitterPos.x + (NormalizedRand() - 0.5f) * emitterSize.x,
        emitterPos.y + (NormalizedRand() - 0.5f) * emitterSize.y,
        emitterPos.z + (NormalizedRand() - 0.5f) * emitterSize.z
    };
    
    const auto& settingsColor {mParticleSettings.mColor};
    const auto& colorRandomPart {mParticleSettings.mColorRandomPart};
    
    Vec4 color {
        settingsColor.x + (NormalizedRand() - 0.5f) * colorRandomPart.x,
        settingsColor.y + (NormalizedRand() - 0.5f) * colorRandomPart.y,
        settingsColor.z + (NormalizedRand() - 0.5f) * colorRandomPart.z,
        settingsColor.w + (NormalizedRand() - 0.5f) * colorRandomPart.w,
    };

    if (mParticleSettings.mVelocityFunction) {
        particle.mVelocity = mParticleSettings.mVelocityFunction();
    } else {
        const auto& settingsVelocity {mParticleSettings.mVelocity};
        const auto& velocityRandomPart {mParticleSettings.mVelocityRandomPart};
        
        Vec3 velocity {
            settingsVelocity.x + (NormalizedRand() - 0.5f) * velocityRandomPart.x,
            settingsVelocity.y + (NormalizedRand() - 0.5f) * velocityRandomPart.y,
            settingsVelocity.z + (NormalizedRand() - 0.5f) * velocityRandomPart.z,
        };
        
        particle.mVelocity = velocity;
    }
    
    particle.mPosition = position;
    particle.mColor = color;
    
    if (mParticleSettings.mSize.HasValue()) {
        assert(mParticleSettings.mSizeRandomPart.HasValue());
        particle.mFullSize = mParticleSettings.mSize.GetValue() +
                            (NormalizedRand() - 0.5f) * mParticleSettings.mSizeRandomPart.GetValue();
        particle.mSize = 0.0f;
    } else {
        assert(mParticleSettings.mPointSize.HasValue());
        assert(mParticleSettings.mPointSizeRandomPart.HasValue());
        particle.mFullSize = mParticleSettings.mPointSize.GetValue() +
                            (NormalizedRand() - 0.5f) * mParticleSettings.mPointSizeRandomPart.GetValue();
        particle.mSize = 0.0f;
    }
    
    particle.mAge = 0.0f;
    particle.mTimeToLive = mParticleSettings.mTimeToLive +
                           (NormalizedRand() - 0.5f) * mParticleSettings.mTimeToLiveRandomPart;
    
    if (mParticleSettings.mZAngularVelocity > 0.0f ||
        mParticleSettings.mZAngularVelocityRandomPart > 0.0f) {
        particle.mZAngle = NormalizedRand() * 360.0f;
        particle.mZAngularVelocity = mParticleSettings.mZAngularVelocity +
                                     (NormalizedRand() - 0.5f) * mParticleSettings.mZAngularVelocityRandomPart;
    } else {
        particle.mZAngle = 0.0f;
        particle.mZAngularVelocity = 0.0f;
    }

    particle.mIsActive = true;
    
    mTimeSinceLastSpawn = 0.0f;
}

void ParticleEmitter::EmitBurst(std::vector<Particle>& particles) {
    for (auto i {0}; i < mEmitterSettings.mBurst; ++i) {
        auto& particle {particles[i]};
        EmitParticle(particle);
    }
}
