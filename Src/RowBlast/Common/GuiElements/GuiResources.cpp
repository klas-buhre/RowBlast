#include "GuiResources.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "CloseButton.hpp"

using namespace RowBlast;

const Pht::Color GuiResources::mBlueButtonColor {0.0f, 0.333f, 0.675f};
const Pht::Color GuiResources::mBlueSelectedButtonColor {0.0f, 0.28f, 0.57f};
const Pht::Color GuiResources::mYellowButtonColor {0.61f, 0.48f, 0.0f};
const Pht::Color GuiResources::mYellowSelectedButtonColor {0.52f, 0.41f, 0.0f};

const std::string GuiResources::mMediumButtonSkewedMeshFilename {"medium_button_skewed_0385.obj"};

GuiResources::GuiResources(Pht::IEngine& engine, const CommonResources& commonResources) :
    mBlackButtonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {0.3f, 0.3f, 0.3f, 1.0f}
    },
    mLargeWhiteButtonTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    },
    mWhiteButtonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    },
    mCaptionTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.0f,
        {0.23f, 0.23f, 0.23f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f},
        Pht::SnapToPixel::Yes,
        0.125f
    },
    mSmallTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {0.35f, 0.35f, 0.35f, 1.0f}
    },
    mLargeWhiteTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    },
    mSmallWhiteTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {0.95f, 0.95f, 0.95f, 1.0f}
    },
    mLargeMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Large,
        MenuWindow::Style::Bright
    },
    mMediumMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
        MenuWindow::Style::Bright,
    },
    mLargeDarkMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Large,
        MenuWindow::Style::Dark,
    },
    mMediumDarkMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
        MenuWindow::Style::Dark,
    },
    mSmallDarkMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Small,
        MenuWindow::Style::Dark,
    },
    mSmallestDarkMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Smallest,
        MenuWindow::Style::Dark,
    },
    mBlackButtonTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.3f, 0.3f, 0.3f, 1.0f}
    },
    mLargeWhiteButtonTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    },
    mWhiteButtonTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    },
    mWhiteButtonTextWithShadowPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mCaptionTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.23f, 0.23f, 0.23f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f},
        Pht::SnapToPixel::Yes,
        0.125f
    },
    mSmallTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.3f, 0.3f, 0.3f, 1.0f}
    },
    mSmallWhiteTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.95f, 0.95f, 0.95f, 1.0f}
    },
    mLargeWhiteTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    },
    mCloseButton {CreateCloseButton(engine, commonResources, PotentiallyZoomedScreen::No)},
    mCloseButtonPotentiallyZoomedScreen {
        CreateCloseButton(engine, commonResources, PotentiallyZoomedScreen::Yes)
    },
    mBluePauseButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::PauseButton,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mDarkBluePauseButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::PauseButton,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    },
    mLargerBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Larger,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mLargerBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Larger,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mLargerDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Larger,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mLargerDarkBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Larger,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    },
    mMediumBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mMediumBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mMediumDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mMediumDarkBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    },
    mSmallBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Small,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mSmallBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Small,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mSmallDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Small,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mSmallDarkBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Small,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    },
    mLargeGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::No)
    },
    mLargeDarkGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::No)
    },
    mLargeBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mLargeBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mLargeDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mLargeDarkBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    },
    mMediumGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::No)
    },
    mMediumGreenGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::Yes)
    },
    mMediumDarkGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::No)
    },
    mMediumDarkGreenGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Medium,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::Yes)
    },
    mMediumThinBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::MediumThin,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mMediumThinDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::MediumThin,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mSmallerBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Smaller,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mSmallerDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Smaller,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mEvenSmallerBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::EvenSmaller,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mEvenSmallerBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::EvenSmaller,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mEvenSmallerDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::EvenSmaller,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mEvenSmallerDarkBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::EvenSmaller,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    },
    mSmallestBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Smallest,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mSmallestDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Smallest,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mThinBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::No)
    },
    mThinBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::Blue,
                           PotentiallyZoomedScreen::Yes)
    },
    mThinDarkBlueGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::No)
    },
    mThinDarkBlueGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::DarkBlue,
                           PotentiallyZoomedScreen::Yes)
    } {}

const Pht::TextProperties&
GuiResources::GetBlackButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mBlackButtonTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mBlackButtonTextProperties;
    }
}

const Pht::TextProperties&
GuiResources::GetLargeWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mLargeWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mLargeWhiteButtonTextProperties;
    }
}

const Pht::TextProperties&
GuiResources::GetWhiteButtonTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mWhiteButtonTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mWhiteButtonTextProperties;
    }
}

const Pht::TextProperties&
GuiResources::GetCaptionTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mCaptionTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mCaptionTextProperties;
    }
}

const Pht::TextProperties&
GuiResources::GetSmallTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mSmallTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mSmallTextProperties;
    }
}

const Pht::TextProperties&
GuiResources::GetLargeWhiteTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mLargeWhiteTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mLargeWhiteTextProperties;
    }
}

const Pht::TextProperties&
GuiResources::GetSmallWhiteTextProperties(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mSmallWhiteTextPropertiesPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mSmallWhiteTextProperties;
    }
}

Pht::RenderableObject&
GuiResources::GetCloseButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mCloseButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mCloseButton;
    }
}

Pht::RenderableObject&
GuiResources::GetLargerBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mLargerBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mLargerBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetLargerDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mLargerDarkBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mLargerDarkBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetMediumBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mMediumBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mMediumBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetMediumDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mMediumDarkBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mMediumDarkBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetSmallBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mSmallBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mSmallBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetSmallDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mSmallDarkBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mSmallDarkBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetEvenSmallerBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mEvenSmallerBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mEvenSmallerBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetEvenSmallerDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mEvenSmallerDarkBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mEvenSmallerDarkBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetLargeBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mLargeBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mLargeBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetLargeDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mLargeDarkBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mLargeDarkBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetMediumGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mMediumGreenGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mMediumGreenGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetMediumDarkGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mMediumDarkGreenGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mMediumDarkGreenGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetThinBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mThinBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mThinBlueGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetThinDarkBlueGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mThinDarkBlueGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mThinDarkBlueGlossyButton;
    }
}
