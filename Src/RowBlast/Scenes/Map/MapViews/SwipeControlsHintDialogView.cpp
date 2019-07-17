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

    CreateText({0.0f, 6.3f, UiLayer::text}, "So far you have been playing", textProperties);
    CreateText({0.0f, 5.225f, UiLayer::text}, "using SingleTap controls:", textProperties);
    
    CreateSingleTapIcon({0.0f, 3.5, UiLayer::text}, GetRoot(), engine);

    CreateText({0.0f, 1.0f, UiLayer::text}, "You can also use Swipe controls", textProperties);
    CreateText({0.0f, -0.075f, UiLayer::text}, "to control your piece using gestures:", textProperties);

    CreateSwipeIcon({0.0f, -1.8f, UiLayer::text}, GetRoot(), engine);
    
    CreateText({0.0f, -4.2f, UiLayer::text}, "You can change the controls in", textProperties);
    CreateText({0.0f, -5.275f, UiLayer::text}, "the settings menu:", textProperties);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "settings.png",
                         {3.7f, -5.1f, UiLayer::text},
                         {0.9f, 0.9f},
                         GetRoot());
    
    Pht::Vec4 iconColor {1.0f, 1.0f, 1.0f, 1.0f};
    Pht::Vec4 iconShadowColor {0.2f, 0.2f, 0.2f, 0.5f};
    Pht::Vec3 iconShadowOffset {-0.05f, -0.05f, UiLayer::textShadow};

    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    buttonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    
    Pht::Vec2 buttonInputSize {194.0f, 43.0f};
    
    mHowToPlayButton = std::make_unique<MenuButton>(engine,
                                                    *this,
                                                    Pht::Vec3 {0.0f, -8.1f, UiLayer::textRectangle},
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

void SwipeControlsHintDialogView::CreateSwipeIcon(const Pht::Vec3& position,
                                                  Pht::SceneObject& parent,
                                                  Pht::IEngine& engine) {
    auto& icon = CreateSceneObject();
    icon.GetTransform().SetPosition(position);
    icon.GetTransform().SetScale(1.8);
    parent.AddChild(icon);
    
    GuiUtils::CreateIcon(engine, *this, "hand.png", {0.0, 0.03f, 0.0f}, {0.9f, 0.9f}, icon);
    GuiUtils::CreateIcon(engine, *this, "back.png", {-0.4f, 0.4f, 0.0f}, {0.42f, 0.42f}, icon);
    GuiUtils::CreateIcon(engine, *this, "right_arrow.png", {0.35f, 0.4f, 0.0f}, {0.42f, 0.42f}, icon);
}
