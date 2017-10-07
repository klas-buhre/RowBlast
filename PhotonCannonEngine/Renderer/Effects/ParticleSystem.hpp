#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <vector>
#include <memory>

#include "VertexBuffer.hpp"
#include "Matrix.hpp"
#include "ParticleEmitter.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    
    class ParticleSystem {
    public:
        ParticleSystem(IEngine& engine,
                       const ParticleSettings& particleSettings,
                       const EmitterSettings& emitterSettings);
        
        void Start();
        bool Update();
        const RenderableObject* GetRenderableObject() const;
        
        bool IsActive() const {
            return mIsActive;
        }
        
        ParticleEmitter& GetEmitter() {
            return mEmitter;
        }
        
    private:
        void WriteVertexBuffer();
        
        std::vector<Particle> mParticles;
        VertexBuffer mVertexBuffer;
        IEngine& mEngine;
        ParticleEmitter mEmitter;
        std::unique_ptr<RenderableObject> mRenderableObject;
        bool mIsActive {false};
    };
    
    struct ParticleEffect {
        Mat4 mTransform;
        std::unique_ptr<ParticleSystem> mParticleSystem;
    };
}

#endif
