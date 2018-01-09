#include "Font.hpp"

#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "FileSystem.hpp"

using namespace Pht;

namespace {
    const char firstPrintableCharacter {32};
    const char lastPrintableCharacter {127};
}

Font::Font(const std::string& filename, int size) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        assert(false);
    }

    std::string fullPath {FileSystem::GetResourceDirectory() + "/" + filename};
    FT_Face face;
    if (auto errorCode {FT_New_Face(ft, fullPath.c_str(), 0, &face)}) {
        std::cout << "ERROR::FREETYPE: Failed to load font. Error " << errorCode << std::endl;
        assert(false);
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    // Disable byte alignment restriction.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Load the first printable set of the ASCII table.
    for (char c = firstPrintableCharacter; c < lastPrintableCharacter; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_ALPHA,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_ALPHA,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Glyph glyph {
            texture,
            Pht::IVec2 {
                static_cast<int>(face->glyph->bitmap.width),
                static_cast<int>(face->glyph->bitmap.rows)
            },
            Pht::IVec2 {face->glyph->bitmap_left, face->glyph->bitmap_top},
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        
        mGlyphs.push_back(glyph);
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Restore byte-alignment.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

Font::~Font() {
    for (auto& glyph: mGlyphs) {
        glDeleteTextures(1, &glyph.mTexture);
    }
}

const Font::Glyph& Font::GetGlyph(char character) const {
    assert(character >= firstPrintableCharacter && character <= lastPrintableCharacter);
    return mGlyphs[character - firstPrintableCharacter];
}

Text::Text(const Vec2& position, const std::string& text, const TextProperties& properties) :
    mPosition {position},
    mText {text},
    mProperties {properties} {}
