#include "SwitchPieceWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

SwitchPieceWindowView::SwitchPieceWindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetSmallestDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 4.3f, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-5.4f, 0.9075f, UiLayer::text}, "You always have three pieces to", textProperties);
    CreateText({-5.15f, -0.1675f, UiLayer::text}, "choose from at the bottom. Try", textProperties);
    CreateText({-5.55f, -1.2425f, UiLayer::text}, "switching to a better fitting piece", textProperties);
}
