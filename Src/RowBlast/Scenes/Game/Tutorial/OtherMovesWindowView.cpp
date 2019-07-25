#include "OtherMovesWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

OtherMovesWindowView::OtherMovesWindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetSmallestDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 5.4f, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());

    auto textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;
    CreateText({0.0f, 0.9075f, UiLayer::text}, "You can alway tap the screen to", textProperties);
    CreateText({0.0f, -0.1675f, UiLayer::text}, "find more moves. Tap anywhere", textProperties);
    CreateText({0.0f, -1.2425f, UiLayer::text}, "except buttons and moves", textProperties);
}
