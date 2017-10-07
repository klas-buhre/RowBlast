#ifndef Material_hpp
#define Material_hpp

#include <string>
#include <OpenGLES/ES3/gl.h>

namespace Pht {
    struct Color {
        float mRed {0.0f};
        float mGreen {0.0f};
        float mBlue {0.0f};
        
        Color operator+(const Color& other) const {
            return Color {mRed + other.mRed, mGreen + other.mGreen, mBlue + other.mBlue};
        }
    };
    
    struct CubeMapTextures {
        std::string mPositiveX;
        std::string mNegativeX;
        std::string mPositiveY;
        std::string mNegativeY;
        std::string mPositiveZ;
        std::string mNegativeZ;
    };
    
    enum class ShaderType {
        PixelLighting,
        VertexLighting,
        TexturedLighting,
        Textured,
        CubeMap,
        VertexColor,
        Particle
    };
    
    enum class GenerateMipmap {
        Yes,
        No
    };
    
    enum class Blend {
        Yes,
        No
    };
    
    class IImage;

    class Material {
    public:
        Material();
        explicit Material(const std::string& textureName);
        Material(const IImage& image, GenerateMipmap generateMipmap);
        Material(const std::string& textureName,
                 float ambient,
                 float diffuse,
                 float specular,
                 float shininess);
        Material(const CubeMapTextures& cubeMapTextures,
                 const Color& ambient,
                 const Color& diffuse, 
                 const Color& specular, 
                 float shininess,
                 float reflectivity);
        Material(const Color& ambient, 
                 const Color& diffuse, 
                 const Color& specular,
                 float shininess);
        explicit Material(const Color& color);
        
        Blend GetBlend() const;

        const Color& GetAmbient() const {
            return mAmbient;
        }
        
        void SetAmbient(const Color& ambient) {
            mAmbient = ambient;
        }

        const Color& GetDiffuse() const {
            return mDiffuse;
        }
        
        void SetDiffuse(const Color& diffuse) {
            mDiffuse = diffuse;
        }

        const Color& GetSpecular() const {
            return mSpecular;
        }
        
        void SetSpecular(const Color& specular) {
            mSpecular = specular;
        }

        void SetBlend(Blend blend) {
            mBlend = blend;
        }
        
        float GetShininess() const {
            return mShininess;
        }
        
        float GetReflectivity() const {
            return mReflectivity;
        }
        
        float GetOpacity() const {
            return mOpacity;
        }
        
        void SetOpacity(float opacity) {
            mOpacity = opacity;
        }
        
        ShaderType GetShaderType() const {
            return mShaderType;
        }
        
        void SetShaderType(ShaderType shaderType) {
            mShaderType = shaderType;
        }
        
        GLuint GetTexture() const {
            return mTexture;
        }

    private:
        void InitTexture(const IImage& image, GenerateMipmap generateMipmap);
        void InitCubeTexures(const CubeMapTextures& filenames);
        
        Color mAmbient;
        Color mDiffuse;
        Color mSpecular;
        float mShininess {0.0f};
        float mReflectivity {0.0f};
        float mOpacity {1.0f};
        GLuint mTexture {0};
        ShaderType mShaderType {ShaderType::VertexLighting};
        Blend mBlend {Blend::No};
    };
}

#endif
