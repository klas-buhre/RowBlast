#ifndef RenderStateManager_hpp
#define RenderStateManager_hpp

#include <OpenGLES/ES3/gl.h>

#include "Optional.hpp"
#include "Noncopyable.hpp"

namespace Pht {
    class ShaderProgram;
    class Material;
    class Vbo;
    
    class RenderStateManager: public Noncopyable {
    public:
        void Init();
        void SetBlend(bool enabled);
        void SetBlendFunc(GLenum sFactor, GLenum dFactor);
        void SetDepthTest(bool enabled);
        void SetDepthWrite(bool enabled);
        void SetCullFace(bool enabled);
        void SetScissorTest(bool enabled);
        void BindTexture(GLenum textureUnitIndex, GLenum target, GLuint texture);
        void UseShader(ShaderProgram& shaderProgram);
        void OnBeginRenderPass();
        
        void UseMaterial(const Material& material) {
            mMaterial = &material;
        }

        void UseVbo(const Vbo& vbo) {
            mVbo = &vbo;
        }

        bool IsShaderInUse(const ShaderProgram& shaderProgram) const {
            return &shaderProgram == mShaderProgram;
        }
        
        bool IsMaterialInUse(const Material& material) const {
            return &material == mMaterial;
        }

        bool IsVboInUse(const Vbo& vbo) const {
            return &vbo == mVbo;
        }

    private:
        struct TextureUnit {
            GLenum mTarget;
            GLuint mTexture;
        };

        void UpdateGlBlend();
        void UpdateGlBlendFunc();
        void UpdateGlDepthTest();
        void UpdateGlDepthWrite();
        void UpdateGlCullFace();
        void UpdateGlScissorTest();
        Optional<TextureUnit>* GetTextureUnit(GLenum unitIndex);
        void InvalidateShader();

        bool mBlendEnabled {false};
        GLenum mBlendSFactor {GL_ONE};
        GLenum mBlendDFactor {GL_ZERO};
        bool mDepthTestEnabled {false};
        bool mDepthWriteEnabled {true};
        bool mCullFaceEnabled {false};
        bool mScissorTestEnabled {false};
        Optional<TextureUnit> mTextureUnit0;
        Optional<TextureUnit> mTextureUnit1;
        const ShaderProgram* mShaderProgram {nullptr};
        const Material* mMaterial {nullptr};
        const Vbo* mVbo {nullptr};
    };
}

#endif
