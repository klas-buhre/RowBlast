#include "SceneObjectPool.hpp"

#include <assert.h>

// Game includes.
#include "Level.hpp"
#include "Field.hpp"
#include "Piece.hpp"
#include "ClickInputHandler.hpp"

using namespace BlocksGame;

namespace {
    int CalcPoolSize(SceneObjectPoolKind poolKind, const Level& level) {
        switch (poolKind) {
            case SceneObjectPoolKind::FieldBlocks: {
                auto visibleRows {Field::maxNumRows};
                auto visibleColumns {level.GetNumColumns()};
                return 3 * visibleRows * visibleColumns - visibleRows - visibleColumns;
            }
            case SceneObjectPoolKind::PieceBlocks:
                return 3 * Piece::maxRows * Piece::maxColumns - Piece::maxRows - Piece::maxColumns;
            case SceneObjectPoolKind::GhostPieces:
                return ClickInputHandler::maxNumVisibleMoves;
        }
    }
}

SceneObjectPool::SceneObjectPool(SceneObjectPoolKind poolKind,
                                 Pht::SceneObject& parentSceneObject,
                                 const Level& level) :
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {
    
    parentSceneObject.AddChild(*mContainerSceneObject);
    mSceneObjects.resize(CalcPoolSize(poolKind, level));
    
    for (auto& sceneObject: mSceneObjects) {
        sceneObject = std::make_unique<Pht::SceneObject>();
        mContainerSceneObject->AddChild(*sceneObject);
    }
    
    ReclaimAll();
}

void SceneObjectPool::ReclaimAll() {
    mNextAvailableIndex = 0;

    for (auto& sceneObject: mSceneObjects) {
        sceneObject->SetIsVisible(false);
    }
}

Pht::SceneObject& SceneObjectPool::AccuireSceneObject() {
    assert(mNextAvailableIndex < mSceneObjects.size());
    
    auto& sceneObject {*mSceneObjects[mNextAvailableIndex]};
    sceneObject.SetIsVisible(true);
    ++mNextAvailableIndex;
    return sceneObject;
}