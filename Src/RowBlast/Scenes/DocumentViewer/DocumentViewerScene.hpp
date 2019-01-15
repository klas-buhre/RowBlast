#ifndef DocumentViewerScene_hpp
#define DocumentViewerScene_hpp

#include <assert.h>

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace RowBlast {
    class DocumentViewerScene {
    public:
        DocumentViewerScene(Pht::IEngine& engine);
        
        void Init();
    
        Pht::SceneObject& GetUiViewsContainer() {
            assert(mUiViewsContainer);
            return *mUiViewsContainer;
        }

        Pht::SceneObject& GetScrollPanelContainer() {
            assert(mScrollPanelContainer);
            return *mScrollPanelContainer;
        }

        Pht::Scene& GetScene() {
            assert(mScene);
            return *mScene;
        }

    private:
        Pht::IEngine& mEngine;
        Pht::Scene* mScene {nullptr};
        Pht::SceneObject* mUiViewsContainer {nullptr};
        Pht::SceneObject* mScrollPanelContainer {nullptr};
    };
}

#endif
