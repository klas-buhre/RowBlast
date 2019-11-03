#include "RestartConfirmationDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

RestartConfirmationDialogView::RestartConfirmationDialogView(Pht::IEngine& engine,
                                                             const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetMediumMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());

    CreateText({-2.0f, 4.25f, UiLayer::text}, "RESTART", guiResources.GetCaptionTextProperties(zoom));
    
    auto textProperties = guiResources.GetSmallTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    CreateText({0.0f, 1.9f, UiLayer::text}, "Are you sure you want to", textProperties);
    CreateText({0.0f, 0.9f, UiLayer::text}, "restart the level?", textProperties);

    Pht::Vec2 buttonInputSize {194.0f, 43.0f};
    MenuButton::Style yellowButtonStyle;
    yellowButtonStyle.mMeshFilename = GuiResources::mMediumButtonSkewedMeshFilename;
    yellowButtonStyle.mColor = GuiResources::mYellowButtonColor;
    yellowButtonStyle.mSelectedColor = GuiResources::mYellowSelectedButtonColor;
    yellowButtonStyle.mPressedScale = 1.05f;
    yellowButtonStyle.mHasShadow = true;

    mYesButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -1.375f, UiLayer::textRectangle},
                                              buttonInputSize,
                                              yellowButtonStyle);
    mYesButton->CreateIcon("restart.png",
                           {-0.9f, 0.06f, UiLayer::buttonText},
                           {0.82f, 0.82f},
                           {0.3f, 0.3f, 0.3f, 1.0f},
                           Pht::Optional<Pht::Vec4> {},
                           Pht::Optional<Pht::Vec3> {});
    mYesButton->CreateText({-0.25f, -0.23f, UiLayer::buttonText},
                           "Yes",
                           guiResources.GetBlackButtonTextProperties(zoom));
    
    MenuButton::Style blueButtonStyle;
    blueButtonStyle.mMeshFilename = GuiResources::mMediumButtonSkewedMeshFilename;
    blueButtonStyle.mColor = GuiResources::mBlueButtonColor;
    blueButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    blueButtonStyle.mPressedScale = 1.05f;
    blueButtonStyle.mHasShadow = true;

    mNoButton = std::make_unique<MenuButton>(engine,
                                             *this,
                                             Pht::Vec3 {0.0f, -3.975f, UiLayer::textRectangle},
                                             buttonInputSize,
                                             blueButtonStyle);
    mNoButton->CreateIcon("play.png",
                          {-0.75f, 0.07f, UiLayer::buttonText},
                          {0.65f, 0.65f},
                          {1.0f, 1.0f, 1.0f, 1.0f},
                          Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                          Pht::Vec3 {-0.05f, -0.05f, UiLayer::textShadow});
    mNoButton->CreateText({-0.2f, -0.23f, UiLayer::buttonText},
                          "No",
                          guiResources.GetWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen());

}
