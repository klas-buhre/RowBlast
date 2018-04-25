#ifndef LevelResources_hpp
#define LevelResources_hpp

#include <map>

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
    class ISceneManager;
}

namespace RowBlast {
    class GameScene;
    
    using PieceTypes = std::map<std::string, std::unique_ptr<const Piece>>;
    
    class LevelResources {
    public:
        LevelResources(Pht::IEngine& engine, const GameScene& scene);

        Pht::RenderableObject& GetLevelBlockRenderable(BlockKind blockKind) const;
        const Piece& GetLPiece() const;
        
        const PieceTypes& GetPieceTypes() const {
            return mPieceTypes;
        }
        
        Pht::RenderableObject& GetBlueprintSlotRenderable() const {
            return *mBlueprintSquare;
        }
        
        Pht::RenderableObject& GetBlueprintSlotAnimationRenderable() const {
            return *mBlueprintSquareAnimation;
        }

        Pht::RenderableObject& GetLevelBombRenderable() const {
            return *mLevelBomb;
        }

    private:
        void CreatePieceTypes(Pht::IEngine& engine, const GameScene& scene);
        void CreateCellRenderables(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateBlueprintRenderables(Pht::IEngine& engine, const GameScene& scene);
        void CreateLevelBombRenderable(Pht::IEngine& engine);
        
        PieceTypes mPieceTypes;
        std::unique_ptr<Pht::RenderableObject> mGrayCube;
        std::unique_ptr<Pht::RenderableObject> mGrayTriangle;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquare;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquareAnimation;
        std::unique_ptr<Pht::RenderableObject> mLevelBomb;
    };
}

#endif