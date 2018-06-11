#include "MapConfirmationDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

MapConfirmationDialogView::MapConfirmationDialogView(Pht::IEngine& engine,
                                                     const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-1.0f, 5.1f, UiLayer::text}, "MAP", guiResources.GetCaptionTextProperties(zoom));
    
    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    CreateText({-5.5f, 2.5f, UiLayer::text}, "Are you sure you want to go back", textProperties);
    CreateText({-1.8f, 1.5f, UiLayer::text}, "to the map?", textProperties);
    
    Pht::Vec2 buttonInputSize {194.0f, 45.0f};
    MenuButton::Style yellowButtonStyle;
    yellowButtonStyle.mMeshFilename = GuiResources::mMediumButtonSkewedMeshFilename;
    yellowButtonStyle.mColor = GuiResources::mYellowButtonColor;
    yellowButtonStyle.mSelectedColor = GuiResources::mYellowSelectedButtonColor;
    yellowButtonStyle.mPressedScale = 1.05f;
    yellowButtonStyle.mHasShadow = true;

    mYesButton = std::make_unique<MenuButton>(engine,
                                              *this,
                                              Pht::Vec3 {0.0f, -0.7f, UiLayer::textRectangle},
                                              buttonInputSize,
                                              yellowButtonStyle);
    mYesButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText},
                           "Yes",
                           guiResources.GetBlackButtonTextProperties(zoom));
    
    MenuButton::Style blueButtonStyle;
    blueButtonStyle.mMeshFilename = GuiResources::mMediumButtonSkewedMeshFilename;
    blueButtonStyle.mColor = GuiResources::mDarkBlueButtonColor;
    blueButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    blueButtonStyle.mPressedScale = 1.05f;
    blueButtonStyle.mHasShadow = true;

    mNoButton = std::make_unique<MenuButton>(engine,
                                             *this,
                                             Pht::Vec3 {0.0f, -3.3f, UiLayer::textRectangle},
                                             buttonInputSize,
                                             blueButtonStyle);
    mNoButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
                          "No",
                          guiResources.GetWhiteButtonTextProperties(zoom));
}
