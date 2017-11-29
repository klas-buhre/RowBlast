#ifndef Engine_hpp
#define Engine_hpp

#include <memory>

#include "IEngine.hpp"
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "Audio.hpp"
#include "SceneManager.hpp"
#include "ParticleSystem.hpp"

namespace Pht {
    class IApplication;
    
    class Engine: public IEngine {
    public:
        Engine(bool createRenderBuffers, const Vec2& screenInputSize);
        
        IRenderer& GetRenderer() override;
        IInput& GetInput() override;
        IAudio& GetAudio() override;
        ISceneManager& GetSceneManager() override;
        IParticleSystem& GetParticleSystem() override;
        float GetLastFrameSeconds() const override;
        
        void Initialize(bool createRenderBuffers);
        void Update(float frameSeconds);

        InputHandler& GetInputHandler() {
            return mInputHandler;
        }
        
    private:
        Renderer mRenderer;
        InputHandler mInputHandler;
        Audio mAudio;
        SceneManager mSceneManager;
        ParticleSystem mParticleSystem;
        std::unique_ptr<IApplication> mApplication;
        float mLastFrameSeconds {0.0f};
    };
}

#endif
