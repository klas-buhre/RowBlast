#include "BombDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "UserServices.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

namespace {
    std::vector<std::string> ToFrameFilenames(ControlType controlType) {
        switch (controlType) {
            case ControlType::Click:
                return {
                    "bomb_frame1.jpg",
                    "bomb_frame1.jpg",
                    "bomb_frame2.jpg",
                    "bomb_frame3.jpg",
                    "bomb_frame4.jpg",
                    "bomb_frame5.jpg"
                };
            case ControlType::Gesture:
                return {
                    "bomb_frame3.jpg",
                    "bomb_frame4.jpg",
                    "bomb_frame5.jpg"
                };
        }
    }
}

BombDialogView::BombDialogView(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               const UserServices& userServices) :
    mUserServices {userServices} {

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetLargeDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties = guiResources.GetLargeWhiteTextProperties(zoom);
    CreateText({-1.4f, 8.25f, UiLayer::text}, "BOMB", largeTextProperties);

    GuiUtils::CreateTitleBarLine(engine, *this);
    
    mSlideAnimation = std::make_unique<SlideAnimation>(engine,
                                                       *this,
                                                       Pht::Vec3 {0.0f, 1.3f, UiLayer::textRectangle},
                                                       9.0f,
                                                       0.5f);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-4.15f, -5.1f, UiLayer::text}, "The bomb clears an area", textProperties);
    
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

void BombDialogView::SetUp(Pht::Scene& scene) {
    auto frameFilenames = ToFrameFilenames(mUserServices.GetSettingsService().GetControlType());
    mSlideAnimation->Init(frameFilenames, scene);
}

void BombDialogView::Update() {
    mSlideAnimation->Update();
}
