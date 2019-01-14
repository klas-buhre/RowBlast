#ifndef AcceptTermsController_hpp
#define AcceptTermsController_hpp

// Game includes.
#include "AcceptTermsScene.hpp"
#include "AcceptTermsDialogView.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;

    class AcceptTermsController {
    public:
        enum class Command {
            Accept,
            ViewTermsOfService,
            ViewPrivacyPolicy,
            None
        };
        
        AcceptTermsController(Pht::IEngine& engine, const CommonResources& commonResources);
    
        bool IsTermsAccepted() const;
        void Init();
        Command Update();
    
    private:
        Command OnTouch(const Pht::TouchEvent& touchEvent);

        Pht::IEngine& mEngine;
        AcceptTermsScene mScene;
        AcceptTermsDialogView mDialogView;
    };
}

#endif
