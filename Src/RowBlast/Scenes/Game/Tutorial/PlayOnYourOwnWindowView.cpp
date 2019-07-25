#include "PlayOnYourOwnWindowView.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

PlayOnYourOwnWindowView::PlayOnYourOwnWindowView(const CommonResources& commonResources) {
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
    CreateText({0.0f, 0.30f, UiLayer::text}, "Now you can play on your own.", textProperties);
    CreateText({0.0f, -0.775f, UiLayer::text}, "Let's clear some blocks!", textProperties);
}
