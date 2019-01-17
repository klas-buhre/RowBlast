#ifndef DocumentViewerController_hpp
#define DocumentViewerController_hpp

// Engine includes.
#include "ScrollPanel.hpp"

// Game includes.
#include "DocumentViewerScene.hpp"
#include "DocumentViewerDialogView.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    
    enum class DocumentId {
        TermsOfService,
        PrivacyPolicy,
        Credits
    };

    class DocumentViewerController {
    public:
        enum class Command {
            Close,
            None
        };
        
        DocumentViewerController(Pht::IEngine& engine, const CommonResources& commonResources);
    
        void Init(DocumentId document);
        Command Update();
    
    private:
        enum class Result {
            Close,
            TouchStartedOverButton,
            None
        };
        
        Result OnTouch(const Pht::TouchEvent& touchEvent);

        Pht::IEngine& mEngine;
        const CommonResources& mCommonResources;
        DocumentViewerScene mScene;
        DocumentViewerDialogView mDialogView;
        std::unique_ptr<Pht::ScrollPanel> mScrollPanel;
    };
}

#endif
