#ifndef TextComponent_hpp
#define TextComponent_hpp

#include "ISceneObjectComponent.hpp"
#include "Font.hpp"
#include "Fnv1Hash.hpp"

namespace Pht {
    class TextComponent: public ISceneObjectComponent {
    public:
        static constexpr ComponentId id {Hash::Fnv1a("TextComponent")};
        
        TextComponent(const std::string& text, const TextProperties& properties);
        
        void Update() override {}
        
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
