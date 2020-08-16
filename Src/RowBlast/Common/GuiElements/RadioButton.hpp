#ifndef RadioButton_hpp
#define RadioButton_hpp

#include <memory>

// Engine includes.
#include "Button.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class TouchEvent;
    class GuiView;
}

namespace RowBlast {
    class RadioButton {
    public:
        RadioButton(Pht::IEngine& engine,
                    Pht::GuiView& view,
                    Pht::SceneObject& clickableObject,
                    const Pht::Vec3& positionInClickableObject,
                    const Pht::Vec2& inputSize);
        
        bool IsClicked(const Pht::TouchEvent& event);
        void SetIsSelected(bool isSelected);
        
        bool IsSelected() {
            return mIsSelected;
        }
        
    private:
        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::Button> mButton;
        Pht::SceneObject* mRadioButtonSceneObject {nullptr};
        std::unique_ptr<Pht::RenderableObject> mSelectedRenderable;
        std::unique_ptr<Pht::RenderableObject> mNotSelectedRenderable;
        bool mIsSelected {false};
    };
}

#endif
