#ifndef GLES3TextRenderer_hpp
#define GLES3TextRenderer_hpp

#include <string>
#include <OpenGLES/ES3/gl.h>

#include "Matrix.hpp"
#include "GLES3ShaderProgram.hpp"

namespace Pht {
    class Font;
    class TextProperties;
    class GLES3RenderStateManager;
    
    class GLES3TextRenderer {
    public:
        GLES3TextRenderer(GLES3RenderStateManager& renderState, const IVec2& screenSize);
        ~GLES3TextRenderer();
        
        void RenderText(const std::string& text,
                        Vec2 position,
                        float slant,
                        const TextProperties& properties);
        
    private:
        void BuildShader(GLES3ShaderProgram& shader,
                         const char* vertexShaderSource,
                         const char* fragmentShaderSource);
        GLES3ShaderProgram& GetShaderProgram(const TextProperties& properties);
        Vec2 AdjustPositionCenterXAlignment(const std::string& text,
                                            Vec2 position,
                                            float slant,
                                            const TextProperties& properties);
        float CalculateTextWidth(const std::string& text,
                                 float slant,
                                 const TextProperties& properties);
        
        GLES3RenderStateManager& mRenderState;
        Mat4 mProjection;
        GLuint mVbo {0};
        GLES3ShaderProgram mTextShader;
        GLES3ShaderProgram mTextDoubleGradientShader;
        GLES3ShaderProgram mTextMidGradientShader;
    };
}

#endif
