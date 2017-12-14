#ifndef Texture_hpp
#define Texture_hpp

#include <string>
#include <memory>

#include <OpenGLES/ES3/gl.h>

namespace Pht {
    enum class GenerateMipmap {
        Yes,
        No
    };
    
    class Texture {
    public:
        Texture(bool hasPremultipliedAlpha);
        ~Texture();
        
        GLuint GetHandle() const {
            return mHandle;
        }

        bool HasPremultipliedAlpha() const {
            return mHasPremultipliedAlpha;
        }
        
    private:
        GLuint mHandle {0};
        bool mHasPremultipliedAlpha {false};
    };
    
    struct EnvMapTextureFilenames {
        bool operator==(const EnvMapTextureFilenames& other) const;
        
        std::string mPositiveX;
        std::string mNegativeX;
        std::string mPositiveY;
        std::string mNegativeY;
        std::string mPositiveZ;
        std::string mNegativeZ;
    };
    
    class IImage;
    
    namespace TextureCache {
        std::shared_ptr<Texture> GetTexture(const std::string& textureName,
                                            GenerateMipmap generateMipmap);
        std::shared_ptr<Texture> GetTexture(const EnvMapTextureFilenames& filenames);
        std::shared_ptr<Texture> InitTexture(const IImage& image, GenerateMipmap generateMipmap);
    }
}

#endif
