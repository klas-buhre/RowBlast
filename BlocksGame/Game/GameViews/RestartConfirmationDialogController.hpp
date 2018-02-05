#ifndef RestartConfirmationDialogController_hpp
#define RestartConfirmationDialogController_hpp

// Game includes.
#include "RestartConfirmationDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace BlocksGame {
    class CommonResources;
    class UserData;
    
    class RestartConfirmationDialogController {
    public:
        enum class Result {
            None,
            RestartGame,
            DoNotRestartGame
        };
        
        RestartConfirmationDialogController(Pht::IEngine& engine,
                                            const CommonResources& commonResources,
                                            const UserData& userData);
        
        void Init();
        Result Update();
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        const UserData& mUserData;
        RestartConfirmationDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
