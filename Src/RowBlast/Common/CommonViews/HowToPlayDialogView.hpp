#ifndef HowToPlayDialogView_hpp
#define HowToPlayDialogView_hpp

#include <memory>
#include <vector>

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
        void GoToNextPage();
        void GoToPreviousPage();
        void Update();

        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetPreviousButton() const {
            return *mPreviousButton;
        }

        const MenuButton& GetNextButton() const {
            return *mNextButton;
        }

    private:
        struct ClearBlocksChildAnimations {
            bool mPlacePiece {false};
        };
        
        void CreateGoalPage(const GuiResources& guiResources,
                            const PieceResources& pieceResources,
                            const LevelResources& levelResources,
                            PotentiallyZoomedScreen zoom);
        void CreateControlsPage(const GuiResources& guiResources, PotentiallyZoomedScreen zoom);
        void CreatePlacePiecePage(const GuiResources& guiResources,
                                  const PieceResources& pieceResources,
                                  const LevelResources& levelResources,
                                  PotentiallyZoomedScreen zoom);
        void CreateFieldQuad(Pht::SceneObject& parent);
        Pht::SceneObject& CreateFilledCircleIcon(int index, bool isFilled);
        Pht::Animation& CreateClearBlocksAnimation(Pht::SceneObject& parent,
                                                   const ClearBlocksChildAnimations& childAnimations,
                                                   const PieceResources& pieceResources,
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
        Pht::SceneObject& CreateLPieceGhostPiece(const Pht::Vec3& position,
                                                 float rotation,
                                                 Pht::SceneObject& parent,
                                                 const LevelResources& levelResources);
        void CreateIcon(const std::string& filename,
                        const Pht::Vec3& position,
                        const Pht::Vec2& size,
                        Pht::SceneObject& parent);
        void CreateSingleTapIcon(const Pht::Vec3& position, Pht::SceneObject& parent);
        void CreateSwipeIcon(const Pht::Vec3& position, Pht::SceneObject& parent);
        void SetPage(int pageIndex);
        
        struct Page {
            Pht::SceneObject& mSceneObject;
            Pht::Animation* mAnimation {nullptr};
        };
        
        Pht::IEngine& mEngine;
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mNextButton;
        std::unique_ptr<MenuButton> mPreviousButton;
        std::vector<Page> mPages;
        int mPageIndex {0};
        int mNumPages {9};
    };
}

#endif