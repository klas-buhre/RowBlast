#include "LevelBombDialogView.hpp"

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
                    "level_bomb_frame1.jpg",
                    "level_bomb_frame1.jpg",
                    "level_bomb_frame2.jpg",
                    "level_bomb_frame3.jpg",
                    "level_bomb_frame4.jpg",
                    "level_bomb_frame5.jpg"
                };
            case ControlType::Gesture:
                return {
                    "level_bomb_frame3.jpg",
                    "level_bomb_frame4.jpg",
                    "level_bomb_frame5.jpg"
                };
        }
    }
}

LevelBombDialogView::LevelBombDialogView(Pht::IEngine& engine,
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
    CreateText({-1.6f, 8.25f, UiLayer::text}, "BOMBS", largeTextProperties);

    GuiUtils::CreateTitleBarLine(engine, *this);
        
    mSlideAnimation = std::make_unique<SlideAnimation>(engine,
                                                       *this,
                                                       Pht::Vec3 {0.0f, 1.7f, UiLayer::textRectangle},
                                                       8.5f,
                                                       0.5f);

    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    CreateText({-4.85f, -4.4f, UiLayer::text}, "Detonate bombs by placing a", textProperties);
    CreateText({-2.3f, -5.475f, UiLayer::text}, "piece on them", textProperties);

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

void LevelBombDialogView::SetUp(Pht::Scene& scene) {
    auto frameFilenames {ToFrameFilenames(mUserServices.GetSettingsService().GetControlType())};
    mSlideAnimation->Init(frameFilenames, scene);
}

void LevelBombDialogView::Update() {
    mSlideAnimation->Update();
}
