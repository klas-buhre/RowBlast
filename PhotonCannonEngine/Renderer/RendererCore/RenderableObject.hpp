#ifndef RenderableObject_hpp
#define RenderableObject_hpp

#include <OpenGLES/ES3/gl.h>

#include "Material.hpp"

namespace Pht {
    class VertexBuffer;
    
    enum class RenderMode {
        Points,
        Triangles
    };
    
    class RenderableObject {
    public:
        RenderableObject(const Material& material, const VertexBuffer& vertexBuffer);
        RenderableObject(const Material& material, RenderMode renderMode);
        ~RenderableObject();
        
        void UploadTriangles(const VertexBuffer& vertexBuffer);
        void UploadPoints(const VertexBuffer& vertexBuffer);
        
        RenderMode GetRenderMode() const {
            return mRenderMode;
        }
        
        GLuint GetVertexBufferId() const {
            return mVertexBufferId;
        }
        
        GLuint GetIndexBufferId() const {
            return mIndexBufferId;
        }
        
        int GetIndexCount() const {
            return mIndexCount;
        }

        const Material& GetMaterial() const {
            return mMaterial;
        }
        
        Material& GetMaterial() {
            return mMaterial;
        }
        
        int GetPointCount() const {
            return mPointCount;
        }
        
    private:
        RenderMode mRenderMode {RenderMode::Triangles};
        Material mMaterial;
        GLuint mVertexBufferId {0};
        GLuint mIndexBufferId {0};
        int mIndexCount {0};
        int mPointCount {0};
    };
}

#endif
