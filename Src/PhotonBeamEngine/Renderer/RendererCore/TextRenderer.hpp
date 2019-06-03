#ifndef TextRenderer_hpp
#define TextRenderer_hpp

#include <string>
#include <OpenGLES/ES3/gl.h>

#include "Matrix.hpp"
#include "ShaderProgram.hpp"

namespace Pht {
    class Font;
    class TextProperties;
    class RenderStateManager;
    
    class TextRenderer {
    public:
        TextRenderer(RenderStateManager& renderState, const IVec2& screenSize);
        ~TextRenderer();
        
        void RenderText(const std::string& text,
                        Vec2 position,
                        float slant,
                        const TextProperties& properties);
        
    private:
        void BuildShader(ShaderProgram& shader,
                         const char* vertexShaderSource,
                         const char* fragmentShaderSource);
        ShaderProgram& GetShaderProgram(const TextProperties& properties);
        Vec2 AdjustPositionCenterXAlignment(const std::string& text,
                                            Vec2 position,
                                            float slant,
                                            const TextProperties& properties);
        float CalculateTextWidth(const std::string& text,
                                 float slant,
                                 const TextProperties& properties);
        
        RenderStateManager& mRenderState;
        Mat4 mProjection;
        GLuint mVbo {0};
        ShaderProgram mTextShader;
        ShaderProgram mTextDoubleGradientShader;
        ShaderProgram mTextMidGradientShader;
    };
}

#endif
