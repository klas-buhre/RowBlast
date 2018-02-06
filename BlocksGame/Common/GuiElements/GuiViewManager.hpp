#ifndef GuiViewManager_hpp
#define GuiViewManager_hpp

#include <vector>

namespace Pht {
    class GuiView;
    class SceneObject;
}

namespace BlocksGame {
    class GuiViewManager {
    public:
        void Init(Pht::SceneObject& parentObject);
        void AddView(Pht::GuiView& view);
        void ActivateView(int viewIndex);
        void DeactivateAllViews();
        
    private:
        std::vector<Pht::GuiView*> mViews;
    };
}

#endif
