#include "MapSceneRenderer.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "MapViewControllers.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"

using namespace BlocksGame;

MapSceneRenderer::MapSceneRenderer(Pht::IEngine& engine,
                                   const MapViewControllers& mapViewControllers) :
    mEngine {engine},
    mMapViewControllers {mapViewControllers} {}

void MapSceneRenderer::RenderFrame() {
    auto* scene {mEngine.GetSceneManager().GetActiveScene()};
    mEngine.GetRenderer().RenderScene(*scene);
    
    RenderViews();
}

void MapSceneRenderer::RenderViews() {
    auto& engineRenderer {mEngine.GetRenderer()};
    
    switch (mMapViewControllers.GetActiveController()) {
        case MapViewControllers::SettingsButton:
            engineRenderer.RenderGuiView(mMapViewControllers.GetSettingsButtonController().GetView());
            break;
        case MapViewControllers::NoLivesDialog: {
            auto& noLivesDialogController {mMapViewControllers.GetNoLivesDialogController()};
            // noLivesDialogController.GetFadeEffect().Render();
            engineRenderer.RenderGuiView(noLivesDialogController.GetView());
            break;
        }
        case MapViewControllers::SettingsMenu: {
            auto& settingsMenuController {mMapViewControllers.GetSettingsMenuController()};
            // settingsMenuController.GetFadeEffect().Render();
            engineRenderer.RenderGuiView(settingsMenuController.GetView());
            break;
        }
    }
}
