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
    class GuiView {
    public:
        GuiView(bool depthTest);
        virtual ~GuiView();
        
        void SetPosition(const Vec2& position);
        void AddSceneObject(std::unique_ptr<SceneObject> sceneObject);
        void AddText(std::unique_ptr<Text> text);
        
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
        
        bool GetDepthTest() const {
            return mDepthTest;
        }
        
        SceneResources& GetSceneResources() {
            return mSceneResources;
        }
        
    private:
        Vec2 mSize {0.0f, 0.0f};
        Vec2 mPosition {0.0f, 0.0f};
        Mat4 mMatrix;
        bool mDepthTest {true};
        SceneResources mSceneResources;
        std::vector<std::unique_ptr<SceneObject>> mSceneObjects;
        std::vector<std::unique_ptr<Text>> mTexts;
    };
}

#endif
