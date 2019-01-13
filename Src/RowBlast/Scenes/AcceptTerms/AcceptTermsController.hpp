#ifndef AcceptTermsController_hpp
#define AcceptTermsController_hpp

// Game includes.
#include "AcceptTermsScene.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class AcceptTermsController {
    public:
        enum class Command {
            GoToTitle,
            None
        };
        
        AcceptTermsController(Pht::IEngine& engine);
    
        bool IsTermsAccepted() const;
        void Init();
        Command Update();
    
    private:
        Pht::IEngine& mEngine;
        AcceptTermsScene mScene;
    };
}

#endif
