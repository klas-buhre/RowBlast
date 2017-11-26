#include "FloatingCubes.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ObjMesh.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace BlocksGame;

FloatingCubes::FloatingCubes(const std::vector<Volume>& volumes,
                             Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             float scale) :
    mEngine {engine},
    mVolumes {volumes} {
    
    mCubes.resize(mVolumes.size());
    
    Pht::ObjMesh triangle {"triangle_428_085.obj", scale};
    Pht::ObjMesh cube {"cube_554.obj", scale};
    auto& materials {commonResources.GetMaterials()};
    
    mCubeRenderables = {
        engine.CreateRenderableObject(cube, materials.GetGoldMaterial()),
        engine.CreateRenderableObject(cube, materials.GetBlueMaterial()),
        engine.CreateRenderableObject(triangle, materials.GetRedMaterial()),
        engine.CreateRenderableObject(cube, materials.GetGreenMaterial())
    };
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
        
        Pht::Vec3 orientation {
            Pht::NormalizedRand() * 360.0f,
            Pht::NormalizedRand() * 360.0f,
            Pht::NormalizedRand() * 360.0f,
        };
        
        Pht::Vec3 angularVelocity {
            (Pht::NormalizedRand() - 0.5f) * 20.0f,
            (Pht::NormalizedRand() - 0.5f) * 20.0f,
            (Pht::NormalizedRand() - 0.5f) * 20.0f
        };
        
        FloatingCube cube {
            position,
            velocity,
            orientation,
            angularVelocity,
            mCubeRenderables[std::rand() % numRenderables].get()
        };
        
        mCubes[i] = cube;
    }
}

void FloatingCubes::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};

    for (auto i {0}; i < mCubes.size(); ++i) {
        auto& cube {mCubes[i]};
        cube.mPosition += cube.mVelocity * dt;
        cube.mOrientation += cube.mAngularVelocity * dt;
        
        const auto& volume {mVolumes[i]};
        auto rightLimit {volume.mPosition.x + volume.mSize.x / 2.0f};
        auto leftLimit {volume.mPosition.x - volume.mSize.x / 2.0f};
        auto& position {cube.mPosition};
        
        if (position.x > rightLimit && cube.mVelocity.x > 0.0f) {
            cube.mVelocity.x = -cube.mVelocity.x;
        }
        
        if (position.x < leftLimit && cube.mVelocity.x < 0.0f) {
            cube.mVelocity.x = -cube.mVelocity.x;
        }
    }
}
