#ifndef MenuButton_hpp
#define MenuButton_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "Button.hpp"
#include "Vector.hpp"
#include "Material.hpp"

namespace Pht {
    class IEngine;
    class IAudio;
    class GuiView;
    class TouchEvent;
    class Text;
    class SceneObject;
}

namespace BlocksGame {
    class MenuButton {
    public:
        struct Style {
            Pht::Color mColor;
            Pht::Color mSelectedColor;
            float mSelectedScale {1.0f};
            bool mIsRounded {true};
            float mOpacity {0.8f};
        };
        
        MenuButton(Pht::IEngine& engine,
                   Pht::GuiView& view,
                   const Pht::Vec3& position,
                   const Pht::Vec2& size,
                   const Pht::Vec2& inputSize,
                   const Style& style);
        
        void SetText(std::unique_ptr<Pht::Text> text);
        bool IsClicked(const Pht::TouchEvent& event) const;
        
        std::vector<Pht::SceneObject*>& GetSceneObjects() {
            return mSceneObjects;
        }
        
    private:
        void AddSceneObject(std::unique_ptr<Pht::SceneObject> sceneObject);
        
        Pht::GuiView& mView;
        Pht::Vec3 mPosition;
        std::unique_ptr<Pht::Button> mButton;
        std::vector<Pht::SceneObject*> mSceneObjects;
        Pht::IAudio& mAudio;
    };
}

#endif
