#ifndef LevelCompletedDialogView_hpp
#define LevelCompletedDialogView_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CloseButton.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    
    class LevelCompletedDialogView: public Pht::GuiView {
    public:
        LevelCompletedDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init();
        void Update();
        void SetNumStars(int numStars);
        
        const CloseButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetNextButton() const {
            return *mNextButton;
        }
        
    private:
        void LoadStar(const CommonResources& commonResources);
        
        Pht::IEngine& mEngine;
        std::unique_ptr<CloseButton> mCloseButton;
        std::unique_ptr<MenuButton> mNextButton;
        std::unique_ptr<Pht::RenderableObject> mStarRenderable;
        std::vector<Pht::SceneObject*> mStars;
        float mStarAngle {0.0f};
    };
}

#endif
