#include "CommonResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IAudio.hpp"
#include "ISceneManager.hpp"

using namespace RowBlast;

const std::string CommonResources::mBlipSound {"Blip.wav"};
const std::string CommonResources::mBombSound {"Bomb.wav"};

CommonResources::CommonResources(Pht::IEngine& engine) :
    mMaterials {engine} {

    auto& renderer {engine.GetRenderer()};
    auto& sceneManager {engine.GetSceneManager()};
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    mHussarFontSize22 = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                    renderer.GetAdjustedNumPixels(22));
    mHussarFontSize27 = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                    renderer.GetAdjustedNumPixels(27));
    mHussarFontSize35 = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                    renderer.GetAdjustedNumPixels(35));
    
    sceneManager.InitSceneSystems(CommonResources::narrowFrustumHeightFactor);
    mHussarFontSize22PotentiallyZoomedScreen = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                                           renderer.GetAdjustedNumPixels(22));
    mHussarFontSize27PotentiallyZoomedScreen = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                                           renderer.GetAdjustedNumPixels(27));
    mHussarFontSize35PotentiallyZoomedScreen = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                                           renderer.GetAdjustedNumPixels(35));
    mHussarFontSize52PotentiallyZoomedScreen = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                                           renderer.GetAdjustedNumPixels(52));

    mOrthographicFrustumSizePotentiallyZoomedScreen = renderer.GetOrthographicFrustumSize();
    mHudFrustumSizePotentiallyZoomedScreen = renderer.GetHudFrustumSize();
    mTopPaddingPotentiallyZoomedScreen = renderer.GetTopPaddingHeight();
    mBottomPaddingPotentiallyZoomedScreen = renderer.GetBottomPaddingHeight();

    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    AddSounds(engine.GetAudio());
    
    mGuiResources = std::make_unique<GuiResources>(engine, *this);
}

void CommonResources::AddSounds(Pht::IAudio& audio) {
    audio.AddSound(mBlipSound);
    audio.AddSound(mBombSound);
}

const Pht::Font&
CommonResources::GetHussarFontSize22(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mHussarFontSize22PotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mHussarFontSize22;
    }
}

const Pht::Font&
CommonResources::GetHussarFontSize27(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mHussarFontSize27PotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mHussarFontSize27;
    }
}

const Pht::Font&
CommonResources::GetHussarFontSize35(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mHussarFontSize35PotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mHussarFontSize35;
    }
}
