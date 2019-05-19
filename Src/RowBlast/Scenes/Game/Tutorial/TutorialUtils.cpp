#include "TutorialUtils.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "GuiView.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"
#include "IGuiLightProvider.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 lightDirectionA {0.785f, 1.0f, 0.67f};
    const Pht::Vec3 lightDirectionB {1.0f, 1.0f, 0.74f};
    constexpr auto borderWidth = 0.08f;
    constexpr auto width = 7.0f;
    constexpr auto height = 9.0f;

    Pht::SceneObject& CreateBlock(Pht::GuiView& view,
                                  const Pht::Vec3& position,
                                  Pht::RenderableObject& blockRenderable,
                                  Pht::SceneObject& parent) {
        auto& block = view.CreateSceneObject();
        auto& transform = block.GetTransform();
        transform.SetPosition(position);
        transform.SetScale(0.8f);
        block.SetRenderable(&blockRenderable);
        parent.AddChild(block);
        
        return block;
    }
    
    void CreateWeld(Pht::GuiView& view,
                    const Pht::Vec3& position,
                    Pht::RenderableObject& weldRenderable,
                    float rotation,
                    Pht::SceneObject& parent) {
        auto& weld = view.CreateSceneObject();
        auto& transform = weld.GetTransform();
        transform.SetPosition(position);
        transform.SetScale(0.8f);
        transform.SetRotation({0.0f, 0.0f, rotation});
        weld.SetRenderable(&weldRenderable);
        parent.AddChild(weld);
    }
}

void TutorialUtils::CreateFieldQuad(Pht::IEngine& engine,
                                    Pht::GuiView& view,
                                    Pht::SceneObject& parent) {
    Pht::Material fieldMaterial;
    fieldMaterial.SetOpacity(0.96f);
    
    auto f = 0.9f;
    
    Pht::QuadMesh::Vertices vertices {
        {{-width / 2.0f, -height / 2.0f, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 1.0f}},
        {{width / 2.0f, -height / 2.0f, 0.0f}, {0.8f * f, 0.225f * f, 0.425f * f, 1.0f}},
        {{width / 2.0f, height / 2.0f, 0.0f}, {0.3f * f, 0.3f * f, 0.752f * f, 1.0f}},
        {{-width / 2.0f, height / 2.0f, 0.0f}, {0.81f * f, 0.225f * f, 0.425f * f, 1.0f}},
    };

    auto& fieldQuad =
        view.CreateSceneObject(Pht::QuadMesh {vertices}, fieldMaterial, engine.GetSceneManager());
    
    fieldQuad.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    parent.AddChild(fieldQuad);
}

