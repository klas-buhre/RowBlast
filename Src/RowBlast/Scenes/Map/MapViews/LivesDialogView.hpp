#ifndef LivesDialogView_hpp
#define LivesDialogView_hpp

#include <memory>
#include <chrono>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class UserServices;
    class CommonResources;
    class IGuiLightProvider;
    
    class LivesDialogView: public Pht::GuiView {
    public:
        LivesDialogView(Pht::IEngine& engine,
                        const CommonResources& commonResources,
                        const UserServices& userServices,
                        IGuiLightProvider& guiLightProvider);
        
        void SetUp();
        void Update();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetOkButton() const {
            return *mOkButton;
        }
        
    private:
        void CreateGlowEffect(const Pht::Vec3& position,
                              Pht::SceneObject& parentObject,
                              float scale);
        void UpdateNumLivesTexts();
        void UpdateCountdownText();
        void UpdateHeart(float dt);
        
        Pht::IEngine& mEngine;
        const UserServices& mUserServices;
        IGuiLightProvider& mGuiLightProvider;
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mOkButton;
        std::chrono::seconds mSecondsUntilNewLife;
        int mNumLives {0};
        std::unique_ptr<Pht::SceneObject> mGlowEffect;
        Pht::TextComponent* mCaptionText {nullptr};
        Pht::TextComponent* mNumLivesText {nullptr};
        Pht::TextComponent* mCountdownText {nullptr};
        Pht::TextComponent* mFullText {nullptr};
        Pht::SceneObject* mHeartSceneObject {nullptr};
        float mAnimationTime {0.0f};
    };
}

#endif
