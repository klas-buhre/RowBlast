#ifndef BombDialogView_hpp
#define BombDialogView_hpp

#include <memory>

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
    class BlastArea;

    class BombDialogView: public Pht::GuiView {
    public:
        BombDialogView(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       const PieceResources& pieceResources,
                       const LevelResources& levelResources,
                       const BlastArea& blastRadiusAnimation,
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
                             const LevelResources& levelResources,
                             const BlastArea& blastRadiusAnimation);

        Pht::IEngine& mEngine;
        const UserServices& mUserServices;
        IGuiLightProvider* mGuiLightProvider {nullptr};
        Pht::Animation* mAnimation {nullptr};
        Pht::Animation* mHandPhtAnimation {nullptr};
        Pht::Animation* mBlastRadiusPhtAnimation {nullptr};
        Pht::Animation* mBombAnimation {nullptr};
        Pht::SceneObject* mBombMoves {nullptr};
        Pht::SceneObject* mBombGhostPieceContainer {nullptr};
        Pht::SceneObject* mBlastRadius {nullptr};
        std::unique_ptr<HandAnimation> mHandAnimation;
        std::unique_ptr<TutorialExplosionParticleEffect> mExplosionEffect;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
