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
        Moon1
    };
    
    enum class TerrainMesh {
        Mesh1
    };
    
    struct TerrainSegment {
        Pht::Vec3 mPosition;
        TerrainMesh mMesh {TerrainMesh::Mesh1};
        TerrainMaterial mMaterail {TerrainMaterial::Sand1};
    };

    void CreateTerrain(Pht::IEngine& engine,
                       Pht::Scene& scene,
                       int layerIndex,
                       const std::vector<TerrainSegment>& segments);
}

#endif
