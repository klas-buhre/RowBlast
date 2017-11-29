#include "SettingsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"

using namespace BlocksGame;

SettingsMenuView::SettingsMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::TextProperties textProperties {commonResources.GetHussarFontSize27()};
    Pht::TextProperties settingsTextProperties {commonResources.GetHussarFontSize30()};
    
    Pht::Vec2 size {engine.GetRenderer().GetHudFrustumSize().x, 12.6f};
    
    SetSize(size);
    SetPosition({0.0f, 0.0f});
    SetDepthTest(false);
    
    auto quad {MenuQuad::CreateGray(engine, size)};
    quad->SetPosition({0.0f, 0.0f, -1.0f});
    AddSceneObject(std::move(quad));
    
    Pht::Material lineMaterial {Pht::Color{1.0f, 1.0f, 1.0f}};
    lineMaterial.SetOpacity(0.4f);
    auto& sceneManager {engine.GetSceneManager()};
    auto lineSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {size.x - 1.0f, 0.08f}, lineMaterial)
    };
    lineSceneObject->SetPosition({0.0f, 3.3f, 0.0f});
    AddSceneObject(std::move(lineSceneObject));
    
    AddText(std::make_unique<Pht::Text>(Pht::Vec2 {-2.0f, 4.3f}, "SETTINGS", settingsTextProperties));
    
    Pht::Material barMaterial {Pht::Color{0.4f, 0.74f, 1.0f}};
    barMaterial.SetOpacity(0.24f);
    auto controlsBarSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {7.0f, 1.72f}, barMaterial)
    };
    controlsBarSceneObject->SetPosition({-2.0f, 1.3f, -0.5f});
    AddSceneObject(std::move(controlsBarSceneObject));
    
    AddText(std::make_unique<Pht::Text>(Pht::Vec2 {-4.65f, 1.07f}, "CONTROLS", textProperties));
    
    Pht::Vec2 buttonSize {4.0f, 1.72f};
    Pht::Vec2 buttonInputSize {86.0f, 43.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = Pht::Color {0.4f, 0.74f, 1.0f};
    buttonStyle.mSelectedColor = Pht::Color {0.6f, 0.94f, 1.0f};
    buttonStyle.mOpacity = 0.76f;
    buttonStyle.mIsRounded = false;
    
    Pht::Vec3 controlsButtonPosition {3.5f, 1.3f, -0.5f};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  controlsButtonPosition,
                                                  buttonSize,
                                                  buttonInputSize,
                                                  buttonStyle);
    auto controlsClickText {
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.05f, -0.23f}, "CLICK", textProperties)
    };
        
    mControlsClickText = controlsClickText.get();
    mControlsButton->SetText(std::move(controlsClickText));
    
    auto controlsSwipeText {
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.05f, -0.23f}, "SWIPE", textProperties)
    };

    mControlsSwipeText = controlsSwipeText.get();
    mControlsSwipeText->mIsVisible = false;
    mControlsButton->SetText(std::move(controlsSwipeText));

    auto soundBarSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {7.0f, 1.72f}, barMaterial)
    };
    soundBarSceneObject->SetPosition({-2.0f, -1.3f, -0.5f});
    AddSceneObject(std::move(soundBarSceneObject));
        
    AddText(std::make_unique<Pht::Text>(Pht::Vec2 {-4.65f, -1.53f}, "SOUND", textProperties));
    
    Pht::Vec3 soundButtonPosition {3.5f, -1.3f, -0.5f};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                soundButtonPosition,
                                                buttonSize,
                                                buttonInputSize,
                                                buttonStyle);
    auto soundOnText {
        std::make_unique<Pht::Text>(Pht::Vec2 {-0.6f, -0.23f}, "ON", textProperties)
    };
    
    mSoundOnText = soundOnText.get();
    mSoundButton->SetText(std::move(soundOnText));
    
    auto soundOffText {
        std::make_unique<Pht::Text>(Pht::Vec2 {-0.7f, -0.23f}, "OFF", textProperties)
    };

    mSoundOffText = soundOffText.get();
    mSoundOffText->mIsVisible = false;
    mSoundButton->SetText(std::move(soundOffText));

    MenuButton::Style backButtonStyle {buttonStyle};
    backButtonStyle.mPressedScale = 0.925f;
    
    Pht::Vec2 backButtonSize {11.0f, 1.72f};
    Pht::Vec2 backButtonInputSize {236.0f, 43.0f};
    
    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -3.9f, -0.5f},
                                               backButtonSize,
                                               backButtonInputSize,
                                               backButtonStyle);
    mBackButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.0f, -0.23f}, "BACK", textProperties));
}
