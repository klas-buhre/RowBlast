#include "SettingsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"
#include "UiLayer.hpp"

using namespace BlocksGame;

SettingsMenuView::SettingsMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::TextProperties textProperties {commonResources.GetHussarFontSize27()};
    Pht::TextProperties settingsTextProperties {commonResources.GetHussarFontSize30()};
    
    Pht::Vec2 size {engine.GetRenderer().GetHudFrustumSize().x, 12.6f};
    
    SetSize(size);
    SetPosition({0.0f, 0.0f});
    
    auto quad {MenuQuad::CreateGray(engine, GetSceneResources(), size)};
    quad->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(quad));
    
    Pht::Material lineMaterial {Pht::Color{1.0f, 1.0f, 1.0f}};
    lineMaterial.SetOpacity(0.4f);
    auto& sceneManager {engine.GetSceneManager()};
    auto lineSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {size.x - 1.0f, 0.08f},
                                       lineMaterial,
                                       GetSceneResources())
    };
    lineSceneObject->GetTransform().SetPosition({0.0f, 3.3f, UiLayer::textRectangle});
    AddSceneObject(std::move(lineSceneObject));
    
    CreateText({-2.0f, 4.3f, UiLayer::text}, "SETTINGS", settingsTextProperties);
    
    Pht::Material barMaterial {Pht::Color{0.4f, 0.74f, 1.0f}};
    barMaterial.SetOpacity(0.24f);
    auto controlsBarSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {7.0f, 1.72f}, barMaterial, GetSceneResources())
    };
    controlsBarSceneObject->GetTransform().SetPosition({-2.0f, 1.3f, UiLayer::textRectangle});
    AddSceneObject(std::move(controlsBarSceneObject));
    
    CreateText({-4.65f, 1.07f, UiLayer::text}, "CONTROLS", textProperties);
    
    Pht::Vec2 buttonSize {4.0f, 1.72f};
    Pht::Vec2 buttonInputSize {86.0f, 43.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.4f, 0.74f, 1.0f};
    buttonStyle.mSelectedColor = Pht::Color {0.6f, 0.94f, 1.0f};
    buttonStyle.mOpacity = 0.76f;
    buttonStyle.mIsRounded = false;
    
    Pht::Vec3 controlsButtonPosition {3.5f, 1.3f, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   controlsButtonPosition,
                                                   buttonSize,
                                                   buttonInputSize,
                                                   buttonStyle);
    mControlsClickText = &(mControlsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                                       "CLICK",
                                                       textProperties).GetSceneObject());
    mControlsSwipeText = &(mControlsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                                       "SWIPE",
                                                       textProperties).GetSceneObject());
    mControlsSwipeText->SetIsVisible(false);

    auto soundBarSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {7.0f, 1.72f}, barMaterial, GetSceneResources())
    };
    soundBarSceneObject->GetTransform().SetPosition({-2.0f, -1.3f, UiLayer::textRectangle});
    AddSceneObject(std::move(soundBarSceneObject));
        
    CreateText({-4.65f, -1.53f, UiLayer::text}, "SOUND", textProperties);
    
    Pht::Vec3 soundButtonPosition {3.5f, -1.3f, UiLayer::textRectangle};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                soundButtonPosition,
                                                buttonSize,
                                                buttonInputSize,
                                                buttonStyle);
    mSoundOnText = &(mSoundButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText},
                                              "ON",
                                              textProperties).GetSceneObject());
    mSoundOffText = &(mSoundButton->CreateText({-0.7f, -0.23f, UiLayer::buttonText},
                                               "OFF",
                                               textProperties).GetSceneObject());
    mSoundOffText->SetIsVisible(false);

    MenuButton::Style backButtonStyle {buttonStyle};
    backButtonStyle.mPressedScale = 0.925f;
    
    Pht::Vec2 backButtonSize {11.0f, 1.72f};
    Pht::Vec2 backButtonInputSize {236.0f, 43.0f};
    
    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -3.9f, UiLayer::textRectangle},
                                               backButtonSize,
                                               backButtonInputSize,
                                               backButtonStyle);
    mBackButton->CreateText({-1.0f, -0.23f, UiLayer::buttonText}, "BACK", textProperties);
}
