#include "MenuButton.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "SceneObject.hpp"
#include "QuadMesh.hpp"
#include "SphereMesh.hpp"
#include "GuiView.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"
#include "ObjMesh.hpp"
#include "SceneObjectUtils.hpp"

// Game includes.
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    std::unique_ptr<Pht::SceneObject> CreateMainSceneObject(Pht::ISceneManager& sceneManager,
                                                            Pht::GuiView& view,
                                                            const MenuButton::Style& style) {
        if (style.mRenderableObject) {
            return std::make_unique<Pht::SceneObject>(style.mRenderableObject);
        }

        Pht::Material material {style.mColor, style.mColor, {1.0f, 1.0f, 1.0f}, 20.0f};        
        return sceneManager.CreateSceneObject(Pht::ObjMesh {style.mMeshFilename},
                                              material,
                                              view.GetSceneResources());
    }
}

MenuButton::MenuButton(Pht::IEngine& engine,
                       Pht::GuiView& view,
                       const Pht::Vec3& position,
                       const Pht::Vec2& inputSize,
                       const Style& style) :
    MenuButton {engine, view, view.GetRoot(), position, inputSize, style} {}

MenuButton::MenuButton(Pht::IEngine& engine,
                       Pht::GuiView& view,
                       Pht::SceneObject& parent,
                       const Pht::Vec3& position,
                       const Pht::Vec2& inputSize,
                       const Style& style) :
    mEngine {engine},
    mView {view},
    mStyle {style} {
    
    auto& sceneManager = engine.GetSceneManager();
    
    if (style.mSceneObject) {
        mSceneObject = style.mSceneObject;
    } else {
        auto sceneObject = CreateMainSceneObject(sceneManager, view, style);
        mSceneObject = sceneObject.get();
        mView.GetSceneResources().AddSceneObject(std::move(sceneObject));
        parent.AddChild(*mSceneObject);
    }

    mSceneObject->GetTransform().SetPosition(position);
    mButton = std::make_unique<Pht::Button>(*mSceneObject, inputSize, engine);
    
    if (style.mHasShadow) {
        Pht::Material shaddowMaterial {Pht::Color{0.4f, 0.4f, 0.4f}};
        shaddowMaterial.SetOpacity(0.16f);
        
        auto shadowSceneObject =
            sceneManager.CreateSceneObject(Pht::ObjMesh {style.mMeshFilename},
                                           shaddowMaterial,
                                           view.GetSceneResources());

        shadowSceneObject->GetTransform().SetPosition(Pht::Vec3{-0.15f, -0.15f, -0.1f});
        mSceneObject->AddChild(*shadowSceneObject);
        mView.GetSceneResources().AddSceneObject(std::move(shadowSceneObject));
    }
    
    auto onDownFunction = [this] () {
        Pht::SceneObjectUtils::ScaleRecursively(*mSceneObject, mStyle.mPressedScale);
        if (mStyle.mSelectedRenderableObject) {
            mSceneObject->SetRenderable(mStyle.mSelectedRenderableObject);
        } else {
            if (auto* renderable = mSceneObject->GetRenderable()) {
                renderable->GetMaterial().SetAmbient(mStyle.mSelectedColor);
            }
        }
        if (mText) {
            mText->GetProperties().mScale = mStyle.mTextScale * mStyle.mPressedScale;
        }
    };
    
    mButton->SetOnDown(onDownFunction);
    
    auto onUpFunction = [this] () {
        Pht::SceneObjectUtils::ScaleRecursively(*mSceneObject, 1.0f);
        if (mStyle.mRenderableObject) {
            mSceneObject->SetRenderable(mStyle.mRenderableObject);
        } else {
            if (auto* renderable = mSceneObject->GetRenderable()) {
                renderable->GetMaterial().SetAmbient(mStyle.mColor);
            }
        }
        if (mText) {
            mText->GetProperties().mScale = mStyle.mTextScale;
        }
    };
    
    mButton->SetOnUpInside(onUpFunction);
    mButton->SetOnUpOutside(onUpFunction);
    mButton->SetOnMoveOutside(onUpFunction);
}

Pht::TextComponent& MenuButton::CreateText(const Pht::Vec3& position,
                                           const std::string& text,
                                           const Pht::TextProperties& properties) {
    auto sceneObject = std::make_unique<Pht::SceneObject>();
    auto textComponent = std::make_unique<Pht::TextComponent>(*sceneObject, text, properties);
    
    mText = textComponent.get();
    sceneObject->SetComponent<Pht::TextComponent>(std::move(textComponent));
    sceneObject->GetTransform().SetPosition(position);
    mSceneObject->AddChild(*sceneObject);
    
    mView.GetSceneResources().AddSceneObject(std::move(sceneObject));
    return *mText;
}

Pht::SceneObject& MenuButton::CreateIcon(const std::string& filename,
                                         const Pht::Vec3& position,
                                         const Pht::Vec2& size,
                                         const Pht::Vec4& color,
                                         const Pht::Optional<Pht::Vec4>& shadowColor,
                                         const Pht::Optional<Pht::Vec3>& shadowOffset) {
    auto containerSceneObject = std::make_unique<Pht::SceneObject>();
    auto& container = *containerSceneObject;
    containerSceneObject->GetTransform().SetPosition(position);
    mSceneObject->AddChild(*containerSceneObject);
    mView.GetSceneResources().AddSceneObject(std::move(containerSceneObject));
    
    auto& sceneManager = mEngine.GetSceneManager();

    Pht::Material iconMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
    iconMaterial.SetBlend(Pht::Blend::Yes);
    iconMaterial.SetOpacity(color.w);
    iconMaterial.SetAmbient(Pht::Color{color.x, color.y, color.z});
    
    auto iconSceneObject =
        sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                       iconMaterial,
                                       mView.GetSceneResources());
    
    container.AddChild(*iconSceneObject);
    mView.GetSceneResources().AddSceneObject(std::move(iconSceneObject));

    if (shadowColor.HasValue()) {
        auto& shadowColorValue = shadowColor.GetValue();
        Pht::Material shadowMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
        shadowMaterial.SetBlend(Pht::Blend::Yes);
        shadowMaterial.SetOpacity(shadowColorValue.w);
        shadowMaterial.SetAmbient(Pht::Color{shadowColorValue.x, shadowColorValue.y, shadowColorValue.z});
        
        auto shadowSceneObject =
            sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                           shadowMaterial,
                                           mView.GetSceneResources());
        
        shadowSceneObject->GetTransform().SetPosition(shadowOffset.GetValue());
        container.AddChild(*shadowSceneObject);
        mView.GetSceneResources().AddSceneObject(std::move(shadowSceneObject));
    }
    
    return container;
}

bool MenuButton::IsClicked(const Pht::TouchEvent& event) const {
    auto isClicked {mButton->IsClicked(event)};
    if (isClicked) {
        auto& audio {mEngine.GetAudio()};
        auto shouldToggleSound {mPlaySoundIfAudioDisabled && !audio.IsSoundEnabled()};
        if (shouldToggleSound) {
            audio.EnableSound();
        }
            
        audio.PlaySound(static_cast<Pht::AudioResourceId>(SoundId::ButtonClick));
        
        if (shouldToggleSound) {
            audio.DisableSound();
        }
    }
    
    return isClicked;
}
