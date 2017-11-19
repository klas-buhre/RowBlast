#ifndef CylinderMesh_hpp
#define CylinderMesh_hpp

#include "ParametricSurface.hpp"

namespace Pht {
    struct CylinderTextureCounts {
        Vec2 mBody;
        Vec2 mDiscs;
    };
    
    class CylinderMesh: public IMesh {
    public:
        CylinderMesh(float radius, float height, const Optional<std::string>& name = {});
        CylinderMesh(float radius,
                     float height,
                     const CylinderTextureCounts& textureCounts,
                     const Optional<std::string>& name = {});
        
        Optional<std::string> GetName() const override;
        VertexBuffer GetVertices(VertexFlags flags) const override;
        
    private:
        enum class DiscType {
            Upper,
            Lower
        };
        
        void GenerateBody(VertexBuffer& vertexBuffer) const;
        void GenerateDisc(VertexBuffer& vertexBuffer, DiscType discType) const;
        
        Optional<std::string> mName;
        float mRadius;
        float mHeight;
        CylinderTextureCounts mTextureCounts;
    };
}

#endif
