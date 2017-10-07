#ifndef MapPin_hpp
#define MapPin_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "Button.hpp"
#include "Font.hpp"

namespace Pht {
    class IRenderer;
    class IEngine;
}

namespace BlocksGame {
    class MapPin {
    public:
        MapPin(std::shared_ptr<Pht::RenderableObject> renderable,
               const Pht::Vec3& position,
               int level,
               const Pht::Text& text,
               Pht::IEngine& engine);
        
        void Update();
        void SetNumStars(int numStars, std::shared_ptr<Pht::RenderableObject> starRenderable);
        
        const Pht::Vec3& GetPosition() const {
            return mPosition;
        }
        
        Pht::SceneObject& GetSceneObject() {
            return mSceneObject;
        }
        
        const Pht::Text& GetText() const {
            return mText;
        }
        
        Pht::Button& GetButton() {
            return mButton;
        }
        
        int GetLevel() const {
            return mLevel;
        }
        
        const std::vector<std::unique_ptr<Pht::SceneObject>>& GetStars() const {
            return mStars;
        }
        
        void SetIsSelected(bool isSelected) {
            mIsSelected = isSelected;
        }
        
        bool IsSelected() const {
            return mIsSelected;
        }
        
        void SetIsClickable(bool isClickable) {
            mIsClickable = isClickable;
        }
        
        bool IsClickable() const {
            return mIsClickable;
        }
        
        static Pht::Color selectedColorAdd;
        
    private:
        Pht::IRenderer& mRenderer;
        Pht::Vec3 mPosition;
        Pht::SceneObject mSceneObject;
        Pht::Button mButton;
        int mLevel;
        Pht::Text mText;
        std::vector<std::unique_ptr<Pht::SceneObject>> mStars;
        bool mIsSelected {false};
        bool mIsClickable {false};
    };
}

#endif
