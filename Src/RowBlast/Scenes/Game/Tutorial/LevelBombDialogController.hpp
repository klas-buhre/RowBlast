#ifndef LevelBombDialogController_hpp
#define LevelBombDialogController_hpp

// Game includes.
#include "LevelBombDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class PieceResources;
    class GhostPieceBlocks;
    class LevelResources;
    
    class LevelBombDialogController {
    public:
        enum class Result {
            None,
            Play
        };
        
        LevelBombDialogController(Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  const PieceResources& pieceResources,
                                  const GhostPieceBlocks& ghostPieceBlocks,
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
        LevelBombDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
    };
}

#endif
