#ifndef ScrollPanel_hpp
#define ScrollPanel_hpp

#include <memory>

#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    
    class ScrollPanel {
    public:
        ScrollPanel(IEngine& engine, float dampingCoefficient, float cutoffVelocity);
        virtual ~ScrollPanel() {}
        
        void SetPosition(const Vec3& position);
        void AddSceneObject(SceneObject& sceneObject);
        void OnTouch(const TouchEvent& event);
        void Update();
        
        void SetSize(const Vec2& size) {
            mSize = size;
        }
        
        const Vec2& GetSize() const {
            return mSize;
        }
        
        Pht::SceneObject& GetRoot() {
            return *mRoot;
        }
        
    private:
        void UpdateIsTouchingState(const Pht::TouchEvent& touch);
        void SetYPosition(float yPosition);
        
        IEngine& mEngine;
        Vec2 mSize {0.0f, 0.0f};
        float mDampingCoefficient;
        float mCutoffVelocity;
        std::unique_ptr<Pht::SceneObject> mRoot;
        std::unique_ptr<Pht::SceneObject> mPanel;
        bool mIsTouching {false};
        Vec2 mTouchLocationAtScrollBegin {0.0f, 0.0f};
        float mPanelYPositionAtScrollBegin {0.0f};
        float mPanelYVelocity {0.0f};
    };
}

#endif
