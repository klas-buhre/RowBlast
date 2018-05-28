#include "LevelStartDialogView.hpp"

// Engine includes.
#include "TextComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "Level.hpp"

using namespace RowBlast;

LevelStartDialogView::LevelStartDialogView(Pht::IEngine& engine,
                                           const CommonResources& commonResources) {
    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    mCaption = &CreateText({-2.1f, 5.1f, UiLayer::text},
                           "LEVEL 1",
                           guiResources.GetCaptionTextProperties(zoom));

    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.5f,
        GetSize().y / 2.0f - 1.5f,
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

    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    mClearObjective = &CreateText({-3.6f, 2.7f, UiLayer::text},
                                  "Clear all gray blocks",
                                  textProperties);
    mClearObjective->GetSceneObject().SetIsVisible(false);
    mBuildObjective = &CreateText({-2.95f, 2.7f, UiLayer::text},
                                  "Fill all gray slots",
                                  textProperties);
    mBuildObjective->GetSceneObject().SetIsVisible(false);

    Pht::Vec2 playButtonInputSize {205.0f, 59.0f};
    
    MenuButton::Style playButtonStyle;
    playButtonStyle.mMeshFilename = GuiResources::mBigButtonMeshFilename;
    playButtonStyle.mColor = GuiResources::mBlueButtonColor;
    playButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    playButtonStyle.mPressedScale = 1.05f;
    playButtonStyle.mHasShadow = true;

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -3.1f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateText({-1.1f, -0.31f, UiLayer::buttonText},
                            "Play",
                            guiResources.GetLargeWhiteButtonTextProperties(zoom));
}

void LevelStartDialogView::Init(const LevelInfo& levelInfo) {
    mCaption->GetText() = "LEVEL " + std::to_string(levelInfo.mIndex);
    
    mClearObjective->GetSceneObject().SetIsVisible(false);
    mBuildObjective->GetSceneObject().SetIsVisible(false);
    
    switch (levelInfo.mObjective) {
        case Level::Objective::Clear:
            mClearObjective->GetSceneObject().SetIsVisible(true);
            break;
        case Level::Objective::Build:
            mBuildObjective->GetSceneObject().SetIsVisible(true);
            break;
    }
}
