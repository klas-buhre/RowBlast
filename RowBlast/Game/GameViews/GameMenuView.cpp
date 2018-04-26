#include "GameMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    const Pht::Color buttonColor {0.4f, 0.74f, 1.0f};
}

GameMenuView::GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes)
    };

    Pht::TextProperties pausedTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes)
    };
    
    auto frustumWidth {engine.GetRenderer().GetHudFrustumSize().x};
    Pht::Vec2 size {frustumWidth, 19.0f};
    SetSize(size);
    SetPosition({0.0f, 0.0f});

    auto quad {MenuQuad::CreateGray(engine, GetSceneResources(), size)};
    quad->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(quad));

    Pht::Material lineMaterial {Pht::Color{1.0f, 1.0f, 1.0f}};
    lineMaterial.SetOpacity(0.4f);
    auto& sceneManager {engine.GetSceneManager()};
    auto lineSceneObject {
        sceneManager.CreateSceneObject(Pht::QuadMesh {frustumWidth - 1.0f, 0.08f},
                                       lineMaterial,
                                       GetSceneResources())
    };
    lineSceneObject->GetTransform().SetPosition({0.0f, 6.0f, UiLayer::textRectangle});
    AddSceneObject(std::move(lineSceneObject));
    
    CreateText({-1.71f, 7.4f, UiLayer::text}, "PAUSED", pausedTextProperties);
    
    Pht::Vec2 buttonSize {10.0f, 1.72f};
    Pht::Vec2 buttonInputSize {215.0f, 43.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = buttonColor;
    buttonStyle.mSelectedColor = Pht::Color {0.6f, 0.94f, 1.0f};
    buttonStyle.mPressedScale = 0.925f;
    buttonStyle.mOpacity = 0.76f;
    buttonStyle.mIsRounded = false;

    mResumeButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, 3.6f, UiLayer::textRectangle},
                                                 buttonSize,
                                                 buttonInputSize,
                                                 buttonStyle);
    mResumeButton->CreateText({-1.4f, -0.23f, UiLayer::buttonText}, "RESUME", buttonTextProperties);
    
    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 1.3f, UiLayer::textRectangle},
                                               buttonSize,
                                               buttonInputSize,
                                               buttonStyle);
    mUndoButton->CreateText({-2.1f, -0.23f, UiLayer::buttonText}, "UNDO MOVE", buttonTextProperties);
    
    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -1.0f, UiLayer::textRectangle},
                                                  buttonSize,
                                                  buttonInputSize,
                                                  buttonStyle);
    mRestartButton->CreateText({-1.5f, -0.23f, UiLayer::buttonText}, "RESTART", buttonTextProperties);

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -3.3f, UiLayer::textRectangle},
                                                   buttonSize,
                                                   buttonInputSize,
                                                   buttonStyle);
    mSettingsButton->CreateText({-1.6f, -0.23f, UiLayer::buttonText}, "SETTINGS", buttonTextProperties);

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -5.6f, UiLayer::textRectangle},
                                              buttonSize,
                                              buttonInputSize,
                                              buttonStyle);
    mMapButton->CreateText({-0.8f, -0.23f, UiLayer::buttonText}, "MAP", buttonTextProperties);
}

void GameMenuView::EnableUndoButton() {
    mIsUndoButtonEnabled = true;
    mUndoButton->GetSceneObject().GetRenderable()->GetMaterial().SetAmbient(buttonColor);
}

void GameMenuView::DisableUndoButton() {
    mIsUndoButtonEnabled = false;
    
    Pht::Color greyColor {0.65f, 0.65f, 0.65f};
    mUndoButton->GetSceneObject().GetRenderable()->GetMaterial().SetAmbient(greyColor);
}
