#include "GameMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

GameMenuView::GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeMenuWindowPotentiallyZoomedScreen()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    SetPosition({0.0f, 0.0f});
    
    CreateText({-1.71f, 7.6f, UiLayer::text}, "PAUSED", guiResources.GetCaptionTextProperties(zoom));

    MenuButton::Style yellowButtonStyle;
    yellowButtonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    yellowButtonStyle.mColor = GuiResources::mYellowButtonColor;
    yellowButtonStyle.mSelectedColor = GuiResources::mYellowSelectedButtonColor;
    yellowButtonStyle.mPressedScale = 1.05f;
    yellowButtonStyle.mHasShadow = true;

    MenuButton::Style blueButtonStyle;
    blueButtonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    blueButtonStyle.mColor = GuiResources::mBlueButtonColor;
    blueButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    blueButtonStyle.mPressedScale = 1.05f;
    blueButtonStyle.mHasShadow = true;
    
    Pht::Vec2 buttonInputSize {194.0f, 45.0f};

    mResumeButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, 4.7f, UiLayer::textRectangle},
                                                 buttonInputSize,
                                                 yellowButtonStyle);
    mResumeButton->CreateText({-1.25f, -0.23f, UiLayer::buttonText},
                              "Resume",
                              guiResources.GetBlackButtonTextProperties(zoom));
    
    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 2.1f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               blueButtonStyle);
    mUndoButton->CreateText({-1.9f, -0.23f, UiLayer::buttonText},
                            "Undo Move",
                            guiResources.GetWhiteButtonTextProperties(zoom));

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -0.5f, UiLayer::textRectangle},
                                                   buttonInputSize,
                                                   blueButtonStyle);
    mSettingsButton->CreateText({-1.3f, -0.23f, UiLayer::buttonText},
                                "Settings",
                                guiResources.GetWhiteButtonTextProperties(zoom));

    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -3.1f, UiLayer::textRectangle},
                                                  buttonInputSize,
                                                  blueButtonStyle);
    mRestartButton->CreateText({-1.3f, -0.23f, UiLayer::buttonText},
                               "Restart",
                               guiResources.GetWhiteButtonTextProperties(zoom));

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -5.7f, UiLayer::textRectangle},
                                              buttonInputSize,
                                              blueButtonStyle);
    mMapButton->CreateText({-0.8f, -0.23f, UiLayer::buttonText},
                           "Map",
                           guiResources.GetWhiteButtonTextProperties(zoom));
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
