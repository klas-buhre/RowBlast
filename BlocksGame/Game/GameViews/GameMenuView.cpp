#include "GameMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "MenuQuad.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Color buttonColor {0.4f, 0.74f, 1.0f};
}

GameMenuView::GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::TextProperties buttonTextProperties {commonResources.GetHussarFontSize27()};
    Pht::TextProperties pausedTextProperties {commonResources.GetHussarFontSize30()};
    
    auto frustumWidth {engine.GetRenderer().GetHudFrustumSize().x};
    Pht::Vec2 size {frustumWidth, 19.0f};
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
        sceneManager.CreateSceneObject(Pht::QuadMesh {frustumWidth - 1.0f, 0.08f}, lineMaterial)
    };
    lineSceneObject->SetPosition({0.0f, 6.0f, 0.0f});
    AddSceneObject(std::move(lineSceneObject));
    
    auto pausedText {
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.71f, 7.4f}, "PAUSED", pausedTextProperties)
    };
    AddText(std::move(pausedText));

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
                                                 Pht::Vec3 {0.0f, 3.6f, -0.5f},
                                                 buttonSize,
                                                 buttonInputSize,
                                                 buttonStyle);
    mResumeButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.4f, -0.23f}, "RESUME", buttonTextProperties));

    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 1.3f, -0.5f},
                                               buttonSize,
                                               buttonInputSize,
                                               buttonStyle);
    mUndoButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-2.1f, -0.23f}, "UNDO MOVE", buttonTextProperties));
    
    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -1.0f, -0.5f},
                                                  buttonSize,
                                                  buttonInputSize,
                                                  buttonStyle);
    mRestartButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.5f, -0.23f}, "RESTART", buttonTextProperties));

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -3.3f, -0.5f},
                                                   buttonSize,
                                                   buttonInputSize,
                                                   buttonStyle);
    mSettingsButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-1.6f, -0.23f}, "SETTINGS", buttonTextProperties));

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -5.6f, -0.5f},
                                              buttonSize,
                                              buttonInputSize,
                                              buttonStyle);
    mMapButton->SetText(
        std::make_unique<Pht::Text>(Pht::Vec2 {-0.8f, -0.23f}, "MAP", buttonTextProperties));
}

void GameMenuView::EnableUndoButton() {
    mIsUndoButtonEnabled = true;

    for (auto sceneObject: mUndoButton->GetSceneObjects()) {
        sceneObject->GetRenderable()->GetMaterial().SetAmbient(buttonColor);
    }
}

void GameMenuView::DisableUndoButton() {
    mIsUndoButtonEnabled = false;
    
    Pht::Color greyColor {0.65f, 0.65f, 0.65f};
    
    for (auto sceneObject: mUndoButton->GetSceneObjects()) {
        sceneObject->GetRenderable()->GetMaterial().SetAmbient(greyColor);
    }
}
