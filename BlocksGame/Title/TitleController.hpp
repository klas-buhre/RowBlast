#ifndef TitleController_hpp
#define TitleController_hpp

// Game includes.
#include "TitleScene.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class TitleController {
    public:
        enum class Command {
            GoToMap,
            None
        };
        
        TitleController(Pht::IEngine& engine, const CommonResources& commonResources);
    
        Command Update();
    
    private:
        Pht::IEngine& mEngine;
        TitleScene mScene;
    };
}

#endif
