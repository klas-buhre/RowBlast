#ifndef BombDialogController_hpp
#define BombDialogController_hpp

// Game includes.
#include "BombDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    
    class BombDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        BombDialogController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init();
        Result Update();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        BombDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
