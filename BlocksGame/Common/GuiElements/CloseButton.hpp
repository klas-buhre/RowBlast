#ifndef CloseButton_hpp
#define CloseButton_hpp

#include <memory>

// Engine includes.
#include "Button.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class GuiView;
    class TouchEvent;
    class Text;
    class SceneObject;
    class IAudio;
}

namespace BlocksGame {
    class CloseButton {
    public:
        CloseButton(Pht::IEngine& engine,
                    Pht::GuiView& view,
                    const Pht::Vec3& position);
        
        void SetText(std::unique_ptr<Pht::Text> text);
        bool IsClicked(const Pht::TouchEvent& event) const;
        
    private:
        Pht::GuiView& mView;
        Pht::Vec3 mPosition;
        std::unique_ptr<Pht::Button> mButton;
        Pht::SceneObject* mSceneObject;
        Pht::IAudio& mAudio;
    };
}

#endif
