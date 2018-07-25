#ifndef SwitchPieceWindowController_hpp
#define SwitchPieceWindowController_hpp

// Game includes.
#include "SwitchPieceWindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class SwitchPieceWindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        SwitchPieceWindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init();
        Result Update();
        void Close();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        SwitchPieceWindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
