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
    class TextComponent;
    class TextProperties;
}

namespace RowBlast {
    class MenuButton {
    public:
        struct Style {
            Pht::Color mColor;
            Pht::Color mSelectedColor;
            float mPressedScale {1.0f};
            bool mIsRounded {true};
            bool mHasShadow {false};
            float mOpacity {0.8f};
        };
        
        MenuButton(Pht::IEngine& engine,
                   Pht::GuiView& view,
                   const Pht::Vec3& position,
                   const Pht::Vec2& size,
                   const Pht::Vec2& inputSize,
                   const Style& style);
        
        Pht::TextComponent& CreateText(const Pht::Vec3& position,
                                       const std::string& text,
                                       const Pht::TextProperties& properties);
        bool IsClicked(const Pht::TouchEvent& event) const;
        
        Pht::SceneObject& GetSceneObject() {
            return *mSceneObjects.front();
        }
        
    private:
        void AddSceneObject(std::unique_ptr<Pht::SceneObject> sceneObject);
        
        Pht::GuiView& mView;
        Pht::IAudio& mAudio;
        std::unique_ptr<Pht::Button> mButton;
        std::vector<Pht::SceneObject*> mSceneObjects;
    };
}

#endif
