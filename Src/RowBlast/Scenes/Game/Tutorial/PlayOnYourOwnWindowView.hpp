#ifndef PlayOnYourOwnWindowView_hpp
#define PlayOnYourOwnWindowView_hpp

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class PlayOnYourOwnWindowView: public Pht::GuiView {
    public:
        PlayOnYourOwnWindowView(const CommonResources& commonResources);
    };
}

#endif
