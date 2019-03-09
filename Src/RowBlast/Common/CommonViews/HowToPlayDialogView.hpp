#ifndef HowToPlayDialogView_hpp
#define HowToPlayDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class IGuiLightProvider;
    class PieceResources;
    
    class HowToPlayDialogView: public Pht::GuiView {
    public:
        enum class SceneId {
            Map,
            Game
        };
    
        HowToPlayDialogView(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const PieceResources& pieceResources,
                            SceneId sceneId);

        void OnDeactivate() override;
    
        void SetUp();
        void Update();

        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
    private:
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
    };
}

#endif
