#ifndef RenderableObject_hpp
#define RenderableObject_hpp

#include "Material.hpp"
#include "Optional.hpp"

namespace Pht {
    class GpuVbo;
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
    
    enum class VertexBufferLocation {
        AtGpuOnly,
        AtGpuAndCpu
    };
    
    class RenderableObject {
    public:
        RenderableObject(const Material& material,
                         const IMesh& mesh,
                         const VertexFlags& attributeFlags,
                         VertexBufferLocation bufferLocation);
        RenderableObject(const Material& material,
                         std::unique_ptr<VertexBuffer> cpuSideBuffer,
                         RenderMode renderMode);
        RenderableObject(const Material& material,
                         const VertexBuffer& fromBuffer,
                         const Optional<std::string>& vboName);
        RenderableObject(const Material& material, std::shared_ptr<GpuVbo> vbo);

        ~RenderableObject();
        
        void UploadTriangles(BufferUsage bufferUsage);
        void UploadPoints(BufferUsage bufferUsage);
        
        const GpuVbo& GetVbo() const {
            return *mVbo;
        }

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
        void UploadMeshVertexData(const IMesh& mesh,
                                  const VertexFlags& attributeFlags,
                                  VertexBufferLocation bufferLocation);
        void UploadTriangles(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage);
        void UploadPoints(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage);
        
        RenderMode mRenderMode {RenderMode::Triangles};
        Material mMaterial;
        std::shared_ptr<GpuVbo> mVbo;
    };
}

#endif
