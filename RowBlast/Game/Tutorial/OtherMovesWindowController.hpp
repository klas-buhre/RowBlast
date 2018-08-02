#ifndef OtherMovesWindowController_hpp
#define OtherMovesWindowController_hpp

// Game includes.
#include "OtherMovesWindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class OtherMovesWindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        OtherMovesWindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init();
        Result Update();
        void Close();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        OtherMovesWindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
