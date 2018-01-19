#ifndef LevelResources_hpp
#define LevelResources_hpp

#include <map>

// Game includes.
#include "Piece.hpp"

namespace Pht {
    class IEngine;
    class ISceneManager;
}

namespace BlocksGame {
    class GameScene;
    
    using PieceTypes = std::map<std::string, std::unique_ptr<const Piece>>;
    
    struct BlueprintSlotRenderables {
        Pht::RenderableObject* mSlot {nullptr};
        Pht::RenderableObject* mAnimation {nullptr};
    };
    
    class LevelResources {
    public:
        LevelResources(Pht::IEngine& engine, const GameScene& scene);

        const Piece& GetLPiece() const;
        
        const PieceTypes& GetPieceTypes() const {
            return mPieceTypes;
        }
        
        Pht::RenderableObject& GetGrayCube() const {
            return *mGrayCube;
        }
        
        Pht::RenderableObject& GetGoldCube() const {
            return *mGoldCube;
        }

        Pht::RenderableObject& GetGrayTriangle() const {
            return *mGrayTriangle;
        }
        
        Pht::RenderableObject& GetGoldTriangle() const {
            return *mGoldTriangle;
        }
        
        const BlueprintSlotRenderables& GetBlueprintSquareRenderables() const {
            return mBlueprintSquareRenderables;
        }
        
    private:
        void CreatePieceTypes(Pht::IEngine& engine, const GameScene& scene);
        void CreateCellRenderables(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateBlueprintRenderables(Pht::IEngine& engine, const GameScene& scene);
        
        PieceTypes mPieceTypes;
        std::unique_ptr<Pht::RenderableObject> mGrayCube;
        std::unique_ptr<Pht::RenderableObject> mGoldCube;
        std::unique_ptr<Pht::RenderableObject> mGrayTriangle;
        std::unique_ptr<Pht::RenderableObject> mGoldTriangle;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquare;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquareAnimation;
        BlueprintSlotRenderables mBlueprintSquareRenderables;
    };
}

#endif
