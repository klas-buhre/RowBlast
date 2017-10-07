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
        
        void RenderText(const std::string& text, Vec2 position, const TextProperties& properties);
        
    private:
        Mat4 mProjection;
        GLuint mVbo {0};
        ShaderProgram mTextShader;
    };
}

#endif
