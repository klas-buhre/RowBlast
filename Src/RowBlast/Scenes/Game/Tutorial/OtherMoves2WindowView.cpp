#include "OtherMoves2WindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

OtherMoves2WindowView::OtherMoves2WindowView(const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetSmallestDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 4.3f, UiLayer::tutorialWindow});
    SetSize(menuWindow.GetSize());

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-5.05f, 0.9075f, UiLayer::text}, "Tap again to find more moves", textProperties);
    CreateText({-5.35f, -0.1675f, UiLayer::text}, "using the same piece. Remember", textProperties);
    CreateText({-5.6f, -1.2425f, UiLayer::text}, "not to press any moves or buttons", textProperties);
}
