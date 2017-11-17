#ifndef ParticleEffect_hpp
#define ParticleEffect_hpp

#include <vector>
#include <memory>

#include "VertexBuffer.hpp"
#include "Matrix.hpp"
#include "ParticleEmitter.hpp"
#include "RenderableObject.hpp"
#include "ISceneObjectComponent.hpp"
#include "Fnv1Hash.hpp"

namespace Pht {
    class IEngine;
    
    class ParticleEffect: public ISceneObjectComponent {
    public:
        static constexpr ComponentId id {Hash::Fnv1a("ParticleEffect")};
        
        ParticleEffect(IEngine& engine,
                       const ParticleSettings& particleSettings,
                       const EmitterSettings& emitterSettings,
                       RenderMode renderMode);
        
        void Update() override;
        
        void Start();
        const RenderableObject* GetRenderableObject() const;
        
        bool IsActive() const {
            return mIsActive;
        }
        
        ParticleEmitter& GetEmitter() {
            return mEmitter;
        }
        
    private:
        void WriteVertexBuffer();
        void WritePoints();
        void WriteTriangles();
        void WriteParticleTriangles(const Particle& particle);
        
        IEngine& mEngine;
        ParticleEmitter mEmitter;
        RenderMode mRenderMode {RenderMode::Triangles};
        std::vector<Particle> mParticles;
        std::unique_ptr<VertexBuffer> mVertexBuffer;
        std::unique_ptr<RenderableObject> mRenderableObject;
        bool mIsActive {false};
    };
    
    struct ParticleEffect_ {
        Mat4 mTransform;
        std::unique_ptr<ParticleEffect> mParticleSystem;
    };
}

#endif
