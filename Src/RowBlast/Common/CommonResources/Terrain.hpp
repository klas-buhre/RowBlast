#ifndef Terrain_hpp
#define Terrain_hpp

#include <vector>

// Engine includes:
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
}

namespace RowBlast {
    enum class TerrainMaterial {
        Sand1,
        Sand1Bright,
        Sand3,
        Sand3Bright,
        Moon1,
        Moon3
    };
    
    enum class TerrainMesh {
        Mesh1,
        Mesh3,
    };
    
    struct TerrainSegment {
        Pht::Vec3 mPosition;
        TerrainMesh mMesh {TerrainMesh::Mesh1};
        TerrainMaterial mMaterail {TerrainMaterial::Sand1};
        Pht::Vec3 mRotation {0.0f, 0.0f, 0.0f};
    };

    void CreateTerrain(Pht::IEngine& engine,
                       Pht::Scene& scene,
                       int layerIndex,
                       const std::vector<TerrainSegment>& segments);
}

#endif
