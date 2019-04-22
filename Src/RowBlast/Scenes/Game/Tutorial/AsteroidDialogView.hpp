#ifndef AsteroidDialogView_hpp
#define AsteroidDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "IGuiLightProvider.hpp"
#include "HandAnimation.hpp"
#include "TutorialFieldBottomGlowAnimation.hpp"

namespace Pht {
    class IEngine;
    class Animation;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class LevelResources;
    class PieceResources;

    class AsteroidDialogView: public Pht::GuiView {
    public:
        AsteroidDialogView(Pht::IEngine& engine,
                           const CommonResources& commonResources,
                           const PieceResources& pieceResources,
                           const LevelResources& levelResources,
                           const UserServices& userServices);

        void OnDeactivate() override;

        void SetUp();
        void Update();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }
        
        const MenuButton& GetPlayButton() const {
            return *mPlayButton;
        }
        
    private:
        void CreateAnimation(const PieceResources& pieceResources,
                             const LevelResources& levelResources);

        Pht::IEngine& mEngine;
        const UserServices& mUserServices;
        IGuiLightProvider* mGuiLightProvider {nullptr};
        Pht::Animation* mAnimation {nullptr};
        Pht::Animation* mHandPhtAnimation {nullptr};
        Pht::Animation* mPieceAnimation {nullptr};
        Pht::SceneObject* mMoves {nullptr};
        Pht::SceneObject* mGhostPieceContainer {nullptr};
        std::unique_ptr<HandAnimation> mHandAnimation;
        std::unique_ptr<TutorialFieldBottomGlowAnimation> mBottomGlowAnimation;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
