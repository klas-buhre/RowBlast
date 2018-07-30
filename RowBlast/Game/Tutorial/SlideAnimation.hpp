#ifndef SlideAnimation_hpp
#define SlideAnimation_hpp

#include <vector>
#include <string>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class RenderableObject;
    class GuiView;
}

namespace RowBlast {
    class SlideAnimation {
    public:
        SlideAnimation(Pht::IEngine& engine,
                       Pht::GuiView& view,
                       const Pht::Vec3& position,
                       float size,
                       float frameTime);
        
        void Init(const std::vector<std::string>& frameFilenames, Pht::Scene& scene);
        void Update();
        
    private:
        void SetFrame(int frameIndex);
        
        Pht::IEngine& mEngine;
        Pht::SceneObject& mSlideSceneObject;
        std::vector<Pht::RenderableObject*> mFrameRenderables;
        float mElapsedTime {0.0f};
        int mFrameIndex {0};
        float mSize {0.0f};
        float mFrameTime {0.0f};
    };
}

#endif
