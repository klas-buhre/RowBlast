#ifndef TextComponent_hpp
#define TextComponent_hpp

#include "ISceneObjectComponent.hpp"
#include "Font.hpp"

namespace Pht {
    class TextComponent: public ISceneObjectComponent {
    public:
        static const ComponentId id;
        
        TextComponent(const std::string& text, const TextProperties& properties);
        
        const std::string& GetText() const {
            return mText;
        }
        
        const TextProperties& GetProperties() const {
            return mProperties;
        }
        
    private:
        std::string mText;
        TextProperties mProperties;
    };
}

#endif
