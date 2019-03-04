#ifndef MenuButton_hpp
#define MenuButton_hpp

#include <memory>

// Engine includes.
#include "Button.hpp"
#include "Vector.hpp"
#include "Material.hpp"
#include "Optional.hpp"

namespace Pht {
    class IEngine;
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
            Pht::SceneObject* mSceneObject {nullptr};
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
        void CreateIcon(const std::string& filename,
                        const Pht::Vec3& position,
                        const Pht::Vec2& size,
                        const Pht::Vec4& color,
                        const Pht::Optional<Pht::Vec4>& shadowColor,
                        const Pht::Optional<Pht::Vec3>& shadowOffset);
        bool IsClicked(const Pht::TouchEvent& event) const;
        
        Pht::SceneObject& GetSceneObject() {
            return *mSceneObject;
        }
        
        const Pht::Button& GetButton() const {
            return *mButton;
        }
        
        void SetPlaySoundIfAudioDisabled(bool playSoundIfAudioDisabled) {
            mPlaySoundIfAudioDisabled = playSoundIfAudioDisabled;
        }

    private:
        Pht::IEngine& mEngine;
        Pht::GuiView& mView;
        std::unique_ptr<Pht::Button> mButton;
        Pht::SceneObject* mSceneObject {nullptr};
        Pht::TextComponent* mText {nullptr};
        Style mStyle;
        bool mPlaySoundIfAudioDisabled {false};
    };
}

#endif
