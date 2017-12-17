#include "Clouds.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "MathUtils.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "QuadMesh.hpp"

using namespace BlocksGame;

Clouds::Clouds(Pht::IEngine& engine,
               Pht::Scene& scene,
               int layerIndex,
               const std::vector<CloudPathVolume>& volumes,
               float velocity) :
    mEngine {engine},
    mPathVolumes {volumes} {
    
    mClouds.resize(mPathVolumes.size());
    
    auto& sceneObject {scene.CreateSceneObject()};
    sceneObject.SetLayer(layerIndex);
    scene.GetRoot().AddChild(sceneObject);
    
    Pht::Material cloudMaterial {"cloud_A.png", 0.9f, 0.0f, 0.0f, 0.0f};
    cloudMaterial.SetBlend(Pht::Blend::Yes);

    for (auto i {0}; i < mClouds.size(); ++i) {
        auto& cloud {mClouds[i]};
        const auto& volume {mPathVolumes[i]};
        auto& cloudSize {volume.mCloudSize};
        cloud.mSceneObject = &scene.CreateSceneObject(Pht::QuadMesh {cloudSize.x, cloudSize.y},
                                                      cloudMaterial);
        sceneObject.AddChild(*cloud.mSceneObject);
        
        Pht::Vec3 cloudPosition {
            Pht::NormalizedRand() * volume.mSize.x - volume.mSize.x / 2.0f + volume.mPosition.x,
            Pht::NormalizedRand() * volume.mSize.y - volume.mSize.y / 2.0f + volume.mPosition.y,
            Pht::NormalizedRand() * volume.mSize.z - volume.mSize.z / 2.0f + volume.mPosition.z
        };
        
        Pht::Vec3 cloudVelocity {
            volume.mSize == Pht::Vec3{0.0f, 0.0f, 0.0f} ? 0.0f : velocity,
            0.0f,
            0.0f
        };
        
        cloud.mVelocity = cloudVelocity;
        cloud.mSceneObject->GetTransform().SetPosition(cloudPosition);
    }
}

void Clouds::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};

    for (auto i {0}; i < mClouds.size(); ++i) {
        auto& cloud {mClouds[i]};
        auto& transform {cloud.mSceneObject->GetTransform()};
        transform.Translate(cloud.mVelocity * dt);
        
        const auto& volume {mPathVolumes[i]};
        auto rightLimit {volume.mPosition.x + volume.mSize.x / 2.0f};
        auto leftLimit {volume.mPosition.x - volume.mSize.x / 2.0f};
        auto position {cloud.mSceneObject->GetWorldSpacePosition()};
        
        if (position.x > rightLimit) {
            transform.Translate({-volume.mSize.x, 0.0f, 0.0f});
        } else if (position.x < leftLimit) {
            transform.Translate({volume.mSize.x, 0.0f, 0.0f});
        }
    }
}
