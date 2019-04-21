#ifndef LevelBombDialogView_hpp
#define LevelBombDialogView_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "IGuiLightProvider.hpp"
#include "HandAnimation.hpp"
#include "TutorialExplosionParticleEffect.hpp"

namespace Pht {
    class IEngine;
    class Animation;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class LevelResources;
    class PieceResources;

    class LevelBombDialogView: public Pht::GuiView {
    public:
        LevelBombDialogView(Pht::IEngine& engine,
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
        Pht::SceneObject* mLevelBombs {nullptr};
        Pht::SceneObject* mGhostPieceContainer {nullptr};
        std::unique_ptr<HandAnimation> mHandAnimation;
        std::vector<TutorialExplosionParticleEffect> mExplosionEffects;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
