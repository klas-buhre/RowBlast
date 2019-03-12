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
    class CommonResources;
    
    using PieceTypes = std::map<std::string, std::unique_ptr<const Piece>>;
    
    class LevelResources {
    public:
        LevelResources(Pht::IEngine& engine,
                       const GameScene& scene,
                       const CommonResources& commonResources);

        Pht::RenderableObject& GetLevelBlockRenderable(BlockKind blockKind) const;
        
        const PieceTypes& GetPieceTypes() const {
            return mPieceTypes;
        }
        
        Pht::RenderableObject& GetBlueprintSlotRenderable() const {
            return *mBlueprintSquare;
        }

        Pht::RenderableObject& GetBlueprintSlotNonDepthWritingRenderable() const {
            return *mBlueprintSquareNonDepthWriting;
        }

        Pht::RenderableObject& GetBlueprintSlotAnimationRenderable() const {
            return *mBlueprintSquareAnimation;
        }

        Pht::RenderableObject& GetFieldCellRenderable() const {
            return *mFieldCell;
        }

        Pht::RenderableObject& GetLevelBombRenderable() const {
            return *mLevelBomb;
        }

        Pht::RenderableObject& GetBigAsteroidRenderable() const {
            return *mBigAsteroid;
        }

        Pht::RenderableObject& GetSmallAsteroidRenderable() const {
            return *mSmallAsteroid;
        }

        Pht::RenderableObject& GetAsteroidFragmentRenderable() const {
            return *mAsteroidFragment;
        }

    private:
        void CreatePieceTypes(Pht::IEngine& engine, const GameScene& scene);
        void CreateCellRenderables(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateBlueprintRenderables(Pht::IEngine& engine,
                                        const GameScene& scene,
                                        const CommonResources& commonResources);
        void CreateLevelBombRenderable(Pht::IEngine& engine);
        void CreateBigAsteroidRenderable(Pht::IEngine& engine);
        void CreateSmallAsteroidRenderable(Pht::IEngine& engine);
        void CreateAsteroidFragmentRenderable(Pht::IEngine& engine);
        
        PieceTypes mPieceTypes;
        std::unique_ptr<Pht::RenderableObject> mGrayCube;
        std::unique_ptr<Pht::RenderableObject> mGrayTriangle;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquare;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquareNonDepthWriting;
        std::unique_ptr<Pht::RenderableObject> mBlueprintSquareAnimation;
        std::unique_ptr<Pht::RenderableObject> mFieldCell;
        std::unique_ptr<Pht::RenderableObject> mLevelBomb;
        std::unique_ptr<Pht::RenderableObject> mBigAsteroid;
        std::unique_ptr<Pht::RenderableObject> mSmallAsteroid;
        std::unique_ptr<Pht::RenderableObject> mAsteroidFragment;
    };
}

#endif
