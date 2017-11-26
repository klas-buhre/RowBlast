#ifndef TitleController_hpp
#define TitleController_hpp

// Game includes.
#include "TitleSceneRenderer.hpp"
#include "TitleScene.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    
    class TitleController {
    public:
        enum class Command {
            GoToMap,
            None
        };
        
        TitleController(Pht::IEngine& engine, const CommonResources& commonResources);
    
        void Reset();
        Command Update();
        void RenderScene();
    
    private:
        Pht::IEngine& mEngine;
        TitleScene mScene;
        TitleSceneRenderer mRenderer;
    };
}

#endif
