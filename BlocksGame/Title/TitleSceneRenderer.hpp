#ifndef TitleSceneRenderer_hpp
#define TitleSceneRenderer_hpp

namespace Pht {
    class IRenderer;
}

namespace BlocksGame {
    class TitleScene;
    
    class TitleSceneRenderer {
    public:
        TitleSceneRenderer(Pht::IRenderer& engineRenderer, const TitleScene& scene);
    
        void RenderFrame();
        
    private:
        Pht::IRenderer& mEngineRenderer;
        const TitleScene& mScene;
    };
}

#endif