void TutorialUtils::CreateFieldQuadFrame(Pht::IEngine& engine,
                                         Pht::GuiView& view,
                                         Pht::SceneObject& parent) {
    Pht::Material lineMaterial {Pht::Color{0.0f, 0.0f, 0.0f}};
    lineMaterial.SetOpacity(0.325f);
    auto& sceneManager = engine.GetSceneManager();
    
    auto& container = view.CreateSceneObject();
    container.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::textShadow});
    parent.AddChild(container);
    
    auto& lineSceneObject =
        view.CreateSceneObject(Pht::QuadMesh {width + borderWidth, borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject.GetTransform().SetPosition({0.0f, height / 2.0f, 0.0f});
    container.AddChild(lineSceneObject);
    
    auto& lineSceneObject2 =
        view.CreateSceneObject(Pht::QuadMesh {width + borderWidth, borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject2.GetTransform().SetPosition({0.0f, -height / 2.0f, 0.0f});
    container.AddChild(lineSceneObject2);

    auto& lineSceneObject3 =
        view.CreateSceneObject(Pht::QuadMesh {borderWidth, height - borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject3.GetTransform().SetPosition({-width / 2.0f, 0.0f, 0.0f});
    container.AddChild(lineSceneObject3);

    auto& lineSceneObject4 =
        view.CreateSceneObject(Pht::QuadMesh {borderWidth, height - borderWidth},
                               lineMaterial,
                               sceneManager);
    lineSceneObject4.GetTransform().SetPosition({width / 2.0f, 0.0f, 0.0f});
    container.AddChild(lineSceneObject4);
}

void TutorialUtils::SetGuiLightDirections(IGuiLightProvider& guiLightProvider) {
    guiLightProvider.SetGuiLightDirections(lightDirectionA, lightDirectionB);
}

Pht::SceneObject& TutorialUtils::CreateThreeGrayBlocks(Pht::GuiView& view,
                                                       const Pht::Vec3& position,
                                                       Pht::SceneObject& parent,
                                                       const LevelResources& levelResources) {
    auto& blocks = view.CreateSceneObject();
    parent.AddChild(blocks);
    
    blocks.GetTransform().SetPosition(position);
    
    auto& blockRenderable = levelResources.GetLevelBlockRenderable(BlockKind::Full);

    auto halfCellSize = 0.5f;
    CreateBlock(view, {-halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock(view, {0.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock(view, {halfCellSize * 2.0f, 0.0f, 0.0f}, blockRenderable, blocks);
    
    return blocks;
}

Pht::SceneObject& TutorialUtils::CreateGrayBlock(Pht::GuiView& view,
                                                 const Pht::Vec3& position,
                                                 Pht::SceneObject& parent,
                                                 const LevelResources& levelResources) {
    auto& blockRenderable = levelResources.GetLevelBlockRenderable(BlockKind::Full);
    return CreateBlock(view, position, blockRenderable, parent);
}

Pht::SceneObject& TutorialUtils::CreateTwoBlocks(Pht::GuiView& view,
                                                 const Pht::Vec3& position,
                                                 BlockColor color,
                                                 float rotation,
                                                 Pht::SceneObject& parent,
                                                 const PieceResources& pieceResources) {
    auto& blocks = view.CreateSceneObject();
    parent.AddChild(blocks);
    
    auto& transform = blocks.GetTransform();
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, rotation});
    
    auto& blockRenderable =
        pieceResources.GetBlockRenderableObject(BlockKind::Full, color, BlockBrightness::Normal);

    auto halfCellSize = 0.5f;
    CreateBlock(view, {-halfCellSize, 0.0f, 0.0f}, blockRenderable, blocks);
    CreateBlock(view, {halfCellSize, 0.0f, 0.0f}, blockRenderable, blocks);
    
    auto& weldRenderable = pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                                                  color,
                                                                  BlockBrightness::Normal);

    CreateWeld(view, {0.0f, 0.0f, halfCellSize}, weldRenderable, 0.0f, blocks);
    
    return blocks;
}

Pht::SceneObject& TutorialUtils::CreateColoredBlock(Pht::GuiView& view,
                                                    const Pht::Vec3& position,
                                                    BlockColor color,
                                                    Pht::SceneObject& parent,
                                                    const PieceResources& pieceResources) {
    auto& blockRenderable =
        pieceResources.GetBlockRenderableObject(BlockKind::Full, color, BlockBrightness::Normal);

    return CreateBlock(view, position, blockRenderable, parent);
}

Pht::SceneObject& TutorialUtils::CreateRowBomb(Pht::GuiView& view,
                                               const Pht::Vec3& position,
                                               Pht::SceneObject& parent,
                                               const PieceResources& pieceResources) {
    auto& rowBombRenderable = pieceResources.GetRowBombRenderableObject();
    return CreateBlock(view, position, rowBombRenderable, parent);
}

Pht::SceneObject& TutorialUtils::CreateTransparentRowBomb(Pht::GuiView& view,
                                                          const Pht::Vec3& position,
                                                          Pht::SceneObject& parent,
                                                          const PieceResources& pieceResources) {
    auto& transparentRowBombRenderable = pieceResources.GetTransparentRowBombRenderableObject();
    return CreateBlock(view, position, transparentRowBombRenderable, parent);
}

Pht::SceneObject& TutorialUtils::CreateBomb(Pht::GuiView& view,
                                            const Pht::Vec3& position,
                                            Pht::SceneObject& parent,
                                            const PieceResources& pieceResources) {
    auto& bombRenderable = pieceResources.GetBombRenderableObject();
    return CreateBlock(view, position, bombRenderable, parent);
}

Pht::SceneObject& TutorialUtils::CreateTransparentBomb(Pht::GuiView& view,
                                                       const Pht::Vec3& position,
                                                       Pht::SceneObject& parent,
                                                       const PieceResources& pieceResources) {
    auto& transparentBombRenderable = pieceResources.GetTransparentBombRenderableObject();
    return CreateBlock(view, position, transparentBombRenderable, parent);
}

Pht::SceneObject& TutorialUtils::CreateLevelBomb(Pht::GuiView& view,
                                                 const Pht::Vec3& position,
                                                 Pht::SceneObject& parent,
                                                 const LevelResources& levelResources) {
    auto& levelBombRenderable = levelResources.GetLevelBombRenderable();
    auto& sceneObject = CreateBlock(view, position, levelBombRenderable, parent);
    sceneObject.GetTransform().SetRotation({110.0f, -20.0f, 0.0f});
    return sceneObject;
}

Pht::SceneObject& TutorialUtils::CreateAsteroid(Pht::GuiView& view,
                                                const Pht::Vec3& position,
                                                Pht::SceneObject& parent,
                                                const LevelResources& levelResources) {
    auto& asteroidRenderable = levelResources.GetSmallAsteroidRenderable();
    auto& sceneObject = CreateBlock(view, position, asteroidRenderable, parent);
    return sceneObject;
}

Pht::SceneObject& TutorialUtils::CreateLPiece(Pht::GuiView& view,
                                              const Pht::Vec3& position,
                                              float rotation,
                                              Pht::SceneObject& parent,
                                              const PieceResources& pieceResources) {
    auto& lPiece = view.CreateSceneObject();
    parent.AddChild(lPiece);
    
    auto& transform = lPiece.GetTransform();
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, rotation});
    
    auto& blockRenderable = pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                                    BlockColor::Yellow,
                                                                    BlockBrightness::Normal);

    auto halfCellSize = 0.5f;
    CreateBlock(view, {-halfCellSize, -halfCellSize, 0.0f}, blockRenderable, lPiece);
    CreateBlock(view, {halfCellSize, -halfCellSize, 0.0f}, blockRenderable, lPiece);
    CreateBlock(view, {halfCellSize, halfCellSize, 0.0f}, blockRenderable, lPiece);
    
    auto& weldRenderable = pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                                                  BlockColor::Yellow,
                                                                  BlockBrightness::Normal);

    CreateWeld(view, {0.0f, -halfCellSize, halfCellSize}, weldRenderable, 0.0f, lPiece);
    CreateWeld(view, {halfCellSize, 0.0f, halfCellSize}, weldRenderable, 90.0f, lPiece);
    
    return lPiece;
}

Pht::SceneObject& TutorialUtils::CreateBPiece(Pht::GuiView& view,
                                              const Pht::Vec3& position,
                                              float rotation,
                                              Pht::SceneObject& parent,
                                              const PieceResources& pieceResources) {
    auto& bPiece = view.CreateSceneObject();
    parent.AddChild(bPiece);
    
    auto& transform = bPiece.GetTransform();
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, rotation});
    
    auto& blockRenderable = pieceResources.GetBlockRenderableObject(BlockKind::Full,
                                                                    BlockColor::Blue,
                                                                    BlockBrightness::Normal);

    auto halfCellSize = 0.5f;
    CreateBlock(view, {0.0f, halfCellSize, 0.0f}, blockRenderable, bPiece);
    CreateBlock(view, {2.0f * halfCellSize, halfCellSize, 0.0f}, blockRenderable, bPiece);
    CreateBlock(view, {-2.0f * halfCellSize, -halfCellSize, 0.0f}, blockRenderable, bPiece);
    CreateBlock(view, {0.0f, -halfCellSize, 0.0f}, blockRenderable, bPiece);
    CreateBlock(view, {2.0f * halfCellSize, -halfCellSize, 0.0f}, blockRenderable, bPiece);
    
    auto& weldRenderable = pieceResources.GetWeldRenderableObject(WeldRenderableKind::Normal,
                                                                  BlockColor::Blue,
                                                                  BlockBrightness::Normal);

    CreateWeld(view, {halfCellSize, halfCellSize, halfCellSize}, weldRenderable, 0.0f, bPiece);
    CreateWeld(view, {-halfCellSize, -halfCellSize, halfCellSize}, weldRenderable, 0.0f, bPiece);
    CreateWeld(view, {halfCellSize, -halfCellSize, halfCellSize}, weldRenderable, 0.0f, bPiece);
    CreateWeld(view, {0.0f, 0.0f, halfCellSize}, weldRenderable, 90.0f, bPiece);
    CreateWeld(view, {2.0f * halfCellSize, 0.0f, halfCellSize}, weldRenderable, 90.0f, bPiece);
    
    return bPiece;
}

Pht::SceneObject& TutorialUtils::CreatePieceGhostPiece(Pht::GuiView& view,
                                                       const std::string& pieceName,
                                                       const Pht::Vec3& position,
                                                       float rotation,
                                                       Pht::SceneObject& parent,
                                                       const LevelResources& levelResources) {
    auto& ghostPiece = view.CreateSceneObject();
    parent.AddChild(ghostPiece);
    
    auto& transform = ghostPiece.GetTransform();
    transform.SetPosition(position);
    transform.SetScale(0.8f);
    transform.SetRotation({0.0f, 0.0f, rotation});
    
    auto& pieceTypes = levelResources.GetPieceTypes();
    auto i = pieceTypes.find(pieceName);
    assert(i != std::end(pieceTypes));
    ghostPiece.SetRenderable(i->second->GetGhostPieceRenderable());
    
    return ghostPiece;
}
