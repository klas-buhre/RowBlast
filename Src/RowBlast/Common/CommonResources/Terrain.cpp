#include "Terrain.hpp"

// Engine includes:
#include "IEngine.hpp"
#include "Scene.hpp"
#include "ObjMesh.hpp"
#include "Material.hpp"

using namespace RowBlast;

namespace {
    Pht::ObjMesh ToObjMesh(TerrainMesh terrainMesh) {
        switch (terrainMesh) {
            case TerrainMesh::Mesh1:
                return Pht::ObjMesh {"terrain1_2888.obj", 1.0f};
            case TerrainMesh::Mesh3:
                return Pht::ObjMesh {"terrain3_2888.obj", 1.0f};
        }
    }
    
    Pht::Material ToMaterial(TerrainMaterial terrainMaterial) {
        switch (terrainMaterial) {
            case TerrainMaterial::Sand1:
                return Pht::Material {"terrain1_2.jpg", 0.6f, 0.6f, 0.1f, 1.0f};
            case TerrainMaterial::Sand1Bright:
                return Pht::Material {"terrain1_2.jpg", 0.6f, 0.615f, 0.1f, 1.0f};
            case TerrainMaterial::Sand3:
                return Pht::Material {"terrain3_2.jpg", 0.6f, 0.6f, 0.1f, 1.0f};
            case TerrainMaterial::Sand3Bright:
                return Pht::Material {"terrain3_2.jpg", 0.6f, 0.615f, 0.1f, 1.0f};
            case TerrainMaterial::Moon1:
                return Pht::Material {"terrain1_4.jpg", 0.05f, 0.72f, 0.1f, 1.0f};
            case TerrainMaterial::Moon3:
                return Pht::Material {"terrain3_4.jpg", 0.05f, 0.72f, 0.1f, 1.0f};
        }
    }
}

void RowBlast::CreateTerrain(Pht::IEngine& engine,
                             Pht::Scene& scene,
                             int layerIndex,
                             const std::vector<TerrainSegment>& segments) {
    auto& container = scene.CreateSceneObject();
    container.SetLayer(layerIndex);
    scene.GetRoot().AddChild(container);
    
    for (auto& segment: segments) {
        auto objMesh = ToObjMesh(segment.mMesh);
        auto material = ToMaterial(segment.mMaterail);
        auto& segmentSceneObject = scene.CreateSceneObject(objMesh, material);
        
        auto& transform = segmentSceneObject.GetTransform();
        transform.SetPosition(segment.mPosition);
        transform.SetRotation(segment.mRotation);
        
        container.AddChild(segmentSceneObject);
    }
}
