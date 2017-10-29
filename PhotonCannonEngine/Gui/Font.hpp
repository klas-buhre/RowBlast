#ifndef Font_hpp
#define Font_hpp

#include <OpenGLES/ES3/gl.h>
#include <vector>

#include "Vector.hpp"

namespace Pht {
    class Font {
    public:
        Font(const std::string& filename, int size);
        
        struct Glyph {
            GLuint mTexture {0};
            Pht::IVec2 mSize;
            Pht::IVec2 mBearing;
            unsigned int mAdvance {0};
        };
        
        const Glyph& GetGlyph(char character) const;
        
    private:
        std::vector<Glyph> mGlyphs;
    };
    
    enum class TextShadow {
        Yes,
        No
    };
    
    struct TextProperties {
        const Font& mFont;
        float mScale {1.0f};
        Vec4 mColor {1.0f, 1.0f, 1.0f, 1.0f};
        TextShadow mShadow {TextShadow::No};
        Pht::Vec2 mOffset {0.0f, 0.0f};
        Vec4 mShadowColor {0.0f, 0.0f, 0.0f, 1.0f};
    };
    
    struct Text {
        Text(const Vec2& position, const std::string& text, const TextProperties& properties);
        
        Vec2 mPosition;
        std::string mText;
        TextProperties mProperties;
        bool mIsVisible {true};
    };
}

#endif
