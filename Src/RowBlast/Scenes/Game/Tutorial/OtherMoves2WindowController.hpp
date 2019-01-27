#ifndef OtherMoves2WindowController_hpp
#define OtherMoves2WindowController_hpp

// Game includes.
#include "OtherMoves2WindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class OtherMoves2WindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        OtherMoves2WindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void SetUp();
        Result Update();
        void Close();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        OtherMoves2WindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
