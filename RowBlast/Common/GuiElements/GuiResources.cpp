#include "GuiResources.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

const Pht::Color GuiResources::mBlueButtonColor {0.0f, 0.385f, 1.0f};
const Pht::Color GuiResources::mBlueSelectedButtonColor {0.0f, 0.32f, 0.85f};
const Pht::Color GuiResources::mYellowButtonColor {0.68f, 0.535f, 0.0f};
const Pht::Color GuiResources::mYellowSelectedButtonColor {0.55f, 0.45f, 0.0f};

const std::string GuiResources::mMediumButtonMeshFilename {"medium_button_0385.obj"};

GuiResources::GuiResources(Pht::IEngine& engine, const CommonResources& commonResources) :
    mLargeMenuWindow {engine, commonResources, MenuWindow::Size::Large},
    mMediumMenuWindow {engine, commonResources, MenuWindow::Size::Medium},
    mBlackButtonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.3f, 0.3f, 0.3f, 1.0f}
    },
    mWhiteButtonTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mCaptionTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.2f, 0.2f, 0.2f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    },
    mSmallTextProperties {
        commonResources.GetHussarFontSize27(PotentiallyZoomedScreen::Yes),
        1.0f,
        {0.26f, 0.26f, 0.26f, 1.0f}
    } {}
