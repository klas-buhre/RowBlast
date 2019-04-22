#ifndef AsteroidDialogController_hpp
#define AsteroidDialogController_hpp

// Game includes.
#include "AsteroidDialogView.hpp"
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
    
    class AsteroidDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        AsteroidDialogController(Pht::IEngine& engine,
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
        AsteroidDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
