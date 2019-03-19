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
    
    CreateText({-1.85f, 9.2f, UiLayer::text}, "PAUSE", captionTextProperties);

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
    
    Pht::Vec4 iconColor {1.0f, 1.0f, 1.0f, 1.0f};
    Pht::Vec4 iconShadowColor {0.2f, 0.2f, 0.2f, 0.5f};
    Pht::Vec3 iconShadowOffset {-0.05f, -0.05f, -0.1f};

    mResumeButton = std::make_unique<MenuButton>(engine,
                                                 *this,
                                                 Pht::Vec3 {0.0f, 6.4f, UiLayer::textRectangle},
                                                 buttonInputSize,
                                                 yellowButtonStyle);
    mResumeButton->CreateIcon("play.png",
                              {-1.57f, 0.03f, UiLayer::buttonText},
                              {0.65f, 0.65f},
                              {0.3f, 0.3f, 0.3f, 1.0f},
                              Pht::Optional<Pht::Vec4> {},
                              Pht::Optional<Pht::Vec3> {});
    mResumeButton->CreateText({-1.02f, -0.23f, UiLayer::buttonText},
                              "Resume",
                              guiResources.GetBlackButtonTextProperties(zoom));
    
    mUndoButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, 3.8f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               blueButtonStyle);
    mUndoButton->CreateIcon("undo.png",
                            {-2.2f, 0.07f, UiLayer::buttonText},
                            {0.82f, 0.82f},
                            iconColor,
                            iconShadowColor,
                            iconShadowOffset);
    mUndoButton->CreateText({-1.55f, -0.23f, UiLayer::buttonText},
                            "Undo Move",
                            guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());


    mHowToPlayButton = std::make_unique<MenuButton>(engine,
                                                    *this,
                                                    Pht::Vec3 {0.0f, 1.2f, UiLayer::textRectangle},
                                                    buttonInputSize,
                                                    blueButtonStyle);
    mHowToPlayButton->CreateIcon("help.png",
                                 {-2.5f, 0.05f, UiLayer::buttonText},
                                 {0.82f, 0.82f},
                                 iconColor,
                                 iconShadowColor,
                                 iconShadowOffset);
    mHowToPlayButton->CreateText({-1.8f, -0.23f, UiLayer::buttonText},
                                 "How To Play",
                                 guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mLevelInfoButton = std::make_unique<MenuButton>(engine,
                                                    *this,
                                                    Pht::Vec3 {0.0f, -1.4f, UiLayer::textRectangle},
                                                    buttonInputSize,
                                                    blueButtonStyle);
    mLevelInfoButton->CreateIcon("info.png",
                                 {-2.0f, 0.08f, UiLayer::buttonText},
                                 {0.82f, 0.82f},
                                 iconColor,
                                 iconShadowColor,
                                 iconShadowOffset);
    mLevelInfoButton->CreateText({-1.3f, -0.23f, UiLayer::buttonText},
                                 "Level Info",
                                 guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   Pht::Vec3 {0.0f, -4.0f, UiLayer::textRectangle},
                                                   buttonInputSize,
                                                   blueButtonStyle);
    mSettingsButton->CreateIcon("settings.png",
                                {-1.7f, 0.09f, UiLayer::buttonText},
                                {0.77f, 0.77f},
                                iconColor,
                                iconShadowColor,
                                iconShadowOffset);
    mSettingsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                "Settings",
                                guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mRestartButton = std::make_unique<MenuButton>(engine,
                                                  *this,
                                                  Pht::Vec3 {0.0f, -6.6f, UiLayer::textRectangle},
                                                  buttonInputSize,
                                                  blueButtonStyle);
    mRestartButton->CreateIcon("restart.png",
                               {-1.6f, 0.1f, UiLayer::buttonText},
                               {0.82f, 0.82f},
                               iconColor,
                               iconShadowColor,
                               iconShadowOffset);
    mRestartButton->CreateText({-0.95f, -0.23f, UiLayer::buttonText},
                               "Restart",
                               guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

    mMapButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -9.2f, UiLayer::textRectangle},
                                              buttonInputSize,
                                              blueButtonStyle);
    mMapButton->CreateIcon("home.png",
                           {-1.15f, 0.07f, UiLayer::buttonText},
                           {0.65f, 0.65f},
                           iconColor,
                           iconShadowColor,
                           iconShadowOffset);
    mMapButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
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
