#include "OtherMovesWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

OtherMovesWindowView::OtherMovesWindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallestDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 4.3f, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.75f, 0.9075f, UiLayer::text}, "Tap anywhere to find more moves", textProperties);
    CreateText({-4.8f, -0.1675f, UiLayer::text}, "without switching piece. Tap", textProperties);
    CreateText({-6.1f, -1.2425f, UiLayer::text}, "anywhere except buttons and moves", textProperties);
}
