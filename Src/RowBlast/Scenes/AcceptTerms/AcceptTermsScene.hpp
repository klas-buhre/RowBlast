#ifndef AcceptTermsScene_hpp
#define AcceptTermsScene_hpp

#include <assert.h>

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class AcceptTermsScene {
    public:
        AcceptTermsScene(Pht::IEngine& engine);
        
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
