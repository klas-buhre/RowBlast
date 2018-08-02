#ifndef OtherMovesWindowView_hpp
#define OtherMovesWindowView_hpp

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class OtherMovesWindowView: public Pht::GuiView {
    public:
        OtherMovesWindowView(const CommonResources& commonResources);
    };
}

#endif
