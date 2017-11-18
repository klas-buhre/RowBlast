#ifndef ParticleEffect_hpp
#define ParticleEffect_hpp

#include <vector>
#include <memory>

#include "VertexBuffer.hpp"
#include "ParticleEmitter.hpp"
#include "RenderableObject.hpp"
#include "ISceneObjectComponent.hpp"

namespace Pht {
    class SceneObject;
    class IParticleSystem;
    
    class ParticleEffect: public ISceneObjectComponent {
    public:
        static const ComponentId id;
        
        ParticleEffect(SceneObject& sceneObject,
                       IParticleSystem& particleSystem,
                       const ParticleSettings& particleSettings,
                       const EmitterSettings& emitterSettings,
                       RenderMode renderMode);
        ~ParticleEffect();
        
        void Update(float dt);
        void Start();
        void Stop();
        
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
        
        SceneObject& mSceneObject;
        IParticleSystem& mParticleSystem;
        ParticleEmitter mEmitter;
        RenderMode mRenderMode {RenderMode::Triangles};
        std::vector<Particle> mParticles;
        std::unique_ptr<VertexBuffer> mVertexBuffer;
        std::shared_ptr<RenderableObject> mRenderableObject;
        bool mIsActive {false};
    };
}

#endif
