#ifndef SameColorDialogView_hpp
#define SameColorDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;

    class SameColorDialogView: public Pht::GuiView {
    public:
        SameColorDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
