#ifndef IEngine_hpp
#define IEngine_hpp

namespace Pht {
    class IRenderer;
    class IInput;
    class IAudio;
    class ISceneManager;
    class RenderableObject;
    class IMesh;
    class Material;
    class IParticleSystem;

    class IEngine {
    public:
        virtual ~IEngine() {}
        
        virtual IRenderer& GetRenderer() = 0;
        virtual IInput& GetInput() = 0;
        virtual IAudio& GetAudio() = 0;
        virtual ISceneManager& GetSceneManager() = 0;
        virtual IParticleSystem& GetParticleSystem() = 0;
        virtual float GetLastFrameSeconds() const = 0;
        virtual std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                         const Material& material) = 0;
    };
}

#endif
