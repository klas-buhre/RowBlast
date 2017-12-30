#include "MapSceneRenderer.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "MapHud.hpp"
#include "MapViewControllers.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"
#include "RenderUtils.hpp"

using namespace BlocksGame;

MapSceneRenderer::MapSceneRenderer(Pht::IEngine& engine,
                                   const MapHud& hud,
                                   const MapViewControllers& mapViewControllers,
                                   const UserData& userData) :
    mEngine {engine},
    mHud {hud},
    mMapViewControllers {mapViewControllers},
    mUserData {userData} {}

void MapSceneRenderer::RenderFrame() {
    auto* scene {mEngine.GetSceneManager().GetActiveScene()};
    mEngine.GetRenderer().RenderScene(*scene);
    
    RenderHud();
    RenderViews();
}

void MapSceneRenderer::RenderHud() {
    auto& engineRenderer {mEngine.GetRenderer()};
    engineRenderer.SetHudMode(true);
    
    RenderUtils::RenderGradientRectangle(engineRenderer, mHud.GetLivesRectangle());
    RenderUtils::RenderText(engineRenderer, mHud.GetLivesText());
    
    if (!mUserData.GetLifeManager().HasFullNumLives()) {
        RenderUtils::RenderGradientRectangle(engineRenderer, mHud.GettNewLifeCountdownRectangle());
        RenderUtils::RenderText(engineRenderer, mHud.GetNewLifeCountdownText());
    }
    
    engineRenderer.SetHudMode(false);
}

void MapSceneRenderer::RenderViews() {
    auto& engineRenderer {mEngine.GetRenderer()};
    
    switch (mMapViewControllers.GetActiveController()) {
        case MapViewControllers::SettingsButton:
            engineRenderer.RenderGuiView(mMapViewControllers.GetSettingsButtonController().GetView());
            break;
        case MapViewControllers::NoLivesDialog: {
            auto& noLivesDialogController {mMapViewControllers.GetNoLivesDialogController()};
            noLivesDialogController.GetFadeEffect().Render();
            engineRenderer.RenderGuiView(noLivesDialogController.GetView());
            break;
        }
        case MapViewControllers::SettingsMenu: {
            auto& settingsMenuController {mMapViewControllers.GetSettingsMenuController()};
            settingsMenuController.GetFadeEffect().Render();
            engineRenderer.RenderGuiView(settingsMenuController.GetView());
            break;
        }
    }
}
