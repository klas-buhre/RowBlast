#include "HowToPlayDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "PieceResources.hpp"
#include "IGuiLightProvider.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 lightDirectionA {0.785f, 1.0f, 0.67f};
    const Pht::Vec3 lightDirectionB {1.0f, 1.0f, 0.74f};
}

HowToPlayDialogView::HowToPlayDialogView(Pht::IEngine& engine,
                                         const CommonResources& commonResources,
                                         const PieceResources& pieceResources,
                                         SceneId sceneId) {
    auto zoom {sceneId == SceneId::Game ? PotentiallyZoomedScreen::Yes : PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-4.5f, 8.25f, UiLayer::text}, "HOW TO PLAY", largeTextProperties);

    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };

    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    

    auto& block {CreateSceneObject()};
    auto& blockRenderable {
        pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                BlockColor::Blue,
                                                BlockBrightness::Normal)
    };
    block.SetRenderable(&blockRenderable);
    block.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::block});
    GetRoot().AddChild(block);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.4f, -4.4f, UiLayer::text}, "Bring the asteroid down to", textProperties);
    CreateText({-3.8f, -5.475f, UiLayer::text}, "the bottom of the field", textProperties);
}

void HowToPlayDialogView::SetUp() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetGuiLightDirections(lightDirectionA, lightDirectionB);
    }
}

void HowToPlayDialogView::Update() {

}

void HowToPlayDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}
