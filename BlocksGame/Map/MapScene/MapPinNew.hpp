#ifndef MapPinNew_hpp
#define MapPinNew_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "Button.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class RenderableObject;
    class Button;
    class Material;
}

namespace BlocksGame {
    class CommonResources;
    
    class MapPinNew {
    public:
        MapPinNew(Pht::IEngine& engine,
                  const CommonResources& commonResources,
                  Pht::Scene& scene,
                  Pht::SceneObject& containerObject,
                  std::shared_ptr<Pht::RenderableObject> starRenderable,
                  const Pht::Vec3& position,
                  int level,
                  int numStars,
                  bool isClickable);

        void SetIsSelected(bool isSelected);
        const Pht::Vec3& GetPosition() const;

        Pht::Button& GetButton() {
            return *mButton;
        }

        bool IsClickable() const {
            return mIsClickable;
        }
        
        int GetLevel() const {
            return mLevel;
        }

    private:
        const Pht::Material& mBlueMaterial;
        Pht::SceneObject* mSceneObject {nullptr};
        std::unique_ptr<Pht::Button> mButton;
        int mLevel;
        bool mIsClickable {false};
    };
}

#endif
