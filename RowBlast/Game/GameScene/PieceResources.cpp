#include "PieceResources.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "Material.hpp"
#include "ObjMesh.hpp"
#include "SphereMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "FlashingBlocksAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto numWeldRenderables {3};
    
    std::string ToMeshName(BlockKind blockKind) {
        switch (blockKind) {
            case BlockKind::LowerRightHalf:
                return "triangle_320.obj";
            case BlockKind::UpperRightHalf:
                return "triangle_320_r270.obj";
            case BlockKind::UpperLeftHalf:
                return "triangle_320_r180.obj";
            case BlockKind::LowerLeftHalf:
                return "triangle_320_r90.obj";
            case BlockKind::Full:
                return "cube_428.obj";
            default:
                assert(!"Unsupported block kind");
                break;
        }
    }
    
    const Pht::Material& ToMaterial(BlockColor color, const GameScene& scene) {
        switch (color) {
            case BlockColor::Red:
                return scene.GetRedMaterial();
            case BlockColor::Green:
                return scene.GetGreenMaterial();
            case BlockColor::Blue:
                return scene.GetBlueMaterial();
            case BlockColor::Yellow:
                return scene.GetYellowMaterial();
            case BlockColor::None:
                assert(!"Not a color");
                break;
        }
    }
    
    void AddColorToMaterial(Pht::Material& material, const Pht::Color& colorAdd) {
        material.SetAmbient(material.GetAmbient() + colorAdd);
        material.SetDiffuse(material.GetDiffuse() + colorAdd);
        material.SetSpecular(material.GetSpecular() + colorAdd);
    }
    
    Pht::Material ToMaterial(BlockColor color, BlockBrightness brightness, const GameScene& scene) {
        auto material {ToMaterial(color, scene)};
        
        switch (brightness) {
            case BlockBrightness::Normal:
                break;
            case BlockBrightness::Flashing:
                AddColorToMaterial(material, FlashingBlocksAnimation::colorAdd);
                break;
            case BlockBrightness::BlueprintFillFlashing:
                AddColorToMaterial(material, FlashingBlocksAnimation::brightColorAdd);
                break;
            case BlockBrightness::SemiFlashing:
                AddColorToMaterial(material, FlashingBlocksAnimation::semiFlashingColorAdd);
                break;
        }
        
        return material;
    }
    
    std::unique_ptr<Pht::RenderableObject>
    ToWeldRenderableObject(WeldRenderableKind weldRenderableKind,
                           const Pht::Material& material,
                           Pht::ISceneManager& sceneManager) {
        switch (weldRenderableKind) {
            case WeldRenderableKind::Normal:
                return sceneManager.CreateRenderableObject(Pht::QuadMesh {0.17f,
                                                                          0.85f,
                                                                          std::string{"normalWeld"}},
                                                           material);
            case WeldRenderableKind::Aslope:
                return sceneManager.CreateRenderableObject(Pht::QuadMesh {0.19f,
                                                                          0.85f,
                                                                          std::string{"aslopeWeld"}},
                                                           material);
            case WeldRenderableKind::Diagonal:
                return sceneManager.CreateRenderableObject(Pht::ObjMesh {"weld_76.obj", 0.95f},
                                                           material);
        }
    }
}

PieceResources::PieceResources(Pht::IEngine& engine, const GameScene& scene) {
    auto& sceneManager {engine.GetSceneManager()};
    
    CreateBlocks(sceneManager, scene);
    CreateWelds(sceneManager, scene);
    CreateBombs(sceneManager, scene);
}

Pht::RenderableObject& PieceResources::GetBlockRenderableObject(BlockKind blockKind,
                                                                BlockColor color,
                                                                BlockBrightness brightness) const {
    return *(mBlocks[CalcBlockIndex(blockKind, color, brightness)]);
}

int PieceResources::CalcBlockIndex(BlockKind blockKind,
                                   BlockColor color,
                                   BlockBrightness brightness) const {
    auto blockKindIndex {static_cast<int>(blockKind)};
    auto colorIndex {static_cast<int>(color)};
    auto brightnessIndex {static_cast<int>(brightness)};
    
    assert(blockKindIndex >= 0 && blockKindIndex < Quantities::numBlockRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBlockBrightness);

    auto index {
        brightnessIndex * Quantities::numBlockRenderables * Quantities::numBlockColors +
        colorIndex * Quantities::numBlockRenderables + blockKindIndex
    };
    
    assert(index < mBlocks.size());
    return index;
}

Pht::RenderableObject& PieceResources::GetWeldRenderableObject(WeldRenderableKind weldRenderable,
                                                               BlockColor color,
                                                               BlockBrightness brightness) const {
    return *(mWelds[CalcWeldIndex(weldRenderable, color, brightness)]);
}

