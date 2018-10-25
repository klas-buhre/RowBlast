#ifndef LaserDialogView_hpp
#define LaserDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "SlideAnimation.hpp"
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace RowBlast {
    class CommonResources;

    class LaserDialogView: public Pht::GuiView {
    public:
        LaserDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

        void Init(Pht::Scene& scene);
        void Update();
        
        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }
        
    private:
        std::unique_ptr<SlideAnimation> mSlideAnimation;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
