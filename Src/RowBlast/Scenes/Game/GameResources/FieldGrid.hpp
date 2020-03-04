#ifndef FieldGrid_hpp
#define FieldGrid_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;
    class Level;

    class FieldGrid {
    public:
        FieldGrid(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources);
    
        void Init(const Level& level);
        
        struct Segment {
            int mRow {0};
            Pht::SceneObject& mSceneObject;
        };
        
        const std::vector<Segment>& GetSegments() const {
            return mSegments;
        }
        
    private:
        Pht::RenderableObject& GetGridSegmentRenderable(int gridSegmentWidthNumCells);
        void AddBottomRow(Pht::SceneObject& container,
                          int numLevelRows,
                          int gridSegmentWidthNumCells);
        
        GameScene& mScene;
        std::unique_ptr<Pht::RenderableObject> mGridSegmentSize7Renderable;
        std::unique_ptr<Pht::RenderableObject> mGridSegmentSize8Renderable;
        std::unique_ptr<Pht::RenderableObject> mGridSegmentSize9Renderable;
        std::vector<Segment> mSegments;
    };
}

#endif
