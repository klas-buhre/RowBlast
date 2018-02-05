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
        void SetActiveView(int viewIndex);
        
    private:
        std::vector<Pht::GuiView*> mViews;
    };
}

#endif
