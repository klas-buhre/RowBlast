#ifndef GuiView_hpp
#define GuiView_hpp

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
        void AddText(std::unique_ptr<Text> text); // TODO: remove
        
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
        std::vector<std::unique_ptr<Text>> mTexts; // TODO: remove
    };
}

#endif
