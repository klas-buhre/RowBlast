#ifndef LaserDialogController_hpp
#define LaserDialogController_hpp

// Game includes.
#include "LaserDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class PieceResources;
    class LevelResources;
    
    class LaserDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        LaserDialogController(Pht::IEngine& engine,
                              const CommonResources& commonResources,
                              const PieceResources& pieceResources,
                              const LevelResources& levelResources,
                              const UserServices& userServices);
        
        void SetUp();
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider);
        Result Update();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        LaserDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
