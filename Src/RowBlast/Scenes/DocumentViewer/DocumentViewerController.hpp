#ifndef DocumentViewerController_hpp
#define DocumentViewerController_hpp

// Game includes.
#include "DocumentViewerScene.hpp"
#include "DocumentViewerDialogView.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;

    class DocumentViewerController {
    public:
        enum class Command {
            Close,
            None
        };
        
        DocumentViewerController(Pht::IEngine& engine, const CommonResources& commonResources);
    
        void Init();
        Command Update();
    
    private:
        Command OnTouch(const Pht::TouchEvent& touchEvent);

        Pht::IEngine& mEngine;
        DocumentViewerScene mScene;
        DocumentViewerDialogView mDialogView;
    };
}

#endif
