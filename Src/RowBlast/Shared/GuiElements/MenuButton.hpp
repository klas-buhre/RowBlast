#ifndef MenuButton_hpp
#define MenuButton_hpp

#include <memory>

// Engine includes.
#include "Button.hpp"
#include "Vector.hpp"
#include "Material.hpp"

namespace Pht {
    class IEngine;
    class IAudio;
    class GuiView;
    class TouchEvent;
    class SceneObject;
    class RenderableObject;
    class TextComponent;
    class TextProperties;
}

namespace RowBlast {
    class MenuButton {
    public:
        struct Style {
            std::string mMeshFilename;
            Pht::Color mColor;
            Pht::Color mSelectedColor;
            float mPressedScale {1.0f};
            bool mHasShadow {false};
            Pht::RenderableObject* mRenderableObject {nullptr};
            Pht::RenderableObject* mSelectedRenderableObject {nullptr};
            float mTextScale {1.0f};
        };
        
        MenuButton(Pht::IEngine& engine,
                   Pht::GuiView& view,
                   const Pht::Vec3& position,
                   const Pht::Vec2& inputSize,
                   const Style& style);

        MenuButton(Pht::IEngine& engine,
                   Pht::GuiView& view,
                   Pht::SceneObject& parent,
                   const Pht::Vec3& position,
                   const Pht::Vec2& inputSize,
                   const Style& style);

        Pht::TextComponent& CreateText(const Pht::Vec3& position,
                                       const std::string& text,
                                       const Pht::TextProperties& properties);
        bool IsClicked(const Pht::TouchEvent& event) const;
        
        Pht::SceneObject& GetSceneObject() {
            return *mSceneObject;
        }
        
        const Pht::Button& GetButton() const {
            return *mButton;
        }

    private:
        Pht::GuiView& mView;
        Pht::IAudio& mAudio;
        std::unique_ptr<Pht::Button> mButton;
        Pht::SceneObject* mSceneObject {nullptr};
        Pht::TextComponent* mText {nullptr};
        Style mStyle;
    };
}

#endif
