#include "PieceResources.hpp"

#include <assert.h>

// Engine includes.
#include "ISceneManager.hpp"
#include "Material.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "GameScene.hpp"
#include "FlashingBlocksAnimation.hpp"

using namespace BlocksGame;

namespace {
    std::string ToMeshName(BlockRenderable blockRenderable) {
        switch (blockRenderable) {
            case BlockRenderable::LowerRightHalf:
                return "triangle_428.obj";
            case BlockRenderable::UpperRightHalf:
                return "triangle_428_r270.obj";
            case BlockRenderable::UpperLeftHalf:
                return "triangle_428_r180.obj";
            case BlockRenderable::LowerLeftHalf:
                return "triangle_428_r90.obj";
            case BlockRenderable::Full:
                return "cube_428.obj";
            case BlockRenderable::None:
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

PieceResources::PieceResources(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    CreateBlocks(sceneManager, scene);
    CreateWelds(sceneManager, scene);
}

Pht::RenderableObject& PieceResources::GetBlockRenderableObject(BlockRenderable blockRenderable,
                                                                BlockColor color,
                                                                BlockBrightness brightness) {
    auto blockRenderableIndex {static_cast<int>(blockRenderable)};
    auto colorIndex {static_cast<int>(color)};
    auto brightnessIndex {static_cast<int>(brightness)};
    
    assert(blockRenderableIndex >= 0 && blockRenderableIndex < Quantities::numBlockRenderables &&
           colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBlockBrightness);
    
    return *(mBlocks[blockRenderableIndex][colorIndex][brightnessIndex]);
}

Pht::RenderableObject& PieceResources::GetWeldRenderableObject(BlockColor color,
                                                               BlockBrightness brightness) {
    auto colorIndex {static_cast<int>(color)};
    auto brightnessIndex {static_cast<int>(brightness)};
    
    assert(colorIndex >= 0 && colorIndex < Quantities::numBlockColors &&
           brightnessIndex >= 0 && brightnessIndex < Quantities::numBlockBrightness);

    return *(mWelds[colorIndex][brightnessIndex]);
}

void PieceResources::CreateBlocks(Pht::ISceneManager& sceneManager, const GameScene& scene) {
    mBlocks.resize(Quantities::numBlockRenderables);
    
    for (auto blockRenderableIndex {0};
         blockRenderableIndex < Quantities::numBlockRenderables;
         ++blockRenderableIndex) {
        mBlocks[blockRenderableIndex].resize(Quantities::numBlockColors);
        
        for (auto colorIndex {0}; colorIndex < Quantities::numBlockColors; ++colorIndex) {
            mBlocks[blockRenderableIndex][colorIndex].resize(Quantities::numBlockBrightness);
        }
    }
    
    auto cellSize {scene.GetCellSize()};
    
    for (auto blockRenderableIndex {0};
         blockRenderableIndex < Quantities::numBlockRenderables;
         ++blockRenderableIndex) {
        for (auto colorIndex {0}; colorIndex < Quantities::numBlockColors; ++colorIndex) {
            for (auto brightnessIndex {0};
                 brightnessIndex < Quantities::numBlockBrightness;
                 ++brightnessIndex) {
                auto meshName {ToMeshName(static_cast<BlockRenderable>(blockRenderableIndex))};
                
                auto material {
                    ToMaterial(static_cast<BlockColor>(colorIndex),
                               static_cast<BlockBrightness>(brightnessIndex),
                               scene)
                };
                
                mBlocks[blockRenderableIndex][colorIndex][brightnessIndex] =
                    sceneManager.CreateRenderableObject(Pht::ObjMesh {meshName, cellSize}, material);
            }
        }
    }
}

void PieceResources::CreateWelds(Pht::ISceneManager& sceneManager, const GameScene& scene) {

}
