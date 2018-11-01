#ifndef GameOverDialogController_hpp
#define GameOverDialogController_hpp

// Game includes.
#include "GameOverDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    
    class GameOverDialogController {
    public:
        enum class Result {
            None,
            Retry,
            BackToMap
        };
        
        GameOverDialogController(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const UserServices& userServices);
        
        void Init();
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        const UserServices& mUserServices;
        GameOverDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
