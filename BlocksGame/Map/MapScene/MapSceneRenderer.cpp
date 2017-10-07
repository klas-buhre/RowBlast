#include "MapSceneRenderer.hpp"

// Engine includes.
#include "IRenderer.hpp"

// Game includes.
#include "MapScene.hpp"
#include "MapHud.hpp"
#include "MapViewControllers.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"
#include "RenderUtils.hpp"

using namespace BlocksGame;

MapSceneRenderer::MapSceneRenderer(Pht::IRenderer& engineRenderer,
                                   const MapScene& scene,
                                   const MapHud& hud,
                                   const MapViewControllers& mapViewControllers,
                                   const UserData& userData) :
    mEngineRenderer {engineRenderer},
    mScene {scene},
    mHud {hud},
    mMapViewControllers {mapViewControllers},
    mUserData {userData} {}

void MapSceneRenderer::RenderFrame() {
    mEngineRenderer.SetProjectionMode(Pht::ProjectionMode::Perspective);
    RenderUtils::RenderSceneObject(mEngineRenderer, mScene.GetBackground());
    RenderUtils::RenderFloatingCubes(mEngineRenderer, mScene.GetFloatingCubes());
    
    mEngineRenderer.SetProjectionMode(Pht::ProjectionMode::Orthographic);
    RenderPins();
    RenderUtils::RenderParticleEffect(mEngineRenderer, mScene.GetNextLevelParticleEffect());
    
    RenderHud();
    RenderViews();
}

void MapSceneRenderer::RenderPins() {
    for (auto& connection: mScene.GetConnections()) {
        RenderUtils::RenderSceneObject(mEngineRenderer, *connection);
    }
    
    for (auto& pin: mScene.GetPins()) {
        RenderPin(*pin);
    }    
}

void MapSceneRenderer::RenderPin(MapPin& pin) {
    auto& sceneObject {pin.GetSceneObject()};
    
    if (pin.IsSelected()) {
        auto& renderable {sceneObject.GetRenderable()};
        auto& material {renderable.GetMaterial()};
        
        auto ambient {material.GetAmbient()};
        auto diffuse {material.GetDiffuse()};
        auto specular {material.GetSpecular()};

        material.SetAmbient(ambient + MapPin::selectedColorAdd);
        material.SetDiffuse(diffuse + MapPin::selectedColorAdd);
        material.SetSpecular(specular + MapPin::selectedColorAdd);

        RenderUtils::RenderSceneObject(mEngineRenderer, sceneObject);
        
        material.SetAmbient(ambient);
        material.SetDiffuse(diffuse);
        material.SetSpecular(specular);
    } else {
        RenderUtils::RenderSceneObject(mEngineRenderer, sceneObject);
    }
    
    RenderUtils::RenderText(mEngineRenderer, pin.GetText());

    for (auto& star: pin.GetStars()) {
        RenderUtils::RenderSceneObject(mEngineRenderer, *star);
    }
}

void MapSceneRenderer::RenderHud() {
    mEngineRenderer.SetHudMode(true);
    
    RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GetLivesRectangle());
    RenderUtils::RenderText(mEngineRenderer, mHud.GetLivesText());
    
    if (!mUserData.GetLifeManager().HasFullNumLives()) {
        RenderUtils::RenderGradientRectangle(mEngineRenderer, mHud.GettNewLifeCountdownRectangle());
        RenderUtils::RenderText(mEngineRenderer, mHud.GetNewLifeCountdownText());
    }
    
    mEngineRenderer.SetHudMode(false);
}

void MapSceneRenderer::RenderViews() {
    switch (mMapViewControllers.GetActiveController()) {
        case MapViewControllers::SettingsButton:
            mEngineRenderer.RenderGuiView(mMapViewControllers.GetSettingsButtonController().GetView());
            break;
        case MapViewControllers::NoLivesDialog: {
            auto& noLivesDialogController {mMapViewControllers.GetNoLivesDialogController()};
            noLivesDialogController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(noLivesDialogController.GetView());
            break;
        }
        case MapViewControllers::SettingsMenu: {
            auto& settingsMenuController {mMapViewControllers.GetSettingsMenuController()};
            settingsMenuController.GetFadeEffect().Render();
            mEngineRenderer.RenderGuiView(settingsMenuController.GetView());
            break;
        }
    }
}
