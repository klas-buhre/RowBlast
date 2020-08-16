#include "RadioButton.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "ISceneManager.hpp"
#include "QuadMesh.hpp"
#include "GuiView.hpp"

// Game includes.
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    std::unique_ptr<Pht::RenderableObject> CreateIconRenderable(Pht::IEngine& engine,
                                                                const std::string& filename) {
        Pht::Material iconMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
        iconMaterial.SetBlend(Pht::Blend::Yes);
        iconMaterial.SetAmbient(Pht::Color{0.95f, 0.95f, 0.95f});
        
        auto& sceneManager = engine.GetSceneManager();
        return sceneManager.CreateRenderableObject(Pht::QuadMesh {1.1f, 1.1f, std::string{"radioButton"}},
                                                   iconMaterial);
    }
}

RadioButton::RadioButton(Pht::IEngine& engine,
                         Pht::GuiView& view,
                         Pht::SceneObject& clickableObject,
                         const Pht::Vec3& positionInClickableObject,
                         const Pht::Vec2& inputSize) :
    mEngine {engine} {
    
    mButton = std::make_unique<Pht::Button>(clickableObject, inputSize, engine);
    
    mRadioButtonSceneObject = &view.CreateSceneObject();
    mRadioButtonSceneObject->GetTransform().SetPosition(positionInClickableObject);
    clickableObject.AddChild(*mRadioButtonSceneObject);
    
    mSelectedRenderable = CreateIconRenderable(engine, "radio_button_selected.png");
    mNotSelectedRenderable = CreateIconRenderable(engine, "circle.png");
    mRadioButtonSceneObject->SetRenderable(mNotSelectedRenderable.get());
}

bool RadioButton::IsClicked(const Pht::TouchEvent& event) {
    auto isClicked = mButton->IsClicked(event);
    if (isClicked) {
        mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::ButtonClick));
        
        mIsSelected = !mIsSelected;
        if (mIsSelected) {
            mRadioButtonSceneObject->SetRenderable(mSelectedRenderable.get());
        }
    }
    
    return isClicked;
}

void RadioButton::SetIsSelected(bool isSelected) {
    mIsSelected = isSelected;
    if (isSelected) {
        mRadioButtonSceneObject->SetRenderable(mSelectedRenderable.get());
    } else {
        mRadioButtonSceneObject->SetRenderable(mNotSelectedRenderable.get());
    }
}
