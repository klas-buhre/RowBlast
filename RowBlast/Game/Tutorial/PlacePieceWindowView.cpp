#include "PlacePieceWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

PlacePieceWindowView::PlacePieceWindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallestDarkMenuWindowPotentiallyZoomedScreen()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 3.0f, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.4f, 0.30f, UiLayer::text}, "Tap the suggested move to place", textProperties);
    CreateText({-1.5f, -0.775f, UiLayer::text}, "the piece", textProperties);
}
