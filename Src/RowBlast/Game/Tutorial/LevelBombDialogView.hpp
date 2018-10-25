#ifndef LevelBombDialogView_hpp
#define LevelBombDialogView_hpp

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

    class LevelBombDialogView: public Pht::GuiView {
    public:
        LevelBombDialogView(Pht::IEngine& engine, const CommonResources& commonResources);

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
