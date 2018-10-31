#ifndef Font_hpp
#define Font_hpp

#include <OpenGLES/ES3/gl.h>
#include <vector>

#include "Vector.hpp"
#include "Optional.hpp"

namespace Pht {
    class Font {
    public:
        Font(const std::string& filename, int size);
        ~Font();
        
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
    
    enum class TextSpecular {
        Yes,
        No
    };
    
    enum class SnapToPixel {
        Yes,
        No
    };
    
    struct TextProperties {
        const Font& mFont;
        float mScale {1.0f};
        Vec4 mColor {1.0f, 1.0f, 1.0f, 1.0f};
        TextShadow mShadow {TextShadow::No};
        Pht::Vec2 mShadowOffset {0.0f, 0.0f};
        Vec4 mShadowColor {0.0f, 0.0f, 0.0f, 1.0f};
        SnapToPixel mSnapToPixel {SnapToPixel::Yes};
        float mItalicSlant {0.0f};
        Pht::Optional<Pht::Vec3> mBottomGradientColorSubtraction;
        Pht::Optional<Pht::Vec3> mTopGradientColorSubtraction;
        TextSpecular mSpecular {TextSpecular::No};
        Pht::Vec2 mSpecularOffset {0.0f, 0.0f};
        Vec4 mSpecularColor {1.0f, 1.0f, 1.0f, 1.0f};
    };
}

#endif