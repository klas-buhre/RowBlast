#ifndef PlayOnYourOwnWindowController_hpp
#define PlayOnYourOwnWindowController_hpp

// Game includes.
#include "PlayOnYourOwnWindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class PlayOnYourOwnWindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        PlayOnYourOwnWindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init();
        void SetUp();
        void OnPause();
        void OnResumePlaying();
        Result Update();
        void Close();
        
        Pht::GuiView& GetView() {
            return mView;
        }
        
        int GetShownNumTimes() const {
            return mShownNumTimes;
        }
        
    private:
        Pht::IEngine& mEngine;
        PlayOnYourOwnWindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        int mShownNumTimes {0};
        float mElapsedTime {0.0f};
        bool mIsGamePaused {false};
    };
}

#endif
