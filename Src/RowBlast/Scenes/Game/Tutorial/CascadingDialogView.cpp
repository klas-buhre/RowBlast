#include "CascadingDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

CascadingDialogView::CascadingDialogView(Pht::IEngine& engine,
                                         const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetSmallDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetPosition({0.0f, 0.0f});
    SetSize(menuWindow.GetSize());

    auto& largeTextProperties = guiResources.GetLargeWhiteTextProperties(zoom);
    CreateText({-2.0f, 3.85f, UiLayer::text}, "GRAVITY", largeTextProperties);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager = engine.GetSceneManager();
    auto& lineSceneObject =
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager);
    lineSceneObject.GetTransform().SetPosition({0.0f, 3.0f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-5.4f, 1.3575f, UiLayer::text}, "Gray blocks can float in mid-air", textProperties);
    CreateText({-5.2f, 0.2825f, UiLayer::text}, "but colored blocks are always", textProperties);
    CreateText({-3.95f, -0.7925f, UiLayer::text}, "pulled down by gravity", textProperties);

    mPlayButton = GuiUtils::CreateMediumPlayButton(engine,
                                                   *this,
                                                   {0.0f, -3.5f, UiLayer::textRectangle},
                                                   guiResources,
                                                   zoom);
}
