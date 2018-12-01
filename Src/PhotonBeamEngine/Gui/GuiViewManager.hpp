#ifndef GuiViewManager_hpp
#define GuiViewManager_hpp

#include <map>

namespace Pht {
    class GuiView;
    class SceneObject;

    class GuiViewManager {
    public:
        void Init(SceneObject& parentObject);
        void AddView(int viewId, GuiView& view);
        void ActivateView(int viewId);
        void DeactivateAllViews();
        
    private:
        std::map<int, GuiView*> mViews;
    };
}

#endif
