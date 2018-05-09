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

GameMenuView::GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
/*
    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.3f, 0.3f, 0.3f, 1.0f}
    };
*/
    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
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
    
    CreateText({-1.71f, 7.4f, UiLayer::text}, "PAUSED", pausedTextProperties);
    
    Pht::Vec2 buttonInputSize {183.0f, 45.0f};
    MenuButton::Style buttonStyle;
    buttonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    buttonStyle.mColor = GuiResources::mBlueButtonColor;
    buttonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mHasShadow = true;

    mResumeButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, 4.7f, UiLayer::textRectangle},
                                                 buttonInputSize,
                                                 buttonStyle);
    mResumeButton->CreateText({-1.25f, -0.23f, UiLayer::buttonText}, "Resume", buttonTextProperties);
    
    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 2.1f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               buttonStyle);
    mUndoButton->CreateText({-1.9f, -0.23f, UiLayer::buttonText}, "Undo Move", buttonTextProperties);
    
    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -0.5f, UiLayer::textRectangle},
                                                  buttonInputSize,
                                                  buttonStyle);
    mRestartButton->CreateText({-1.2f, -0.23f, UiLayer::buttonText}, "Restart", buttonTextProperties);

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -3.1f, UiLayer::textRectangle},
                                                   buttonInputSize,
                                                   buttonStyle);
    mSettingsButton->CreateText({-1.3f, -0.23f, UiLayer::buttonText}, "Settings", buttonTextProperties);

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -5.7f, UiLayer::textRectangle},
                                              buttonInputSize,
                                              buttonStyle);
    mMapButton->CreateText({-0.7f, -0.23f, UiLayer::buttonText}, "Map", buttonTextProperties);
}

void GameMenuView::EnableUndoButton() {
    mIsUndoButtonEnabled = true;
    
    auto& material {mUndoButton->GetSceneObject().GetRenderable()->GetMaterial()};
    material.SetAmbient(GuiResources::mBlueButtonColor);
    material.SetDiffuse(GuiResources::mBlueButtonColor);
}

void GameMenuView::DisableUndoButton() {
    mIsUndoButtonEnabled = false;
    
    Pht::Color greyColor {0.45f, 0.45f, 0.45f};
    auto& material {mUndoButton->GetSceneObject().GetRenderable()->GetMaterial()};
    material.SetAmbient(greyColor);
    material.SetDiffuse(greyColor);
}
