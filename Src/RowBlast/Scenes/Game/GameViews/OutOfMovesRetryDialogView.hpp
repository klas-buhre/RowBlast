#ifndef OutOfMovesRetryDialogView_hpp
#define OutOfMovesRetryDialogView_hpp

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
    class IGuiLightProvider;
    class GameScene;
    
    class OutOfMovesRetryDialogView: public Pht::GuiView {
    public:
        OutOfMovesRetryDialogView(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void OnDeactivate() override;
        
        void SetUp(GameScene& scene);
        void Update();
        
        void SetGuiLightProvider(IGuiLightProvider& guiLightProvider) {
            mGuiLightProvider = &guiLightProvider;
        }

        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        const MenuButton& GetRetryButton() const {
            return *mRetryButton;
        }
        
    private:
        void CreateAddMovesIcon(const Pht::Vec3& position, const CommonResources& commonResources);
        void CreateMovesIcon(Pht::SceneObject& parent, const CommonResources& commonResources);
        void CreateArrow(const Pht::Vec3& position,
                         const Pht::Vec3& rotation,
                         Pht::RenderableObject& renderable,
                         Pht::SceneObject& parent);
        void CreateGlowEffect(Pht::SceneObject& parentObject);
        void CreateParticles(Pht::SceneObject& parentObject);
        void AnimateIconRotation(float dt);

        Pht::IEngine& mEngine;
        GameScene* mScene {nullptr};
        IGuiLightProvider* mGuiLightProvider {nullptr};
        std::unique_ptr<MenuButton> mCloseButton;
        std::unique_ptr<MenuButton> mRetryButton;
        std::unique_ptr<Pht::SceneObject> mGlowEffect;
        std::unique_ptr<Pht::SceneObject> mParticles;
        std::unique_ptr<Pht::RenderableObject> mArrowRenderable;
        Pht::SceneObject* mMovesIconSceneObject {nullptr};
        float mRotationAnimationTime {0.0f};
        float mScaleAnimationTime {0.0f};
    };
}

#endif
