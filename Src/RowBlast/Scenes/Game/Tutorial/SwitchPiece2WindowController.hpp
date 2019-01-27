#ifndef SwitchPiece2WindowController_hpp
#define SwitchPiece2WindowController_hpp

// Game includes.
#include "SwitchPiece2WindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class SwitchPiece2WindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        SwitchPiece2WindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp();
        Result Update();
        void Close();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        SwitchPiece2WindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
