#ifndef DocumentViewerScene_hpp
#define DocumentViewerScene_hpp

#include <assert.h>

namespace Pht {
    class IEngine;
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
        
    private:
        Pht::IEngine& mEngine;
        Pht::SceneObject* mUiViewsContainer {nullptr};
    };
}

#endif
