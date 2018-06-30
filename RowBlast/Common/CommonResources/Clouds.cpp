#include "Clouds.hpp"

#include <chrono>
#include <random>

// Engine includes.
#include "IEngine.hpp"
#include "MathUtils.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"

using namespace RowBlast;

namespace {
    const auto averageCloudBrightness {0.9f};
    const auto maxCloudBrightness {0.95f};
    
    const std::vector<std::string> textureFilenames {
        "cloud_A_512.png",
        "cloud_B_512.png",
        "cloud_C_512.png",
        "cloud_D_512.png",
        "cloud_E_512.png",
        "cloud_F_512.png"
    };
    
    int CalcNumClouds(const std::vector<CloudPathVolume>& volumes) {
        auto result {0};
        
        for (auto& volume: volumes) {
            result += volume.mNumClouds;
        }
        
        return result;
    }
    
    Pht::Vec3 CalcRandomPositionInVolume(const CloudPathVolume& volume) {
        return Pht::Vec3 {
            Pht::NormalizedRand() * volume.mSize.x - volume.mSize.x / 2.0f + volume.mPosition.x,
            Pht::NormalizedRand() * volume.mSize.y - volume.mSize.y / 2.0f + volume.mPosition.y,
            Pht::NormalizedRand() * volume.mSize.z - volume.mSize.z / 2.0f + volume.mPosition.z
        };
    }
    
    Pht::Vec3 CalcCloudPosition(const Pht::Vec3& clusterPosition,
                                const CloudPathVolume& volume,
                                std::normal_distribution<float>& xDistribution,
                                std::normal_distribution<float>& yDistribution,
                                std::default_random_engine& randomGenerator) {
        return Pht::Vec3 {
            xDistribution(randomGenerator) + clusterPosition.x,
            yDistribution(randomGenerator) + clusterPosition.y,
            Pht::NormalizedRand() * volume.mSize.z - volume.mSize.z / 2.0f + clusterPosition.z
        };
    }
    
    Pht::Vec3 CalcCloudVelocity(const CloudPathVolume& volume, float velocity) {
        if (volume.mSize == Pht::Vec3 {0.0f, 0.0f, 0.0f}) {
            return Pht::Vec3 {0.0f, 0.0f, 0.0f};
        }
        
        if (volume.mVelocity.HasValue()) {
            return Pht::Vec3 {volume.mVelocity.GetValue(), 0.0f, 0.0f};
        }
        
        return Pht::Vec3 {velocity, 0.0f, 0.0f};
    }
    
    float CalcCloudBrightness(const Pht::Vec3& cloudPosition,
                              const Pht::Vec3& clusterPosition,
                              const Pht::Vec2& clusterSize,
                              const CloudPathVolume& volume) {
        auto localCloudPosition {cloudPosition - clusterPosition};
        
        if (localCloudPosition.x > clusterSize.x / 2.0f) {
            localCloudPosition.x = clusterSize.x / 2.0f;
        } else if (localCloudPosition.x < -clusterSize.x / 2.0f) {
            localCloudPosition.x = -clusterSize.x / 2.0f;
        }

        if (localCloudPosition.y > clusterSize.y / 2.0f) {
            localCloudPosition.y = clusterSize.y / 2.0f;
        } else if (localCloudPosition.y < -clusterSize.y / 2.0f) {
            localCloudPosition.y = -clusterSize.y / 2.0f;
        }
        
        auto brightnessFactor {
            (localCloudPosition.x + localCloudPosition.y + localCloudPosition.z) * 2.0f /
            (clusterSize.x + clusterSize.y + volume.mSize.z)
        };
        
        auto brightness {averageCloudBrightness + brightnessFactor * 0.25f};
        
        if (brightness > maxCloudBrightness) {
            brightness = maxCloudBrightness;
        }
        
        return brightness;
    }
}

