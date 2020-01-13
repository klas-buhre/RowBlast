#include "SwipeControlsHintDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "GuiUtils.hpp"
#include "CommonResources.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

SwipeControlsHintDialogView::SwipeControlsHintDialogView(Pht::IEngine& engine,
                                                         const CommonResources& commonResources) {
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetLargeDarkMenuWindow();
    auto zoom = PotentiallyZoomedScreen::No;
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));
    
    SetSize(menuWindow.GetSize());
    
    auto largeTextProperties = guiResources.GetLargeWhiteTextProperties(zoom);
    largeTextProperties.mAlignment = Pht::TextAlignment::CenterX;
    CreateText({0.0f, 8.25f, UiLayer::text}, "HINT", largeTextProperties);
    
    mCloseButton = GuiUtils::CreateCloseButton(engine, *this, guiResources, zoom);
    GuiUtils::CreateTitleBarLine(engine, *this);
    
    auto textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    textProperties.mAlignment = Pht::TextAlignment::CenterX;

    CreateText({0.0f, 6.1f, UiLayer::text}, "So far you have been playing", textProperties);
    CreateText({0.0f, 5.025f, UiLayer::text}, "using DragNDrop controls.", textProperties);
    
    CreateText({0.0f, 3.6f, UiLayer::text}, "You can also use SingelTap", textProperties);
    CreateText({0.0f, 2.525f, UiLayer::text}, "to place pieces by clicking:", textProperties);

    CreateSingleTapIcon({0.0f, 0.6f, UiLayer::text}, GetRoot(), engine);
    
    CreateText({0.0f, -1.5f, UiLayer::text}, "You can select DragNDrop or", textProperties);
    CreateText({0.0f, -2.575f, UiLayer::text}, "SingleTap in the settings menu:", textProperties);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "settings.png",
                         {5.7f, -2.4f, UiLayer::text},
                         {0.9f, 0.9f},
                         GetRoot());
    
    Pht::Vec4 iconColor {1.0f, 1.0f, 1.0f, 1.0f};
    Pht::Vec4 iconShadowColor {0.2f, 0.2f, 0.2f, 0.5f};
    Pht::Vec3 iconShadowOffset {-0.05f, -0.05f, UiLayer::textShadow};

    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mRenderableObject = &guiResources.GetMediumThinBlueGlossyButton();
    buttonStyle.mSelectedRenderableObject = &guiResources.GetMediumThinDarkBlueGlossyButton();

    Pht::Vec2 buttonInputSize {194.0f, 43.0f};
    
    mHowToPlayButton = std::make_unique<MenuButton>(engine,
                                                    *this,
                                                    Pht::Vec3 {0.0f, -4.7f, UiLayer::textRectangle},
                                                    buttonInputSize,
                                                    buttonStyle);
    mHowToPlayButton->CreateIcon("help.png",
                                 {-2.55f, 0.05f, UiLayer::buttonText},
                                 {0.82f, 0.82f},
                                 iconColor,
                                 iconShadowColor,
                                 iconShadowOffset);
    auto& buttonTextProperties = guiResources.GetWhiteButtonTextProperties(zoom);
    mHowToPlayButton->CreateText({-1.85f, -0.23f, UiLayer::buttonText},
                                 "How To Play",
                                 buttonTextProperties);
                                 
    Pht::Vec2 playButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style playButtonStyle;
    playButtonStyle.mPressedScale = 1.05f;
    playButtonStyle.mRenderableObject = &guiResources.GetLargerBlueGlossyButton(zoom);
    playButtonStyle.mSelectedRenderableObject = &guiResources.GetLargerDarkBlueGlossyButton(zoom);

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -8.0f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateIcon("play.png",
                            {-1.42f, 0.0f, UiLayer::buttonText},
                            {0.8f, 0.8f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, UiLayer::textShadow});
    mPlayButton->CreateText({-0.8f, -0.31f, UiLayer::buttonText},
                            "PLAY",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

void SwipeControlsHintDialogView::CreateSingleTapIcon(const Pht::Vec3& position,
                                                      Pht::SceneObject& parent,
                                                      Pht::IEngine& engine) {
    auto& icon = CreateSceneObject();
    icon.GetTransform().SetPosition(position);
    icon.GetTransform().SetScale(1.8f);
    parent.AddChild(icon);
    
    GuiUtils::CreateIcon(engine, *this, "hand.png", {0.0f, 0.03f, 0.0f}, {0.9f, 0.9f}, icon);
    GuiUtils::CreateIcon(engine, *this, "circle.png", {-0.03f, 0.42f, 0.0f}, {0.36f, 0.36f}, icon);
    GuiUtils::CreateIcon(engine, *this, "circle.png", {-0.03f, 0.42f, 0.0f}, {0.42f, 0.42f}, icon);
}
