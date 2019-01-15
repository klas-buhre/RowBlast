#ifndef ScrollPanel_hpp
#define ScrollPanel_hpp

#include <memory>

#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class ScrollPanel {
    public:
        ScrollPanel();
        virtual ~ScrollPanel() {}
        
        void SetPosition(const Vec3& position);
        void AddSceneObject(SceneObject& sceneObject);
        
        void SetSize(const Vec2& size) {
            mSize = size;
        }
        
        const Vec2& GetSize() const {
            return mSize;
        }
        
        Pht::SceneObject& GetRoot() {
            return *mRoot;
        }
        
        Pht::SceneObject& GetPanel() {
            return *mPanel;
        }
        
    private:
        Vec2 mSize {0.0f, 0.0f};
        std::unique_ptr<Pht::SceneObject> mRoot;
        std::unique_ptr<Pht::SceneObject> mPanel;
    };
}

#endif
