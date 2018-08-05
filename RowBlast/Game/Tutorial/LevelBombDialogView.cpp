#include "LevelBombDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

LevelBombDialogView::LevelBombDialogView(Pht::IEngine& engine,
                                         const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.6f, 7.6f, UiLayer::text}, "BOMBS", largeTextProperties);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, 6.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    mSlideAnimation = std::make_unique<SlideAnimation>(engine,
                                                       *this,
                                                       Pht::Vec3 {0.0f, 1.3f, UiLayer::textRectangle},
                                                       8.5f,
                                                       0.5f);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.15f, -4.6f, UiLayer::text}, "The bomb clears an area", textProperties);
    
    Pht::Vec2 playButtonInputSize {194.0f, 43.0f};

    MenuButton::Style playButtonStyle;
    playButtonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    playButtonStyle.mColor = GuiResources::mBlueButtonColor;
    playButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    playButtonStyle.mPressedScale = 1.05f;

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -7.0f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateText({-0.9f, -0.23f, UiLayer::buttonText}, "PLAY", textProperties);
}

void LevelBombDialogView::Init(Pht::Scene& scene) {
    std::vector<std::string> frameFilenames {
        "bomb_frame1.jpg",
        "bomb_frame2.jpg",
        "bomb_frame3.jpg",
        "bomb_frame4.jpg",
        "bomb_frame5.jpg"
    };

    mSlideAnimation->Init(frameFilenames, scene);
}

void LevelBombDialogView::Update() {
    mSlideAnimation->Update();
}
