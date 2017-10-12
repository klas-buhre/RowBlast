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
        virtual ~Texture();
        
        GLuint GetHandle() const {
            return mHandle;
        }
        
    protected:
        GLuint mHandle {0};
    };
    
    struct CubeMapTextures {
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
        std::shared_ptr<Texture> GetTexture(const CubeMapTextures& filenames);
        std::shared_ptr<Texture> InitTexture(const IImage& image, GenerateMipmap generateMipmap);
    }
}

#endif
