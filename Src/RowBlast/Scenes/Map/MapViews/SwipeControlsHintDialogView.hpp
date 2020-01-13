#ifndef SwipeControlsHintDialogView_hpp
#define SwipeControlsHintDialogView_hpp

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
    
    class SwipeControlsHintDialogView: public Pht::GuiView {
    public:
        SwipeControlsHintDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetHowToPlayButton() const {
            return *mHowToPlayButton;
        }

        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }

    private:
        void CreateSingleTapIcon(const Pht::Vec3& position,
                                 Pht::SceneObject& parent,
                                 Pht::IEngine& engine);

        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mHowToPlayButton;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
