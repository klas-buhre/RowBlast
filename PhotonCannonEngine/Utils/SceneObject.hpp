#ifndef SceneObject_hpp
#define SceneObject_hpp

#include "Matrix.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class SceneObject {
    public:
        SceneObject(std::shared_ptr<RenderableObject> renderable);
        
        void SetRenderable(std::shared_ptr<RenderableObject> renderable);
        void Translate(const Vec3& translation);
        void RotateX(float degrees);
        void RotateY(float degrees);
        void RotateZ(float degrees);
        void Scale(float scale);
        void ResetMatrix();
        const RenderableObject& GetRenderable() const;
        RenderableObject& GetRenderable();
        
        const Vec3& GetPosition() {
            return mPosition;
        }
        
        const Mat4& GetMatrix() const {
            return mMatrix;
        }
        
        bool IsVisible() const {
            return mIsVisible;
        }
        
        void SetIsVisible(bool isVisible) {
            mIsVisible = isVisible;
        }

        bool IsInFront() const {
            return mIsInFront;
        }
        
        void SetIsInFront(bool isInFront) {
            mIsInFront = isInFront;
        }
        
    private:
        Vec3 mPosition {0.0f, 0.0f, 0.0f};
        Mat4 mMatrix;
        bool mIsVisible {true};
        bool mIsInFront {false};
        std::shared_ptr<RenderableObject> mRenderable;
    };
}

#endif
