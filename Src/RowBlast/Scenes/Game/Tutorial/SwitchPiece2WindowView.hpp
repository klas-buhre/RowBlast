#ifndef SwitchPiece2WindowView_hpp
#define SwitchPiece2WindowView_hpp

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class SwitchPiece2WindowView: public Pht::GuiView {
    public:
        SwitchPiece2WindowView(const CommonResources& commonResources);
    };
}

#endif
