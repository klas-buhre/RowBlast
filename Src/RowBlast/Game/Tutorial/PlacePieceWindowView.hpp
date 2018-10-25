#ifndef PlacePieceWindowView_hpp
#define PlacePieceWindowView_hpp

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class PlacePieceWindowView: public Pht::GuiView {
    public:
        PlacePieceWindowView(const CommonResources& commonResources);
    };
}

#endif
