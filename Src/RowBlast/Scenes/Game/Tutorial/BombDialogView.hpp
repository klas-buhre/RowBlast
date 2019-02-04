#ifndef BombDialogView_hpp
#define BombDialogView_hpp

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
    class UserServices;

    class BombDialogView: public Pht::GuiView {
    public:
        BombDialogView(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       const UserServices& userServices);

        void SetUp(Pht::Scene& scene);
        void Update();
        
        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }
        
    private:
        const UserServices& mUserServices;
        std::unique_ptr<SlideAnimation> mSlideAnimation;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
