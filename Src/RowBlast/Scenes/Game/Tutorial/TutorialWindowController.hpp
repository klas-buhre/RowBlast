#ifndef TutorialWindowController_hpp
#define TutorialWindowController_hpp

// Game includes.
#include "TutorialWindowView.hpp"
#include "SlidingMenuAnimation.hpp"

namespace RowBlast {
    class CommonResources;
    
    class TutorialWindowController {
    public:
        enum class Result {
            None,
            Done
        };
        
        TutorialWindowController(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const std::vector<std::string>& textLines,
                                 float yPosition);
        
        void SetUp();
        Result Update();
        void Close();
        
        Pht::GuiView& GetView() {
            return mView;
        }
        
    private:
        TutorialWindowView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
    };
}

#endif
