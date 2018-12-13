#include "GameMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "IGuiLightProvider.hpp"

using namespace RowBlast;

GameMenuView::GameMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    SetPosition({0.0f, 0.0f});
    
    Pht::TextProperties captionTextProperties {
        commonResources.GetHussarFontSize35(zoom),
        1.2f,
        {0.23f, 0.23f, 0.23f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f},
        Pht::SnapToPixel::Yes,
        0.125f
    };
    
    CreateText({-1.85f, 8.0f, UiLayer::text}, "PAUSE", captionTextProperties);

    MenuButton::Style yellowButtonStyle;
    yellowButtonStyle.mMeshFilename = GuiResources::mMediumButtonSkewedMeshFilename;
    yellowButtonStyle.mColor = GuiResources::mYellowButtonColor;
    yellowButtonStyle.mSelectedColor = GuiResources::mYellowSelectedButtonColor;
    yellowButtonStyle.mPressedScale = 1.05f;
    yellowButtonStyle.mHasShadow = true;

    MenuButton::Style blueButtonStyle;
    blueButtonStyle.mMeshFilename = GuiResources::mMediumButtonSkewedMeshFilename;
    blueButtonStyle.mColor = GuiResources::mBlueButtonColor;
    blueButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    blueButtonStyle.mPressedScale = 1.05f;
    blueButtonStyle.mHasShadow = true;
    
    Pht::Vec2 buttonInputSize {194.0f, 45.0f};

    mResumeButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, 5.1f, UiLayer::textRectangle},
                                                 buttonInputSize,
                                                 yellowButtonStyle);
    mResumeButton->CreateText({-1.25f, -0.23f, UiLayer::buttonText},
                              "Resume",
                              guiResources.GetBlackButtonTextProperties(zoom));
    
    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 2.5f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               blueButtonStyle);
    mUndoButton->CreateText({-1.9f, -0.23f, UiLayer::buttonText},
                            "Undo Move",
                            guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mGoalButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -0.1f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               blueButtonStyle);
    mGoalButton->CreateText({-1.0f, -0.23f, UiLayer::buttonText},
                            "Goal",
                            guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -2.7f, UiLayer::textRectangle},
                                                   buttonInputSize,
                                                   blueButtonStyle);
    mSettingsButton->CreateText({-1.4f, -0.23f, UiLayer::buttonText},
                                "Settings",
                                guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -5.3f, UiLayer::textRectangle},
                                                  buttonInputSize,
                                                  blueButtonStyle);
    mRestartButton->CreateText({-1.3f, -0.23f, UiLayer::buttonText},
                               "Restart",
                               guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -7.9f, UiLayer::textRectangle},
                                              buttonInputSize,
                                              blueButtonStyle);
    mMapButton->CreateText({-0.8f, -0.23f, UiLayer::buttonText},
                           "Map",
                           guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());
}

void GameMenuView::SetUp() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void GameMenuView::EnableUndoButton() {
    mIsUndoButtonEnabled = true;
    
    auto& material {mUndoButton->GetSceneObject().GetRenderable()->GetMaterial()};
    material.SetAmbient(GuiResources::mBlueButtonColor);
    material.SetDiffuse(GuiResources::mBlueButtonColor);
}

void GameMenuView::DisableUndoButton() {
    mIsUndoButtonEnabled = false;
    
    Pht::Color greyColor {0.4f, 0.4f, 0.4f};
    auto& material {mUndoButton->GetSceneObject().GetRenderable()->GetMaterial()};
    material.SetAmbient(greyColor);
    material.SetDiffuse(greyColor);
}
