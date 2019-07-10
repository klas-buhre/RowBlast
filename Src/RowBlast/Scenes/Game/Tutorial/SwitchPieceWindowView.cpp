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

    auto textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    CreateText({0.0f, 0.9075f, UiLayer::text}, "Try switching to a better fitting", textProperties);
    CreateText({0.0f, -0.1675f, UiLayer::text}, "piece. The current piece is the", textProperties);
    CreateText({0.0f, -1.2425f, UiLayer::text}, "left one of the three at the bottom", textProperties);
}