Clouds::Clouds(Pht::IEngine& engine,
               Pht::Scene& scene,
               int layerIndex,
               const std::vector<CloudPathVolume>& volumes,
               const std::vector<HazeLayer>& hazeLayers,
               float velocity,
               const Pht::Color& color) :
    mEngine {engine},
    mPathVolumes {volumes} {
    
    mClouds.reserve(CalcNumClouds(mPathVolumes));
    
    auto& sceneObject {scene.CreateSceneObject()};
    sceneObject.SetLayer(layerIndex);
    scene.GetRoot().AddChild(sceneObject);
    
    auto seed {static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())};
    std::default_random_engine randomGenerator {seed};

    for (auto& volume: mPathVolumes) {
        std::normal_distribution<float> xDistribution {0.0, volume.mClusterSize.x / 4.0f};
        std::normal_distribution<float> yDistribution {0.0, volume.mClusterSize.y / 4.0f};
        Pht::Vec3 clusterPosition;
        
        for (auto i {0}; i < volume.mNumClouds; ++i) {
            if (volume.mNumCloudsPerCluster.HasValue()) {
                if (i % volume.mNumCloudsPerCluster.GetValue() == 0) {
                    clusterPosition = CalcRandomPositionInVolume(volume);
                }
            }
            
            auto cloudSizeRandPart {
                volume.mCloudSizeRandPart > 0.0f ?
                Pht::NormalizedRand() * volume.mCloudSizeRandPart - volume.mCloudSizeRandPart / 2.0f :
                0.0f
            };
            
            Pht::Vec2 cloudSize {
                volume.mCloudSize.x + cloudSizeRandPart,
                volume.mCloudSize.y + cloudSizeRandPart
            };
            
            auto cloudPosition {
                volume.mNumCloudsPerCluster.HasValue() ?
                    CalcCloudPosition(clusterPosition,
                                      volume,
                                      xDistribution,
                                      yDistribution,
                                      randomGenerator) :
                    CalcRandomPositionInVolume(volume)
            };
            
            auto cloudBrightness {
                volume.mNumCloudsPerCluster.HasValue() ?
                    CalcCloudBrightness(cloudPosition, clusterPosition, volume.mClusterSize, volume) :
                    averageCloudBrightness
            };
            
            auto& textureFilename {textureFilenames[std::rand() % textureFilenames.size()]};
            Pht::Material cloudMaterial {textureFilename, cloudBrightness, 0.0f, 0.0f, 0.0f};
            cloudMaterial.SetAmbient(color * cloudBrightness);
            cloudMaterial.SetBlend(Pht::Blend::Yes);

            auto& cloudSceneObject = scene.CreateSceneObject(Pht::QuadMesh {cloudSize.x, cloudSize.y},
                                                             cloudMaterial);
            sceneObject.AddChild(cloudSceneObject);
            cloudSceneObject.GetTransform().SetPosition(cloudPosition);
            
            Pht::Vec3 cloudVelocity {CalcCloudVelocity(volume, velocity)};
            Cloud cloud {cloudVelocity, cloudSceneObject, volume};
            mClouds.push_back(cloud);
        }
    }
    
    InitHazeLayers(hazeLayers, scene, layerIndex);
}

void Clouds::InitHazeLayers(const std::vector<HazeLayer>& hazeLayers,
                            Pht::Scene& scene,
                            int sceneLayerIndex) {
    auto& sceneObject {scene.CreateSceneObject()};
    sceneObject.SetLayer(sceneLayerIndex);
    scene.GetRoot().AddChild(sceneObject);
    
    auto frustumHeightFactor {mEngine.GetRenderer().GetFrustumHeightFactor()};

    for (auto& hazeLayer: hazeLayers) {
        auto& size {hazeLayer.mSize};
        
        Pht::QuadMesh::Vertices vertices {
            {{-size.x / 2.0f, -size.y * frustumHeightFactor / 2.0f, 0.0f}, hazeLayer.mLowerColor},
            {{size.x / 2.0f, -size.y * frustumHeightFactor / 2.0f, 0.0f}, hazeLayer.mLowerColor},
            {{size.x / 2.0f, size.y * frustumHeightFactor / 2.0f, 0.0f}, hazeLayer.mUpperColor},
            {{-size.x / 2.0f, size.y * frustumHeightFactor / 2.0f, 0.0f}, hazeLayer.mUpperColor},
        };
        
        Pht::Material material;
        
        if (hazeLayer.mLowerColor.w != 1.0f || hazeLayer.mUpperColor.w != 1.0f) {
            material.SetBlend(Pht::Blend::Yes);
        }
        
        auto& hazeSceneObject {scene.CreateSceneObject(Pht::QuadMesh {vertices}, material)};
        hazeSceneObject.GetTransform().SetPosition(hazeLayer.mPosition);
        hazeSceneObject.SetLayer(sceneLayerIndex);
        sceneObject.AddChild(hazeSceneObject);
    }
}

void Clouds::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    
    for (auto& cloud: mClouds) {
        auto& transform {cloud.mSceneObject.GetTransform()};
        transform.Translate(cloud.mVelocity * dt);
        
        const auto& volume {cloud.mPathVolume};
        auto rightLimit {volume.mPosition.x + volume.mSize.x / 2.0f};
        auto leftLimit {volume.mPosition.x - volume.mSize.x / 2.0f};
        auto position {cloud.mSceneObject.GetWorldSpacePosition()};
        
        if (position.x > rightLimit) {
            transform.Translate({-volume.mSize.x, 0.0f, 0.0f});
        } else if (position.x < leftLimit) {
            transform.Translate({volume.mSize.x, 0.0f, 0.0f});
        }
    }
}
