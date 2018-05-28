#ifndef LevelStartDialogView_hpp
#define LevelStartDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class LevelInfo;
    
    class LevelStartDialogView: public Pht::GuiView {
    public:
        LevelStartDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        void Init(const LevelInfo& levelInfo);
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }
        
    private:
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mPlayButton;
        Pht::TextComponent* mCaption {nullptr};
        Pht::TextComponent* mClearObjective {nullptr};
        Pht::TextComponent* mBuildObjective {nullptr};
    };
}

#endif
