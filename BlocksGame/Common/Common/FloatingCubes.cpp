#include "FloatingCubes.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "ObjMesh.hpp"
#include "MathUtils.hpp"
#include "SceneObject.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace BlocksGame;

FloatingCubes::FloatingCubes(Pht::IEngine& engine,
                             Pht::Scene* scene,
                             const std::vector<Volume>& volumes,
                             const CommonResources& commonResources,
                             float scale) :
    mEngine {engine},
    mScene {scene},
    mVolumes {volumes} {
    
    mCubes.resize(mVolumes.size());
    
    Pht::ObjMesh triangleMesh {"triangle_428_085.obj", scale};
    Pht::ObjMesh cubeMesh {"cube_554.obj", scale};
    auto& materials {commonResources.GetMaterials()};
    auto& sceneManager {engine.GetSceneManager()};
    
    mCubeRenderables = {
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGoldMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetBlueMaterial()),
        sceneManager.CreateRenderableObject(triangleMesh, materials.GetRedMaterial()),
        sceneManager.CreateRenderableObject(cubeMesh, materials.GetGreenMaterial())
    };
    
    mSceneObject = std::make_unique<Pht::SceneObject>();
    
    if (mScene) {
        mScene->GetRoot().AddChild(*mSceneObject);
    }
    
    for (auto& cube: mCubes) {
        cube.mSceneObject = std::make_unique<Pht::SceneObject>();
        mSceneObject->AddChild(*cube.mSceneObject);
    }
}

void FloatingCubes::Reset() {
    for (auto i {0}; i < mCubes.size(); ++i) {
        const auto& volume {mVolumes[i]};
    
        Pht::Vec3 position {
            Pht::NormalizedRand() * volume.mSize.x - volume.mSize.x / 2.0f + volume.mPosition.x,
            Pht::NormalizedRand() * volume.mSize.y - volume.mSize.y / 2.0f + volume.mPosition.y,
            Pht::NormalizedRand() * volume.mSize.z - volume.mSize.z / 2.0f + volume.mPosition.z
        };
        
        Pht::Vec3 velocity {
            volume.mSize == Pht::Vec3{0.0f, 0.0f, 0.0f} ? 0.0f : (Pht::NormalizedRand() - 0.5f),
            0.0f,
            0.0f
        };
        
        Pht::Vec3 rotation {
            Pht::NormalizedRand() * 360.0f,
            Pht::NormalizedRand() * 360.0f,
            Pht::NormalizedRand() * 360.0f,
        };
        
        Pht::Vec3 angularVelocity {
            (Pht::NormalizedRand() - 0.5f) * 20.0f,
            (Pht::NormalizedRand() - 0.5f) * 20.0f,
            (Pht::NormalizedRand() - 0.5f) * 20.0f
        };
        
        auto& cube {mCubes[i]};
        cube.mVelocity = velocity;
        cube.mAngularVelocity = angularVelocity;
        
        cube.mSceneObject->SetRenderable(mCubeRenderables[std::rand() % numRenderables]);
        
        auto& transform {cube.mSceneObject->GetTransform()};
        transform.SetPosition(position);
        transform.SetRotation(rotation);
    }
}

void FloatingCubes::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};

    for (auto i {0}; i < mCubes.size(); ++i) {
        auto& cube {mCubes[i]};
        auto& transform {cube.mSceneObject->GetTransform()};
        transform.Translate(cube.mVelocity * dt);
        transform.Rotate(cube.mAngularVelocity * dt);
        
        const auto& volume {mVolumes[i]};
        auto rightLimit {volume.mPosition.x + volume.mSize.x / 2.0f};
        auto leftLimit {volume.mPosition.x - volume.mSize.x / 2.0f};
        auto& position {transform.GetPosition()};
        
        if (position.x > rightLimit && cube.mVelocity.x > 0.0f) {
            cube.mVelocity.x = -cube.mVelocity.x;
        }
        
        if (position.x < leftLimit && cube.mVelocity.x < 0.0f) {
            cube.mVelocity.x = -cube.mVelocity.x;
        }
    }
    
    if (mScene == nullptr) {
        mSceneObject->Update(false);
    }
}
