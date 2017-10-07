#include "TextRenderer.hpp"

#define STRINGIFY(A)  #A
#include "../Shaders/Text.vert"
#include "../Shaders/Text.frag"

#include "Font.hpp"
#include "IEngine.hpp"

using namespace Pht;

TextRenderer::TextRenderer(const IVec2& screenSize) :
    mProjection {Mat4::OrthographicProjection(0.0f, screenSize.x, 0.0f, screenSize.y, -1.0f, 1.0f)},
    mTextShader {{}} {
    
    glGenBuffers(1, &mVbo);
    
    mTextShader.Build(TextVertexShader, TextFragmentShader);
    mTextShader.SetProjection(mProjection);
}

void TextRenderer::RenderText(const std::string& text,
                              Vec2 position,
                              const TextProperties& properties) {
    mTextShader.Use();
    auto& uniforms {mTextShader.GetUniforms()};
    auto& attributes {mTextShader.GetAttributes()};
    
    glUniform4fv(uniforms.mTextColor, 1, properties.mColor.Pointer());

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	glEnableVertexAttribArray(attributes.mTextCoords);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glVertexAttribPointer(attributes.mTextCoords, 4, GL_FLOAT, GL_FALSE, 0, 0);

    for (auto c: text) {
        auto& glyph {properties.mFont.GetGlyph(c)};
        
        GLfloat xPos {position.x + glyph.mBearing.x * properties.mScale};
        GLfloat yPos {position.y - (glyph.mSize.y - glyph.mBearing.y) * properties.mScale};
        GLfloat w {glyph.mSize.x * properties.mScale};
        GLfloat h {glyph.mSize.y * properties.mScale};
        
        GLfloat vertices[6][4] = {
            {xPos,     yPos + h, 0.0f, 0.0f},
            {xPos,     yPos,     0.0f, 1.0f},
            {xPos + w, yPos,     1.0f, 1.0f},

            {xPos,     yPos + h, 0.0f, 0.0f},
            {xPos + w, yPos,     1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}
        };

        glBindTexture(GL_TEXTURE_2D, glyph.mTexture);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        position.x += (glyph.mAdvance >> 6) * properties.mScale;
    }

    glDisableVertexAttribArray(attributes.mTextCoords);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
