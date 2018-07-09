#ifndef LevelGoalDialogController_hpp
#define LevelGoalDialogController_hpp

// Game includes.
#include "LevelGoalDialogView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IInput;
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class PieceResources;
    
    class LevelGoalDialogController {
    public:
        enum class Result {
            None,
            Play,
            Close
        };
        
        LevelGoalDialogController(Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  PieceResources& pieceResources,
                                  PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void Init(SlidingMenuAnimation::UpdateFade updateFade, const LevelInfo& levelInfo);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        LevelGoalDialogView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IInput& mInput;
        LevelGoalDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        SlidingMenuAnimation::UpdateFade mUpdateFade {SlidingMenuAnimation::UpdateFade::Yes};
    };
}

#endif
