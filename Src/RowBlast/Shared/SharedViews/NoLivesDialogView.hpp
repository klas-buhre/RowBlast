#ifndef NoLivesDialogView_hpp
#define NoLivesDialogView_hpp

#include <memory>
#include <chrono>
#include <vector>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class UserServices;
    class IGuiLightProvider;
    
    class NoLivesDialogView: public Pht::GuiView {
    public:
        NoLivesDialogView(Pht::IEngine& engine,
                          const CommonResources& commonResources,
                          const UserServices& userServices,
                          PotentiallyZoomedScreen potentiallyZoomedScreen);

        void SetUp();
        void Update();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetRefillLivesButton() const {
            return *mRefillLivesButton;
        }
        
    private:
        void CreateNoLivesIcon(const Pht::Vec3& position,
                               const CommonResources& commonResources,
                               PotentiallyZoomedScreen zoom);
        void CreateAddLivesIcon(const Pht::Vec3& position,
                                const CommonResources& commonResources,
                                PotentiallyZoomedScreen zoom);
        void CreateGlowEffect(Pht::SceneObject& parentObject, float scale);
        void CreateParticles(Pht::SceneObject& parentObject, float scale);
        void UpdateCountdownText();
        void UpdateHeartAnimation(float dt);
        void UpdateHeartBeatAnimation(float dt);

        Pht::IEngine& mEngine;
        const UserServices& mUserServices;
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mRefillLivesButton;
        std::chrono::seconds mSecondsUntilNewLife;
        Pht::TextComponent* mCountdownText {nullptr};
        Pht::SceneObject* mHeartSceneObject {nullptr};
        std::vector<std::unique_ptr<Pht::SceneObject>> mParticleEffects;
        float mAnimationTime {0.0f};
        float mHeartBeatAnimationTime {0.0f};
    };
}

#endif
