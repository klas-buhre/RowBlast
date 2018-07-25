#ifndef FillRowsWindowView_hpp
#define FillRowsWindowView_hpp

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class FillRowsWindowView: public Pht::GuiView {
    public:
        FillRowsWindowView(const CommonResources& commonResources);
    };
}

#endif
