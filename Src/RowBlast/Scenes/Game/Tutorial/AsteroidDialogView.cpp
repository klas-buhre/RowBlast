#include "AsteroidDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

namespace {
    std::vector<std::string> ToFrameFilenames(ControlType controlType) {
        switch (controlType) {
            case ControlType::Click:
                return {
                    "asteroid_frame1.jpg",
                    "asteroid_frame1.jpg",
                    "asteroid_frame2.jpg",
                    "asteroid_frame3.jpg",
                    "asteroid_frame4.jpg",
                    "asteroid_frame5.jpg",
                    "asteroid_frame6.jpg",
                    "asteroid_frame7.jpg",
                    "asteroid_frame7.jpg"
                };
            case ControlType::Gesture:
                return {
                    "asteroid_frame3.jpg",
                    "asteroid_frame4.jpg",
                    "asteroid_frame5.jpg",
                    "asteroid_frame6.jpg",
                    "asteroid_frame7.jpg",
                    "asteroid_frame7.jpg"
                };
        }
    }
}

AsteroidDialogView::AsteroidDialogView(Pht::IEngine& engine,
                                       const CommonResources& commonResources,
                                       const UserServices& userServices) :
    mUserServices {userServices} {

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-2.2f, 8.25f, UiLayer::text}, "ASTEROID", largeTextProperties);

    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    mSlideAnimation = std::make_unique<SlideAnimation>(engine,
                                                       *this,
                                                       Pht::Vec3 {0.0f, 1.7f, UiLayer::textRectangle},
                                                       8.5f,
                                                       0.5f);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.4f, -4.4f, UiLayer::text}, "Bring the asteroid down to", textProperties);
    CreateText({-3.8f, -5.475f, UiLayer::text}, "the bottom of the field", textProperties);
    
    Pht::Vec2 playButtonInputSize {194.0f, 43.0f};

    MenuButton::Style playButtonStyle;
    playButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    playButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    playButtonStyle.mPressedScale = 1.05f;

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -8.0f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateText({-0.9f, -0.23f, UiLayer::buttonText},
                            "PLAY",
                            guiResources.GetWhiteButtonTextProperties(zoom));
}

void AsteroidDialogView::SetUp(Pht::Scene& scene) {
    auto frameFilenames {ToFrameFilenames(mUserServices.GetSettingsService().GetControlType())};
    mSlideAnimation->Init(frameFilenames, scene);
}

void AsteroidDialogView::Update() {
    mSlideAnimation->Update();
}
