#ifndef GuiView_hpp
#define GuiView_hpp

#include <vector>
#include <memory>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Font.hpp"
#include "SceneObject.hpp"
#include "SceneResources.hpp"

namespace Pht {
    class TextComponent;
    
    class GuiView {
    public:
        virtual ~GuiView();
        
        void SetPosition(const Vec2& position);
        void SetIsActive(bool isActive) {}
        void AddSceneObject(std::unique_ptr<SceneObject> sceneObject);
        void AddText(std::unique_ptr<Text> text);
        TextComponent& CreateText(const Vec3& position,
                                  const std::string& text,
                                  const TextProperties& properties);
        
        void SetSize(const Vec2& size) {
            mSize = size;
        }
        
        const Vec2& GetSize() const {
            return mSize;
        }
        
        const Vec2& GetPosition() const {
            return mPosition;
        }
        
        const Mat4& GetMatrix() const {
            return mMatrix;
        }
        
        const std::vector<std::unique_ptr<SceneObject>>& GetSceneObjects() const {
            return mSceneObjects;
        }
        
        const std::vector<std::unique_ptr<Text>>& GetTexts() const {
            return mTexts;
        }
        
        SceneResources& GetSceneResources() {
            return mSceneResources;
        }
        
        Pht::SceneObject& GetRoot() {
            return mRoot;
        }
        
    private:
        Vec2 mSize {0.0f, 0.0f};
        Vec2 mPosition {0.0f, 0.0f};
        Mat4 mMatrix;
        SceneResources mSceneResources;
        std::vector<std::unique_ptr<SceneObject>> mSceneObjects;
        std::vector<std::unique_ptr<Text>> mTexts;
        Pht::SceneObject mRoot;
    };
}

/*
#include <memory>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "SceneObject.hpp"
#include "SceneResources.hpp"
#include "Font.hpp"

namespace Pht {
    class TextComponent;
    
    class GuiView {
    public:
        GuiView();
        virtual ~GuiView() {}
        
        void SetPosition(const Vec2& position);
        void SetIsActive(bool isActive);
        void AddSceneObject(std::unique_ptr<SceneObject> sceneObject);
        TextComponent& CreateText(const Vec3& position,
                                  const std::string& text,
                                  const TextProperties& properties);
        Vec2 GetPosition() const;
        const Mat4& GetMatrix() const; // TODO: remove
        
        void SetSize(const Vec2& size) {
            mSize = size;
        }
        
        const Vec2& GetSize() const {
            return mSize;
        }
        
        Pht::SceneObject& GetRoot() {
            return *mRoot;
        }
        
        SceneResources& GetSceneResources() {
            return mSceneResources;
        }
        
    private:
        Vec2 mSize {0.0f, 0.0f};
        std::unique_ptr<Pht::SceneObject> mRoot;
        SceneResources mSceneResources;
    };
}
*/
#endif
