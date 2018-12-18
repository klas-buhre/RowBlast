#include "CommonResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "AudioResources.hpp"

using namespace RowBlast;

CommonResources::CommonResources(Pht::IEngine& engine) :
    mMaterials {engine} {

    auto& renderer {engine.GetRenderer()};
    auto& sceneManager {engine.GetSceneManager()};
    
    sceneManager.InitSceneSystems(Pht::ISceneManager::defaultNarrowFrustumHeightFactor);
    mHussarFontSize20 = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                    renderer.GetAdjustedNumPixels(20));
    mHussarFontSize27 = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                    renderer.GetAdjustedNumPixels(27));
    mHussarFontSize35 = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                    renderer.GetAdjustedNumPixels(35));
    
    sceneManager.InitSceneSystems(CommonResources::narrowFrustumHeightFactor);
    mHussarFontSize20PotentiallyZoomedScreen = std::make_unique<Pht::Font>("HussarBoldWeb.otf",
                                                                           renderer.GetAdjustedNumPixels(20));
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
    
    mGuiResources = std::make_unique<GuiResources>(engine, *this);
    
    LoadAudioResouces(engine);
}

const Pht::Font&
CommonResources::GetHussarFontSize20(PotentiallyZoomedScreen potentiallyZoomed) const {
    switch (potentiallyZoomed) {
        case PotentiallyZoomedScreen::Yes:
            return *mHussarFontSize20PotentiallyZoomedScreen;
        case PotentiallyZoomedScreen::No:
            return *mHussarFontSize20;
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
