#ifndef FillRowsWindowController_hpp
#define FillRowsWindowController_hpp

// Game includes.
#include "FillRowsWindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class FillRowsWindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        FillRowsWindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp();
        Result Update();
        void Close();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        FillRowsWindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
