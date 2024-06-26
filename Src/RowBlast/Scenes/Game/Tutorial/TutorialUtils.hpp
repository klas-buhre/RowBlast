#ifndef TutorialUtils_hpp
#define TutorialUtils_hpp

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"

namespace Pht {
    class IEngine;
    class GuiView;
    class SceneObject;
}

namespace RowBlast {
    class IGuiLightProvider;
    class LevelResources;
    class PieceResources;
    class GhostPieceBlocks;

    namespace TutorialUtils {
        void CreateFieldQuad(Pht::IEngine& engine, Pht::GuiView& view, Pht::SceneObject& parent);
        void CreateFieldQuadFrame(Pht::IEngine& engine, Pht::GuiView& view, Pht::SceneObject& parent);
        void SetGuiLightDirections(IGuiLightProvider& guiLightProvider);
        Pht::SceneObject& CreateThreeGrayBlocks(Pht::GuiView& view,
                                                const Pht::Vec3& position,
                                                Pht::SceneObject& parent,
                                                const LevelResources& levelResources);
        Pht::SceneObject& CreateGrayBlock(Pht::GuiView& view,
                                          const Pht::Vec3& position,
                                          Pht::SceneObject& parent,
                                          const LevelResources& levelResources);
        Pht::SceneObject& CreateTwoBlocks(Pht::GuiView& view,
                                          const Pht::Vec3& position,
                                          BlockColor color,
                                          float rotation,
                                          Pht::SceneObject& parent,
                                          const PieceResources& pieceResources);
        Pht::SceneObject& CreateColoredBlock(Pht::GuiView& view,
                                             const Pht::Vec3& position,
                                             BlockColor color,
                                             Pht::SceneObject& parent,
                                             const PieceResources& pieceResources);
        Pht::SceneObject& CreateRowBomb(Pht::GuiView& view,
                                        const Pht::Vec3& position,
                                        Pht::SceneObject& parent,
                                        const PieceResources& pieceResources);
        Pht::SceneObject& CreateTransparentRowBomb(Pht::GuiView& view,
                                                   const Pht::Vec3& position,
                                                   Pht::SceneObject& parent,
                                                   const PieceResources& pieceResources);
        Pht::SceneObject& CreateBomb(Pht::GuiView& view,
                                     const Pht::Vec3& position,
                                     Pht::SceneObject& parent,
                                     const PieceResources& pieceResources);
        Pht::SceneObject& CreateTransparentBomb(Pht::GuiView& view,
                                                const Pht::Vec3& position,
                                                Pht::SceneObject& parent,
                                                const PieceResources& pieceResources);
        Pht::SceneObject& CreateLevelBomb(Pht::GuiView& view,
                                          const Pht::Vec3& position,
                                          Pht::SceneObject& parent,
                                          const LevelResources& levelResources);
        Pht::SceneObject& CreateAsteroid(Pht::GuiView& view,
                                         const Pht::Vec3& position,
                                         Pht::SceneObject& parent,
                                         const LevelResources& levelResources);
        Pht::SceneObject& CreateLPiece(Pht::GuiView& view,
                                       const Pht::Vec3& position,
                                       float rotation,
                                       Pht::SceneObject& parent,
                                       const PieceResources& pieceResources);
        Pht::SceneObject& CreateBPiece(Pht::GuiView& view,
                                       const Pht::Vec3& position,
                                       float rotation,
                                       Pht::SceneObject& parent,
                                       const PieceResources& pieceResources);
        Pht::SceneObject& CreateLPieceGhostPiece(Pht::GuiView& view,
                                                 const Pht::Vec3& position,
                                                 float rotation,
                                                 Pht::SceneObject& parent,
                                                 const GhostPieceBlocks& ghostPieceBlocks);
        Pht::SceneObject& CreateBPieceGhostPiece(Pht::GuiView& view,
                                                 const Pht::Vec3& position,
                                                 float rotation,
                                                 Pht::SceneObject& parent,
                                                 const GhostPieceBlocks& ghostPieceBlocks);
        Pht::SceneObject& CreatePieceGhostPiece(Pht::GuiView& view,
                                                const std::string& pieceName,
                                                const Pht::Vec3& position,
                                                float rotation,
                                                Pht::SceneObject& parent,
                                                const LevelResources& levelResources);
    }
}

#endif
