#ifndef Engine_hpp
#define Engine_hpp

#include <memory>

#include "IEngine.hpp"
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "Audio.hpp"

namespace Pht {
    class IApplication;
    
    class Engine: public IEngine {
    public:
        Engine(bool createRenderBuffers, const Vec2& screenInputSize);
        
        IRenderer& GetRenderer() override;
        IInput& GetInput() override;
        IAudio& GetAudio() override;
        float GetLastFrameSeconds() const override;
        std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material) override;
        
        void Initialize(bool createRenderBuffers);
        void Update(float frameSeconds);

        InputHandler& GetInputHandler() {
            return mInputHandler;
        }
        
    private:
        Renderer mRenderer;
        InputHandler mInputHandler;
        Audio mAudio;
        std::unique_ptr<IApplication> mApplication;
        float mLastFrameSeconds {0.0f};
    };
}

#endif
