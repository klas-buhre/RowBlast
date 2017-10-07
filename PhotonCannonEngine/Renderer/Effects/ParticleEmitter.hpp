#ifndef ParticleEmitter_hpp
#define ParticleEmitter_hpp

#include <vector>
#include <functional>

#include "Vector.hpp"

namespace Pht {
    struct EmitterSettings {
        Vec3 mPosition {0.0f, 0.0f, 0.0f};
        Vec3 mSize {0.0f, 0.0f, 0.0f};
        float mTimeToLive {2.0f};
        float mFrequency {10.0f};
        int mBurst {0};
    };
    
    struct ParticleSettings {
        std::function<Vec3()> mVelocityFunction;
        Vec3 mVelocity {0.0f, 0.0f, 0.0f};
        Vec3 mVelocityRandomPart {0.0f, 0.0f, 0.0f};
        Vec3 mAcceleration {0.0f, 0.0f, 0.0f};
        Vec4 mColor {1.0f, 1.0f, 1.0f, 1.0f};
        Vec4 mColorRandomPart {1.0f, 1.0f, 1.0f, 1.0f};
        std::string mTextureFilename;
        float mTimeToLive {2.0f};
        float mTimeToLiveRandomPart {0.5f};
        float mFadeOutDuration {1.0f};
        int mSize {28};            // In pixels.
        int mSizeRandomPart {10};  // In pixels.
        float mShrinkDuration {1.0f};
    };
    
    struct Particle {
        Vec3 mPosition;
        Vec3 mVelocity;
        Vec4 mColor;
        float mSize;
        float mOriginalSize;
        float mAge;
        float mTimeToLive;
        bool mIsActive {false};
    };
    
    class IEngine;
    
    class ParticleEmitter {
    public:
        ParticleEmitter(IEngine& engine,
                        const ParticleSettings& particleSettings,
                        const EmitterSettings& emitterSettings);
        
        void Start();
        void Update(std::vector<Particle>& particles);
        
        ParticleSettings& GetParticleSettings() {
            return mParticleSettings;
        }
        
    private:
        Particle* FindInactiveParticle(std::vector<Particle>& particles) const;
        void EmitParticle(Particle& particle);
        void EmitBurst(std::vector<Particle>& particles);
    
        IEngine& mEngine;
        ParticleSettings mParticleSettings;
        EmitterSettings mEmitterSettings;
        float mAge {0.0f};
        float mTimeSinceLastSpawn {0.0f};
    };
}

#endif
