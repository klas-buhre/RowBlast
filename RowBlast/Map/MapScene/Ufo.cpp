#include "Ufo.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "MapScene.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 ufoOffset {0.0f, -0.5f, 2.0f};
}

Ufo::Ufo(Pht::IEngine& engine, MapScene& mapScene) :
    mMapScene {mapScene},
    mSceneObject {std::make_unique<Pht::SceneObject>()} {

    auto& sceneManager {engine.GetSceneManager()};
    
    Pht::EnvMapTextureFilenames envMapTextures {
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg"
    };

    Pht::Color ambient {0.78f, 0.78f, 0.78f};
    Pht::Color diffuse {0.78f, 0.78f, 0.78f};

    Pht::Color specular {1.0f, 1.0f, 1.0f};
    const auto shininess {15.0f};
    const auto reflectivity {0.84f};
    
    Pht::Material ufoMaterial {
        "ufo.jpg",
        envMapTextures,
        ambient,
        diffuse,
        specular,
        shininess,
        reflectivity
    };
    
    mUfoSceneObject = sceneManager.CreateSceneObject(Pht::ObjMesh {"ufo_3620.obj", 0.76f},
                                                     ufoMaterial,
                                                     mSceneResources);

    mUfoSceneObject->GetTransform().SetPosition(ufoOffset);
    mSceneObject->AddChild(*mUfoSceneObject);
}

void Ufo::Init() {
    mMapScene.GetUfoContainer().AddChild(*mSceneObject);
    mSceneObject->SetIsVisible(true);
}

void Ufo::SetPosition(const Pht::Vec3& position) {
    mSceneObject->GetTransform().SetPosition(position);
}

const Pht::Vec3& Ufo::GetPosition() const {
    return mSceneObject->GetTransform().GetPosition();
}

void Ufo::SetRotation(const Pht::Vec3& rotation) {
    mUfoSceneObject->GetTransform().SetRotation(rotation);
}

void Ufo::Hide() {
    mSceneObject->SetIsVisible(false);
}

void Ufo::Show() {
    mSceneObject->SetIsVisible(true);
}
