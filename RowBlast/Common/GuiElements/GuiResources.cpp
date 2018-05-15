#include "GuiResources.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "CloseButton.hpp"

using namespace RowBlast;

const Pht::Color GuiResources::mBlueButtonColor {0.0f, 0.385f, 1.0f};
const Pht::Color GuiResources::mBlueSelectedButtonColor {0.0f, 0.32f, 0.85f};
const Pht::Color GuiResources::mYellowButtonColor {0.68f, 0.535f, 0.0f};
const Pht::Color GuiResources::mYellowSelectedButtonColor {0.55f, 0.45f, 0.0f};
const Pht::Color GuiResources::mGreenButtonColor {0.1f, 0.51f, 0.1f};
const Pht::Color GuiResources::mGreenSelectedButtonColor {0.085f, 0.43f, 0.085f};

const std::string GuiResources::mBigButtonMeshFilename {"big_button_048.obj"};
const std::string GuiResources::mMediumButtonMeshFilename {"medium_button_0385.obj"};
const std::string GuiResources::mSmallButtonMeshFilename {"small_button_0385.obj"};

GuiResources::GuiResources(Pht::IEngine& engine, const CommonResources& commonResources) :
    mMediumMenuWindow {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
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
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.06f, 0.06f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mWhiteButtonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mCaptionTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.0f,
        {0.2f, 0.2f, 0.2f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mSmallTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::No),
        1.0f,
        {0.35f, 0.35f, 0.35f, 1.0f}
    },
    mLargeMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Large,
        PotentiallyZoomedScreen::Yes
    },
    mMediumMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Medium,
        PotentiallyZoomedScreen::Yes
    },
    mSmallMenuWindowPotentiallyZoomedScreen {
        engine,
        commonResources,
        MenuWindow::Size::Small,
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
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.06f, 0.06f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mWhiteButtonTextPropertiesPotentiallyZoomedScreen {
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
        {0.2f, 0.2f, 0.2f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mSmallTextPropertiesPotentiallyZoomedScreen {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.26f, 0.26f, 0.26f, 1.0f}
    },
    mCloseButton {CreateCloseButton(engine, commonResources, PotentiallyZoomedScreen::No)},
    mCloseButtonPotentiallyZoomedScreen {
        CreateCloseButton(engine, commonResources, PotentiallyZoomedScreen::Yes)
    } {}

const MenuWindow&
GuiResources::GetMediumMenuWindow(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return mMediumMenuWindowPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return mMediumMenuWindow;
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

Pht::RenderableObject&
GuiResources::GetCloseButton(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mCloseButtonPotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mCloseButton;
    }
}
