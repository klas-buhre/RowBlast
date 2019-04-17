#ifndef LaserDialogView_hpp
#define LaserDialogView_hpp

#include <memory>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "IGuiLightProvider.hpp"
#include "HandAnimation.hpp"
#include "TutorialLaserParticleEffect.hpp"

namespace Pht {
    class IEngine;
    class Animation;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class LevelResources;
    class PieceResources;

    class LaserDialogView: public Pht::GuiView {
    public:
        LaserDialogView(Pht::IEngine& engine,
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
        std::unique_ptr<HandAnimation> mHandAnimation;
        std::unique_ptr<TutorialLaserParticleEffect> mLaserEffect;
        std::unique_ptr<MenuButton> mPlayButton;
    };
}

#endif
