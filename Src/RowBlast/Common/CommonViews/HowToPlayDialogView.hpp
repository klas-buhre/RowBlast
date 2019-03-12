#ifndef HowToPlayDialogView_hpp
#define HowToPlayDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"
#include "Cell.hpp"

namespace Pht {
    class IEngine;
    class Animation;
}

namespace RowBlast {
    class IGuiLightProvider;
    class PieceResources;
    class LevelResources;
    
    class HowToPlayDialogView: public Pht::GuiView {
    public:
        enum class SceneId {
            Map,
            Game
        };
    
        HowToPlayDialogView(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const PieceResources& pieceResources,
                            const LevelResources& levelResources,
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
        void CreateBlockAnimation(const PieceResources& pieceResources,
                                  const LevelResources& levelResources);
        Pht::SceneObject& CreateLPiece(const Pht::Vec3& position,
                                       Pht::SceneObject& parent,
                                       const PieceResources& pieceResources);
        Pht::SceneObject& CreateTwoBlocks(const Pht::Vec3& position,
                                          BlockColor color,
                                          Pht::SceneObject& parent,
                                          const PieceResources& pieceResources);
        Pht::SceneObject& CreateThreeGrayBlocks(const Pht::Vec3& position,
                                                Pht::SceneObject& parent,
                                                const LevelResources& levelResources);
        Pht::SceneObject& CreateThreeGrayBlocksWithGap(const Pht::Vec3& position,
                                                       Pht::SceneObject& parent,
                                                       const LevelResources& levelResources);
        void CreateBlock(const Pht::Vec3& position,
                         Pht::RenderableObject& blockRenderable,
                         Pht::SceneObject& parent);
        void CreateWeld(const Pht::Vec3& position,
                        Pht::RenderableObject& weldRenderable,
                        float rotation,
                        Pht::SceneObject& parent);
        
        Pht::IEngine& mEngine;
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
        Pht::Animation* mAnimation {nullptr};
    };
}

#endif
