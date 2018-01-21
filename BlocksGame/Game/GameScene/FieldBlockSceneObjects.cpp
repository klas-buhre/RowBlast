#include "FieldBlockSceneObjects.hpp"

#include <assert.h>

// Game includes.
#include "Level.hpp"
#include "Field.hpp"

using namespace BlocksGame;

FieldBlockSceneObjects::FieldBlockSceneObjects(Pht::SceneObject& parentSceneObject,
                                               const Level& level) :
    mContainerSceneObject {std::make_unique<Pht::SceneObject>()} {
    
    parentSceneObject.AddChild(*mContainerSceneObject);
    
    auto visibleRows {Field::maxNumRows};
    auto visibleColumns {level.GetNumColumns()};
    
    // Maximum number of visible blocks and welds in one screen.
    auto numSceneObjects {3 * visibleRows * visibleColumns - visibleRows - visibleColumns};
    mSceneObjects.resize(numSceneObjects);
    
    for (auto& sceneObject: mSceneObjects) {
        sceneObject = std::make_unique<Pht::SceneObject>();
        mContainerSceneObject->AddChild(*sceneObject);
    }
    
    ReclaimAll();
}

void FieldBlockSceneObjects::ReclaimAll() {
    mNextAvailableIndex = 0;

    for (auto& sceneObject: mSceneObjects) {
        sceneObject->SetIsVisible(false);
    }
}

Pht::SceneObject& FieldBlockSceneObjects::AccuireSceneObject() {
    assert(mNextAvailableIndex < mSceneObjects.size());
    
    auto& sceneObject {*mSceneObjects[mNextAvailableIndex]};
    ++mNextAvailableIndex;
    return sceneObject;
}