int PieceResources::CalcWeldIndex(WeldRenderableKind weldRenderable,
                                  BlockColor color,
                                  BlockBrightness brightness) const {
    auto weldRenderableIndex {static_cast<int>(weldRenderable)};
    auto colorIndex {static_cast<int>(color)};
    auto brightnessIndex {static_cast<int>(brightness)};
    
    assert(weldRenderableIndex >= 0 && weldRenderableIndex < numWeldRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numWeldBrightness);

    auto index {
        brightnessIndex * numWeldRenderables * Quantities::numBlockColors +
        colorIndex * numWeldRenderables + weldRenderableIndex
    };
    
    assert(index < mWelds.size());
    return index;
}

void PieceResources::CreateBlocks(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    auto numBlocks {
        Quantities::numBlockRenderables * Quantities::numBlockColors * Quantities::numBlockBrightness
    };
    
    mBlocks.resize(numBlocks);
    
    auto cellSize {scene.GetCellSize()};
    
    for (auto blockKindIndex {0};
         blockKindIndex < Quantities::numBlockRenderables;
         ++blockKindIndex) {
        for (auto colorIndex {0}; colorIndex < Quantities::numBlockColors; ++colorIndex) {
            for (auto brightnessIndex {0};
                 brightnessIndex < Quantities::numBlockBrightness;
                 ++brightnessIndex) {
                auto blockKind {static_cast<BlockKind>(blockKindIndex)};
                auto color {static_cast<BlockColor>(colorIndex)};
                auto brightness {static_cast<BlockBrightness>(brightnessIndex)};
                
                auto meshName {ToMeshName(blockKind)};
                auto material {ToMaterial(color, brightness, scene)};
                auto blockIndex {CalcBlockIndex(blockKind, color, brightness)};
                
                auto renderableObject {
                    sceneManager.CreateRenderableObject(Pht::ObjMesh {meshName, cellSize}, material)
                };
                
                mBlocks[blockIndex] = std::move(renderableObject);
            }
        }
    }
}

void PieceResources::CreateWelds(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    auto numWelds {numWeldRenderables * Quantities::numBlockColors * Quantities::numWeldBrightness};
    mWelds.resize(numWelds);
    
    for (auto weldRenderableIndex {0};
         weldRenderableIndex < numWeldRenderables;
         ++weldRenderableIndex) {
        for (auto colorIndex {0}; colorIndex < Quantities::numBlockColors; ++colorIndex) {
            for (auto brightnessIndex {0};
                 brightnessIndex < Quantities::numWeldBrightness;
                 ++brightnessIndex) {
                auto weldRenderableKind {static_cast<WeldRenderableKind>(weldRenderableIndex)};
                auto color {static_cast<BlockColor>(colorIndex)};
                auto brightness {static_cast<BlockBrightness>(brightnessIndex)};
                
                auto material {ToMaterial(color, brightness, scene)};
                auto weldIndex {CalcWeldIndex(weldRenderableKind, color, brightness)};
                mWelds[weldIndex] = ToWeldRenderableObject(weldRenderableKind,
                                                           material,
                                                           sceneManager);
            }
        }
    }
}

void PieceResources::CreateBombs(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    Pht::Material bombMaterial {
        "bomb_798.jpg",
        "bomb_798_emission.jpg",
        1.0f,
        0.87f,
        1.0f,
        1.0f,
        30.0f
    };

    bombMaterial.SetEmissive(Pht::Color {3.0f, 3.0f, 3.0f});
    bombMaterial.GetDepthState().mDepthTestAllowedOverride = true;
    mBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"bomb_798.obj", 16.2f}, bombMaterial);
    
    bombMaterial.SetOpacity(scene.GetGhostPieceOpacity());
    mTransparentBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"bomb_798.obj", 16.2f},
                                                           bombMaterial);

    Pht::Material rowBombMaterial {
        "laser_bomb_diffuse.jpg",
        "laser_bomb_emission.png",
        0.55f,
        0.65f,
        0.05f,
        1.0f,
        20.0f
    };
    
    rowBombMaterial.SetAmbient(Pht::Color {0.5f, 0.5f, 0.85f});
    rowBombMaterial.SetEmissive(Pht::Color {2.0f, 2.0f, 2.0f});
    rowBombMaterial.GetDepthState().mDepthTestAllowedOverride = true;
    mRowBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"laser_bomb_224.obj", 0.6f},
                                                   rowBombMaterial);

    rowBombMaterial.SetOpacity(scene.GetGhostPieceOpacity());
    mTransparentRowBomb = sceneManager.CreateRenderableObject(Pht::ObjMesh {"laser_bomb_224.obj", 0.6f},
                                                              rowBombMaterial);

}
