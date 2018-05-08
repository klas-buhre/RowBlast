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
    const Pht::Color buttonColor {0.68f, 0.535f, 0.0f};
}

GameMenuView::GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.3f, 0.3f, 0.3f, 1.0f},
    };

    Pht::TextProperties pausedTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.2f, 0.2f, 0.2f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };

    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    SetPosition({0.0f, 0.0f});
/*
    auto frustumWidth {engine.GetRenderer().GetHudFrustumSize().x};
    Pht::Vec2 size {frustumWidth, 19.0f};
    SetSize(size);
    SetPosition({0.0f, 0.0f});

    auto quad {MenuQuad::CreateGray(engine, GetSceneResources(), size)};
    quad->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(quad));
*/
    auto frustumWidth {engine.GetRenderer().GetHudFrustumSize().x};

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
    
    // Pht::Vec2 buttonSize {10.0f, 1.72f};
    Pht::Vec2 buttonSize {8.5f, 2.1f};
    Pht::Vec2 buttonInputSize {215.0f, 43.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mColor = buttonColor;
    buttonStyle.mSelectedColor = Pht::Color {0.55f, 0.45f, 0.0f};
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mOpacity = 1.0f;
    buttonStyle.mHasShadow = true;
    buttonStyle.mIsRounded = false;

    mResumeButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, 4.2f, UiLayer::textRectangle},
                                                 buttonSize,
                                                 buttonInputSize,
                                                 buttonStyle);
    mResumeButton->CreateText({-1.25f, -0.23f, UiLayer::buttonText}, "Resume", buttonTextProperties);
    
    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 1.6f, UiLayer::textRectangle},
                                               buttonSize,
                                               buttonInputSize,
                                               buttonStyle);
    mUndoButton->CreateText({-1.9f, -0.23f, UiLayer::buttonText}, "Undo Move", buttonTextProperties);
    
    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -1.0f, UiLayer::textRectangle},
                                                  buttonSize,
                                                  buttonInputSize,
                                                  buttonStyle);
    mRestartButton->CreateText({-1.2f, -0.23f, UiLayer::buttonText}, "Restart", buttonTextProperties);

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -3.6f, UiLayer::textRectangle},
                                                   buttonSize,
                                                   buttonInputSize,
                                                   buttonStyle);
    mSettingsButton->CreateText({-1.3f, -0.23f, UiLayer::buttonText}, "Settings", buttonTextProperties);

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -6.2f, UiLayer::textRectangle},
                                              buttonSize,
                                              buttonInputSize,
                                              buttonStyle);
    mMapButton->CreateText({-0.7f, -0.23f, UiLayer::buttonText}, "Map", buttonTextProperties);
}

void GameMenuView::EnableUndoButton() {
    mIsUndoButtonEnabled = true;
    mUndoButton->GetSceneObject().GetRenderable()->GetMaterial().SetAmbient(buttonColor);
    mUndoButton->GetSceneObject().GetRenderable()->GetMaterial().SetDiffuse(buttonColor);
}

void GameMenuView::DisableUndoButton() {
    mIsUndoButtonEnabled = false;
    
    Pht::Color greyColor {0.45f, 0.45f, 0.45f};
    mUndoButton->GetSceneObject().GetRenderable()->GetMaterial().SetAmbient(greyColor);
    mUndoButton->GetSceneObject().GetRenderable()->GetMaterial().SetDiffuse(greyColor);
}
