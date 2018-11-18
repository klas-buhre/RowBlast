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
    mMediumDarkMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
        MenuWindow::Style::Dark,
        PotentiallyZoomedScreen::No
    },
    mLargeDarkMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Large,
        MenuWindow::Style::Dark,
        PotentiallyZoomedScreen::No
    },
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
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mSmallTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {0.35f, 0.35f, 0.35f, 1.0f}
    },
    mLargeWhiteTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.0f,
        {0.95f, 0.95f, 0.95f, 1.0f}
    },
    mSmallWhiteTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {0.95f, 0.95f, 0.95f, 1.0f}
    },
    mLargeMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Large,
        MenuWindow::Style::Bright,
        PotentiallyZoomedScreen::Yes
    },
    mMediumMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
        MenuWindow::Style::Bright,
        PotentiallyZoomedScreen::Yes
    },
    mLargeDarkMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Large,
        MenuWindow::Style::Dark,
        PotentiallyZoomedScreen::Yes
    },
    mMediumDarkMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
        MenuWindow::Style::Dark,
        PotentiallyZoomedScreen::Yes
    },
    mSmallDarkMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Small,
        MenuWindow::Style::Dark,
        PotentiallyZoomedScreen::Yes
    },
    mSmallestDarkMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Smallest,
        MenuWindow::Style::Dark,
        PotentiallyZoomedScreen::Yes
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
        {0.4f, 0.4f, 0.4f, 0.5f}
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
        {0.95f, 0.95f, 0.95f, 1.0f}
    },
    mCloseButton {CreateCloseButton(engine, commonResources, PotentiallyZoomedScreen::No)},
    mCloseButtonPotentiallyZoomedScreen {
        CreateCloseButton(engine, commonResources, PotentiallyZoomedScreen::Yes)
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
    mSmallGrayGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Small,
                           ButtonColor::Gray,
                           PotentiallyZoomedScreen::Yes)
    },
    mLargeGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::No)
    },
    mLargeGreenGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::Yes)
    },
    mLargeDarkGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::No)
    },
    mLargeDarkGreenGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Large,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::Yes)
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
    mThinGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::No)
    },
    mThinGreenGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::Green,
                           PotentiallyZoomedScreen::Yes)
    },
    mThinDarkGreenGlossyButton {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::No)
    },
    mThinDarkGreenGlossyButtonPotentiallyZoomedScreen {
        CreateGlossyButton(engine,
                           commonResources,
                           ButtonSize::Thin,
                           ButtonColor::DarkGreen,
                           PotentiallyZoomedScreen::Yes)
    } {}

const MenuWindow&
GuiResources::GetMediumDarkMenuWindow(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mMediumDarkMenuWindowPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mMediumDarkMenuWindow;
    }
}

const MenuWindow&
GuiResources::GetLargeDarkMenuWindow(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mLargeDarkMenuWindowPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mLargeDarkMenuWindow;
    }
}

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
GuiResources::GetLargeGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mLargeGreenGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mLargeGreenGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetLargeDarkGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mLargeDarkGreenGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mLargeDarkGreenGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetThinGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mThinGreenGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mThinGreenGlossyButton;
    }
}

Pht::RenderableObject&
GuiResources::GetThinDarkGreenGlossyButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mThinDarkGreenGlossyButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mThinDarkGreenGlossyButton;
    }
}
