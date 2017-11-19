#ifndef TextComponent_hpp
#define TextComponent_hpp

#include "ISceneObjectComponent.hpp"
#include "Font.hpp"

namespace Pht {
    class TextComponent: public ISceneObjectComponent {
    public:
        static const ComponentId id;
        
        TextComponent(SceneObject& sceneObject,
                      const std::string& text,
                      const TextProperties& properties);
        
        SceneObject& GetSceneObject() override;
        
        const std::string& GetText() const {
            return mText;
        }
        
        const TextProperties& GetProperties() const {
            return mProperties;
        }
        
    private:
        SceneObject& mSceneObject;
        std::string mText;
        TextProperties mProperties;
    };
}

#endif
