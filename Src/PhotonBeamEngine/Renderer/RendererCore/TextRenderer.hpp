#ifndef TextRenderer_hpp
#define TextRenderer_hpp

#include <string>
#include <OpenGLES/ES3/gl.h>

#include "Matrix.hpp"
#include "ShaderProgram.hpp"

namespace Pht {
    class Font;
    class TextProperties;
    
    class TextRenderer {
    public:
        TextRenderer(const IVec2& screenSize);
        ~TextRenderer();
        
        void RenderText(const std::string& text,
                        Vec2 position,
                        float slant,
                        const TextProperties& properties);
        
    private:
        ShaderProgram& GetShaderProgram(const TextProperties& properties);
        
        Mat4 mProjection;
        GLuint mVbo {0};
        ShaderProgram mTextShader;
        ShaderProgram mTextBottomGradientShader;
        ShaderProgram mTextMidGradientShader;
    };
}

#endif
