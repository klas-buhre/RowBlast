#ifndef SceneObject_hpp
#define SceneObject_hpp

#include <vector>

#include "Matrix.hpp"
#include "RenderableObject.hpp"
#include "ISceneObjectComponent.hpp"

namespace Pht {
    class SceneObject {
    public:
        using Name = uint32_t;
        
        SceneObject();
        explicit SceneObject(Name name);
        explicit SceneObject(std::shared_ptr<RenderableObject> renderable);
        
        void SetRenderable(std::shared_ptr<RenderableObject> renderable);
        void Translate(const Vec3& translation);
        void RotateX(float degrees);
        void RotateY(float degrees);
        void RotateZ(float degrees);
        void Scale(float scale);
        void ResetTransform();
        void AddChild(SceneObject& child);
        SceneObject* Find(Name name);
        
        template<typename T>
        void SetComponent(std::unique_ptr<T> component) {
            static_assert(std::is_base_of<ISceneObjectComponent, T>::value, "T must be a component");
            for (auto& entry: mComponents) {
                if (entry.first == T::id) {
                    entry.second = std::move(component);
                    return;
                }
            }
            mComponents.emplace_back(T::id, std::move(component));
        }
        
        template<typename T>
        T* GetComponent() {
            static_assert(std::is_base_of<ISceneObjectComponent, T>::value, "T must be a component");
            for (const auto& entry: mComponents) {
                if (entry.first == T::id) {
                    return static_cast<T*>(entry.second.get());
                }
            }
            return nullptr;
        }

        template<typename T>
        const T* GetComponent() const {
            static_assert(std::is_base_of<ISceneObjectComponent, T>::value, "T must be a component");
            for (const auto& entry: mComponents) {
                if (entry.first == T::id) {
                    return static_cast<const T*>(entry.second.get());
                }
            }
            return nullptr;
        }

        void SetName(Name name) {
            mName = name;
        }
        
        const RenderableObject* GetRenderable() const {
            return mRenderable.get();
        }
        
        RenderableObject* GetRenderable() {
            return mRenderable.get();
        }
        
        const Vec3& GetPosition() const {
            return mPosition;
        }
        
        const Mat4& GetTransform() const {
            return mTransform;
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
        
        const std::vector<SceneObject*>& GetChildren() const {
            return mChildren;
        }
        
    private:
        Vec3 mPosition {0.0f, 0.0f, 0.0f};
        Mat4 mTransform;
        bool mIsVisible {true};
        bool mIsInFront {false};
        std::shared_ptr<RenderableObject> mRenderable;
        std::vector<SceneObject*> mChildren;
        std::vector<std::pair<ComponentId, std::unique_ptr<ISceneObjectComponent>>> mComponents;
        Name mName {0};
    };
}

#endif
