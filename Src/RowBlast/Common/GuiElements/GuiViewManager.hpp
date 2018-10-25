#ifndef GuiViewManager_hpp
#define GuiViewManager_hpp

#include <map>

namespace Pht {
    class GuiView;
    class SceneObject;
}

namespace RowBlast {
    class GuiViewManager {
    public:
        void Init(Pht::SceneObject& parentObject);
        void AddView(int viewId, Pht::GuiView& view);
        void ActivateView(int viewId);
        void DeactivateAllViews();
        
    private:
        std::map<int, Pht::GuiView*> mViews;
    };
}

#endif
