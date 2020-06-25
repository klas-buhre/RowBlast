#ifndef HowToPlayDialogController_hpp
#define HowToPlayDialogController_hpp

// Game includes.
#include "HowToPlayDialogView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace Pht {
    class TouchEvent;
}

namespace RowBlast {
    class CommonResources;
    class PieceResources;
    class LevelResources;
    
    class HowToPlayDialogController {
    public:
        static constexpr auto singleTapPageIndex = 3;

        enum class Result {
            None,
            Close
        };
        
        HowToPlayDialogController(Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  const PieceResources& pieceResources,
                                  const LevelResources& levelResources,
                                  HowToPlayDialogView::SceneId sceneId);
        
        void SetUp(int startPage);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider);
        Result Update();
        
        HowToPlayDialogView& GetView() {
            return mView;
        }
    
    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);

        Pht::IEngine& mEngine;
        HowToPlayDialogView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult {Result::None};
        HowToPlayDialogView::SceneId mSceneId {HowToPlayDialogView::SceneId::Map};
    };
}

#endif
