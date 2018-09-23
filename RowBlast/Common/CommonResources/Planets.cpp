#include "Planets.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "SceneObject.hpp"
#include "LightComponent.hpp"

using namespace RowBlast;

namespace {
    const std::string planetMeshName {"planet_960.obj"};
    
    Pht::SceneObject& CreateOgmaSceneObject(Pht::Scene& scene,
                                            Pht::SceneObject& containerSceneObject,
                                            const PlanetConfig& planetConfig) {
        Pht::Material ogmaMaterial {"planet_ogma.jpg", 0.02f, 1.0f, 0.00f, 1.0f};
        auto& ogmaPlanet {
            scene.CreateSceneObject(Pht::ObjMesh {planetMeshName, planetConfig.mSize},
                                    ogmaMaterial)
        };
        
        Pht::Material ringMaterial {"ogma_rings.png", 1.0f, 1.0f, 0.00f, 1.0f};
        ringMaterial.SetBlend(Pht::Blend::Yes);
        auto& rings {
            scene.CreateSceneObject(Pht::ObjMesh {"planet_ring.obj", planetConfig.mSize * 3.0f},
                                    ringMaterial)
        };
        
        auto& ogmaPlanetSystem {scene.CreateSceneObject()};
        ogmaPlanetSystem.AddChild(ogmaPlanet);
        ogmaPlanetSystem.AddChild(rings);
        
        auto& transform {ogmaPlanetSystem.GetTransform()};
        transform.SetPosition(planetConfig.mPosition);
        transform.SetRotation(planetConfig.mOrientation);
        
        containerSceneObject.AddChild(ogmaPlanetSystem);
        
        return ogmaPlanet;
    }

    Pht::SceneObject& CreateTitawinSceneObject(Pht::Scene& scene,
                                               Pht::SceneObject& containerSceneObject,
                                               const PlanetConfig& planetConfig) {
        Pht::Material titawinMaterial {"planet_titawin.jpg", 0.02f, 1.0f, 0.00f, 1.0f};
        auto& titawinPlanet {
            scene.CreateSceneObject(Pht::ObjMesh {planetMeshName, planetConfig.mSize},
                                    titawinMaterial)
        };

        Pht::Material titawinCloudMaterial {"titawin_clouds.png", 0.05f, 1.0f, 0.00f, 1.0f};
        titawinCloudMaterial.SetBlend(Pht::Blend::Yes);
        auto& clouds1 {
            scene.CreateSceneObject(Pht::ObjMesh {planetMeshName, planetConfig.mSize * 1.03f},
                                    titawinCloudMaterial)
        };
        clouds1.GetTransform().SetRotation({0.0f, 220.0f, 0.0f});
        auto& clouds2 {
            scene.CreateSceneObject(Pht::ObjMesh {planetMeshName, planetConfig.mSize * 1.03f},
                                    titawinCloudMaterial)
        };
        clouds2.GetTransform().SetRotation({0.0f, 260.0f, 0.0f});

        auto& titawinPlanetSystem {scene.CreateSceneObject()};
        titawinPlanetSystem.AddChild(titawinPlanet);
        titawinPlanetSystem.AddChild(clouds1);
        titawinPlanetSystem.AddChild(clouds2);
        
        auto& transform {titawinPlanetSystem.GetTransform()};
        transform.SetPosition(planetConfig.mPosition);
        transform.SetRotation(planetConfig.mOrientation);
        
        containerSceneObject.AddChild(titawinPlanetSystem);
        
        return titawinPlanet;
    }

    Pht::SceneObject& CreateMoonSceneObject(Pht::Scene& scene,
                                            Pht::SceneObject& containerSceneObject,
                                            const PlanetConfig& planetConfig) {
        Pht::Material moonMaterial {"moon.jpg", 0.05f, 1.0f, 0.00f, 1.0f};
        auto& moon {
            scene.CreateSceneObject(Pht::ObjMesh {planetMeshName, planetConfig.mSize},
                                    moonMaterial)
        };
        
        auto& transform {moon.GetTransform()};
        transform.SetPosition(planetConfig.mPosition);
        transform.SetRotation(planetConfig.mOrientation);
        
        containerSceneObject.AddChild(moon);
        
        return moon;
    }

    Pht::SceneObject& CreatePlanetSceneObject(Pht::Scene& scene,
                                              Pht::SceneObject& containerSceneObject,
                                              const PlanetConfig& planetConfig) {
        switch (planetConfig.mType) {
            case PlanetType::Ogma:
                return CreateOgmaSceneObject(scene, containerSceneObject, planetConfig);
            case PlanetType::Titawin:
                return CreateTitawinSceneObject(scene, containerSceneObject, planetConfig);
            case PlanetType::Moon:
                return CreateMoonSceneObject(scene, containerSceneObject, planetConfig);
        }
    }
}

Planets::Planets(Pht::IEngine& engine,
                 Pht::Scene& scene,
                 int layerIndex,
                 const std::vector<PlanetConfig>& planetConfigs) :
    mEngine {engine} {

    auto& containerSceneObject {scene.CreateSceneObject()};
    containerSceneObject.SetLayer(layerIndex);
    scene.GetRoot().AddChild(containerSceneObject);
    
    auto& lightSceneObject {scene.CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    lightComponent->SetDirectionalIntensity(1.15f);
    auto* planetsRenderPass {scene.GetRenderPass(layerIndex)};
    planetsRenderPass->SetLight(lightComponent.get());
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    containerSceneObject.AddChild(lightSceneObject);

    mPlanets.reserve(planetConfigs.size());
    
    for (auto& planetConfig: planetConfigs) {
        auto& planetSceneObject {
            CreatePlanetSceneObject(scene, containerSceneObject, planetConfig)
        };
        
        Planet planet {planetConfig.mAngularVelocity, planetSceneObject};
        mPlanets.push_back(planet);
    }
}

void Planets::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    for (auto& planet: mPlanets) {
        Pht::Vec3 rotation {0.0f, planet.mAngularVelocity * dt, 0.0f};
        planet.mSceneObject.GetTransform().Rotate(rotation);
    }
}
