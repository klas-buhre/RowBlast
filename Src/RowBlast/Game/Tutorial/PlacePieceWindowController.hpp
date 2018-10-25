#ifndef PlacePieceWindowController_hpp
#define PlacePieceWindowController_hpp

// Game includes.
#include "PlacePieceWindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class PlacePieceWindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        PlacePieceWindowController(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init();
        Result Update();
        void Close();

        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        PlacePieceWindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
