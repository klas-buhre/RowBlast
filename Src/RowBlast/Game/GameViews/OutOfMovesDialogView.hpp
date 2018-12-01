#ifndef OutOfMovesDialogView_hpp
#define OutOfMovesDialogView_hpp

#include <memory>
#include <vector>

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
    class PieceResources;
    class IGuiLightProvider;
    class GameScene;
    
    class OutOfMovesDialogView: public Pht::GuiView {
    public:
        OutOfMovesDialogView(Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             const PieceResources& pieceResources);

        void SetUp(GameScene& scene);
        void Update();
        void OnDeactivate();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetPlayOnButton() const {
            return *mPlayOnButton;
        }
        
    private:
        void CreateAddMovesIcon(const Pht::Vec3& position,
                                const CommonResources& commonResources,
                                const PieceResources& pieceResources);
        void CreateLPiece(Pht::SceneObject& parentObject,
                          const PieceResources& pieceResources);
        void CreateGreenBlock(const Pht::Vec3& position,
                              Pht::RenderableObject& blockRenderable,
                              Pht::SceneObject& lPiece);
        void CreateGlowEffect(Pht::SceneObject& parentObject);

        Pht::IEngine& mEngine;
        GameScene* mScene {nullptr};
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mPlayOnButton;
        std::unique_ptr<Pht::SceneObject> mGlowEffect;
        Pht::SceneObject* mUpperHudSceneObject {nullptr};
    };
}

#endif
