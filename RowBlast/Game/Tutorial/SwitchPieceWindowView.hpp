#ifndef SwitchPieceWindowView_hpp
#define SwitchPieceWindowView_hpp

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class SwitchPieceWindowView: public Pht::GuiView {
    public:
        SwitchPieceWindowView(const CommonResources& commonResources);
    };
}

#endif
