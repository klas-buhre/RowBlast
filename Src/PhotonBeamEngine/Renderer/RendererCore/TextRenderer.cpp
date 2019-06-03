#include "TextRenderer.hpp"

#define STRINGIFY(A)  #A
#include "../Shaders/Text.vert"
#include "../Shaders/Text.frag"
#include "../Shaders/TextDoubleGradient.vert"
#include "../Shaders/TextDoubleGradient.frag"
#include "../Shaders/TextMidGradient.vert"
#include "../Shaders/TextMidGradient.frag"

#include "Font.hpp"
#include "IEngine.hpp"
#include "RenderStateManager.hpp"

using namespace Pht;

namespace {
    void DisableVertexAttributes(const ShaderProgram& shaderProgram) {
        auto& attributes = shaderProgram.GetAttributes();
    
        glDisableVertexAttribArray(attributes.mPosition);
        glDisableVertexAttribArray(attributes.mNormal);
        glDisableVertexAttribArray(attributes.mTextureCoord);
        glDisableVertexAttribArray(attributes.mColor);
        glDisableVertexAttribArray(attributes.mPointSize);
    }
}

TextRenderer::TextRenderer(RenderStateManager& renderState, const IVec2& screenSize) :
    mRenderState {renderState},
    mProjection {Mat4::OrthographicProjection(0.0f, screenSize.x, 0.0f, screenSize.y, -1.0f, 1.0f)},
    mTextShader {{}},
    mTextDoubleGradientShader {{}},
    mTextMidGradientShader {{}} {
    
    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    
    BuildShader(mTextShader, TextVertexShader, TextFragmentShader);
    BuildShader(mTextDoubleGradientShader, TextDoubleGradientVertexShader, TextDoubleGradientFragmentShader);
    BuildShader(mTextMidGradientShader, TextMidGradientVertexShader, TextMidGradientFragmentShader);
}

TextRenderer::~TextRenderer() {
    glDeleteBuffers(1, &mVbo);
}

void TextRenderer::BuildShader(ShaderProgram& shader,
                               const char* vertexShaderSource,
                               const char* fragmentShaderSource) {
    shader.Build(vertexShaderSource, fragmentShaderSource);
    shader.Use();
    shader.SetProjection(mProjection);
}

void TextRenderer::RenderText(const std::string& text,
                              Vec2 position,
                              float slant,
                              const TextProperties& properties) {
    auto& shaderProgram = GetShaderProgram(properties);
    auto& uniforms = shaderProgram.GetUniforms();
    auto& attributes = shaderProgram.GetAttributes();
    
    if (!mRenderState.IsShaderInUse(shaderProgram)) {
        mRenderState.UseShader(shaderProgram);
        mRenderState.SetDepthTest(false);
        mRenderState.SetBlend(true);
        mRenderState.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        DisableVertexAttributes(shaderProgram);
        
        glEnableVertexAttribArray(attributes.mTextCoords);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glVertexAttribPointer(attributes.mTextCoords, 4, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    glUniform4fv(uniforms.mTextColor, 1, properties.mColor.Pointer());
    
    if (properties.mTopGradientColorSubtraction.HasValue()) {
        glUniform3fv(uniforms.mTextTopColorSubtraction,
                     1,
                     properties.mTopGradientColorSubtraction.GetValue().Pointer());
    }

    if (properties.mMidGradientColorSubtraction.HasValue()) {
        glUniform3fv(uniforms.mTextMidColorSubtraction,
                     1,
                     properties.mMidGradientColorSubtraction.GetValue().Pointer());
    }
    
    switch (properties.mAlignment) {
        case TextAlignment::Left:
            break;
        case TextAlignment::CenterX:
            position = AdjustPositionCenterXAlignment(text, position, slant, properties);
            break;
    }

    for (auto c: text) {
        auto& glyph = properties.mFont.GetGlyph(c);
        
        GLfloat xPos {position.x + glyph.mBearing.x * properties.mScale};
        GLfloat yPos {position.y - (glyph.mSize.y - glyph.mBearing.y) * properties.mScale};
        GLfloat w {glyph.mSize.x * properties.mScale};
        GLfloat h {glyph.mSize.y * properties.mScale};
        
        GLfloat vertices[6][4] {
            {xPos + slant,     yPos + h, 0.0f, 0.0f},
            {xPos,             yPos,     0.0f, 1.0f},
            {xPos + w,         yPos,     1.0f, 1.0f},

            {xPos + slant,     yPos + h, 0.0f, 0.0f},
            {xPos + w,         yPos,     1.0f, 1.0f},
            {xPos + w + slant, yPos + h, 1.0f, 0.0f}
        };

        glBindTexture(GL_TEXTURE_2D, glyph.mTexture);
        
        // Should use glBufferSubData here but it leads to very poor performance for some reason.
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        position.x += (glyph.mAdvance >> 6) * properties.mScale;
    }
}

ShaderProgram& TextRenderer::GetShaderProgram(const TextProperties& textProperties) {
    if (textProperties.mTopGradientColorSubtraction.HasValue()) {
        return mTextDoubleGradientShader;
    }

    if (textProperties.mMidGradientColorSubtraction.HasValue()) {
        return mTextMidGradientShader;
    }

    return mTextShader;
}

Vec2 TextRenderer::AdjustPositionCenterXAlignment(const std::string& text,
                                                  Vec2 position,
                                                  float slant,
                                                  const TextProperties& properties) {
    if (text.empty()) {
        return position;
    }
    
    auto textWidth = CalculateTextWidth(text, slant, properties);
    auto& firstGlyph = properties.mFont.GetGlyph(text.front());
    position.x = position.x - firstGlyph.mBearing.x * properties.mScale - textWidth / 2.0f;
    return position;
}

float TextRenderer::CalculateTextWidth(const std::string& text,
                                       float slant,
                                       const TextProperties& properties) {
    if (text.empty()) {
        return 0.0f;
    }
    
    auto x = 0.0f;
    auto textStartX = x;
    for (auto i = 0; i < text.size(); ++i) {
        auto& glyph = properties.mFont.GetGlyph(text[i]);
        if (i == 0) {
            textStartX = glyph.mBearing.x;
        }
        
        if (i == text.size() - 1) {
            x += glyph.mBearing.x + glyph.mSize.x + slant;
            break;
        }
        
        x += glyph.mAdvance >> 6;
    }
    
    return (x - textStartX) * properties.mScale;
}
