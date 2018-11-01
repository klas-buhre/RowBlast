#ifndef TitleController_hpp
#define TitleController_hpp

// Game includes.
#include "TitleScene.hpp"
#include "Ufo.hpp"
#include "UfoAnimation.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class Universe;
    
    class TitleController {
    public:
        enum class Command {
            GoToMap,
            None
        };
        
        TitleController(Pht::IEngine& engine,
                        const CommonResources& commonResources,
                        const UserServices& userServices,
                        const Universe& universe);
    
        Command Update();
    
    private:
        Pht::IEngine& mEngine;
        TitleScene mScene;
        Ufo mUfo;
        UfoAnimation mUfoAnimation;
    };
}

#endif
