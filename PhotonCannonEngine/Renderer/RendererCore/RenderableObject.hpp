#ifndef RenderableObject_hpp
#define RenderableObject_hpp

#include "Material.hpp"

namespace Pht {
    class Vbo;
    class VertexBuffer;
    class IMesh;
    class VertexFlags;
    
    enum class RenderMode {
        Points,
        Triangles
    };
    
    enum class BufferUsage {
        StaticDraw,
        DynamicDraw
    };
    
    class RenderableObject {
    public:
        RenderableObject(const Material& material, const IMesh& mesh, const VertexFlags& flags);
        RenderableObject(const Material& material, RenderMode renderMode);
        ~RenderableObject();
        
        void UploadTriangles(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage);
        void UploadPoints(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage);
        const Vbo& GetVbo() const;

        RenderMode GetRenderMode() const {
            return mRenderMode;
        }

        const Material& GetMaterial() const {
            return mMaterial;
        }
        
        Material& GetMaterial() {
            return mMaterial;
        }
        
    private:
        void CreateVboAndUploadData(const IMesh& mesh, const VertexFlags& flags);
        
        RenderMode mRenderMode {RenderMode::Triangles};
        Material mMaterial;
        std::shared_ptr<Vbo> mVbo;
    };
}

#endif
