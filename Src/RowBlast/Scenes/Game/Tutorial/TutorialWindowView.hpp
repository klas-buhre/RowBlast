#ifndef TutorialWindowView_hpp
#define TutorialWindowView_hpp

#include <vector>

// Engine includes.
#include "GuiView.hpp"

namespace RowBlast {
    class CommonResources;
    
    class TutorialWindowView: public Pht::GuiView {
    public:
        TutorialWindowView(const CommonResources& commonResources,
                           const std::vector<std::string>& textLines,
                           float yPosition);
    };
}

#endif
