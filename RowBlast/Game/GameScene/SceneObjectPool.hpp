#ifndef SceneObjectPool_hpp
#define SceneObjectPool_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "SceneObject.hpp"

namespace RowBlast {
    class Level;
    
    enum class SceneObjectPoolKind {
        FieldBlocks,
        PieceBlocks,
        GhostPieces,
        PreviewPieceBlocks
    };
    
    class SceneObjectPool {
    public:
        SceneObjectPool(SceneObjectPoolKind poolKind,
                        Pht::SceneObject& parentSceneObject,
                        const Level& level);
        
        void ReclaimAll();
        Pht::SceneObject& AccuireSceneObject();
        void SetIsActive(bool isActive);
        bool IsActive() const;
        
        Pht::SceneObject& GetContainerSceneObject() {
            return *mContainerSceneObject;
        }
        
    private:
        std::unique_ptr<Pht::SceneObject> mContainerSceneObject;
        std::vector<std::unique_ptr<Pht::SceneObject>> mSceneObjects;
        int mNextAvailableIndex {0};
    };
}

#endif
