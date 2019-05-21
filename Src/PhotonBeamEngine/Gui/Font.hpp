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
        
        int GetSize() const {
            return mSize;
        }
        
    private:
        std::vector<Glyph> mGlyphs;
        int mSize {0};
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
        Pht::Optional<Pht::Vec3> mTopGradientColorSubtraction;
        Pht::Optional<Pht::Vec3> mMidGradientColorSubtraction;
        TextSpecular mSpecular {TextSpecular::No};
        Pht::Vec2 mSpecularOffset {0.0f, 0.0f};
        Vec4 mSpecularColor {1.0f, 1.0f, 1.0f, 1.0f};
        TextShadow mSecondShadow {TextShadow::No};
        Pht::Vec2 mSecondShadowOffset {0.0f, 0.0f};
        Vec4 mSecondShadowColor {0.0f, 0.0f, 0.0f, 1.0f};
    };
}

#endif
