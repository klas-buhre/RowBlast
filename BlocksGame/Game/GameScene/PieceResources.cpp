#include "PieceResources.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "Material.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "FlashingBlocksAnimation.hpp"

using namespace BlocksGame;

namespace {
    std::string ToMeshName(BlockRenderableKind blockRenderableKind) {
        switch (blockRenderableKind) {
            case BlockRenderableKind::LowerRightHalf:
                return "triangle_428.obj";
            case BlockRenderableKind::UpperRightHalf:
                return "triangle_428_r270.obj";
            case BlockRenderableKind::UpperLeftHalf:
                return "triangle_428_r180.obj";
            case BlockRenderableKind::LowerLeftHalf:
                return "triangle_428_r90.obj";
            case BlockRenderableKind::Full:
                return "cube_428.obj";
            case BlockRenderableKind::None:
                assert(!"Not a mesh");
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
        }
        
        return material;
    }
}

PieceResources::PieceResources(Pht::IEngine& engine, const GameScene& scene) {
    auto& sceneManager {engine.GetSceneManager()};
    
    CreateBlocks(sceneManager, scene);
    CreateWelds(sceneManager, scene);
}

Pht::RenderableObject& PieceResources::GetBlockRenderableObject(BlockRenderableKind blockRenderable,
                                                                BlockColor color,
                                                                BlockBrightness brightness) {
    return *(mBlocks[CalcBlockIndex(blockRenderable, color, brightness)]);
}

int PieceResources::CalcBlockIndex(BlockRenderableKind blockRenderable,
                                   BlockColor color,
                                   BlockBrightness brightness) {
    auto blockRenderableIndex {static_cast<int>(blockRenderable)};
    auto colorIndex {static_cast<int>(color)};
    auto brightnessIndex {static_cast<int>(brightness)};
    
    assert(blockRenderableIndex >= 0 && blockRenderableIndex < Quantities::numBlockRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBlockBrightness);

    auto index {
        brightnessIndex * Quantities::numBlockRenderables * Quantities::numBlockColors +
        colorIndex * Quantities::numBlockRenderables + blockRenderableIndex
    };
    
    assert(index < mBlocks.size());
    return index;
}

Pht::RenderableObject& PieceResources::GetWeldRenderableObject(BlockColor color,
                                                               BlockBrightness brightness) {
    return *(mWelds[CalcWeldIndex(color, brightness)]);
}

int PieceResources::CalcWeldIndex(BlockColor color, BlockBrightness brightness) {
    auto colorIndex {static_cast<int>(color)};
    auto brightnessIndex {static_cast<int>(brightness)};
    
    assert(colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBlockBrightness);

    auto index {brightnessIndex * Quantities::numBlockColors + colorIndex};
    
    assert(index < mWelds.size());
    return index;
}

void PieceResources::CreateBlocks(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    auto numBlocks {
        Quantities::numBlockRenderables * Quantities::numBlockColors * Quantities::numBlockBrightness
    };
    
    mBlocks.resize(numBlocks);
    
    auto cellSize {scene.GetCellSize()};
    
    for (auto blockRenderableIndex {0};
         blockRenderableIndex < Quantities::numBlockRenderables;
         ++blockRenderableIndex) {
        for (auto colorIndex {0}; colorIndex < Quantities::numBlockColors; ++colorIndex) {
            for (auto brightnessIndex {0};
                 brightnessIndex < Quantities::numBlockBrightness;
                 ++brightnessIndex) {
                auto blockRenderableKind {static_cast<BlockRenderableKind>(blockRenderableIndex)};
                auto color {static_cast<BlockColor>(colorIndex)};
                auto brightness {static_cast<BlockBrightness>(brightnessIndex)};
                
                auto meshName {ToMeshName(blockRenderableKind)};
                auto material {ToMaterial(color, brightness, scene)};
                auto blockIndex {CalcBlockIndex(blockRenderableKind, color, brightness)};
                
                auto renderableObject {
                    sceneManager.CreateRenderableObject(Pht::ObjMesh {meshName, cellSize}, material)
                };
                
                mBlocks[blockIndex] = std::move(renderableObject);
            }
        }
    }
}

void PieceResources::CreateWelds(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    mWelds.resize(Quantities::numBlockColors * Quantities::numBlockBrightness);
    
    for (auto colorIndex {0}; colorIndex < Quantities::numBlockColors; ++colorIndex) {
        for (auto brightnessIndex {0};
             brightnessIndex < Quantities::numBlockBrightness;
             ++brightnessIndex) {
            auto color {static_cast<BlockColor>(colorIndex)};
            auto brightness {static_cast<BlockBrightness>(brightnessIndex)};
            
            auto material {ToMaterial(color, brightness, scene)};
            auto weldIndex {CalcWeldIndex(color, brightness)};
            
            auto renderableObject {
                sceneManager.CreateRenderableObject(Pht::QuadMesh {0.19f, 0.85f}, material)
            };
            
            mWelds[weldIndex] = std::move(renderableObject);
        }
    }
}
