#ifndef TriangleMesh_hpp
#define TriangleMesh_hpp

#include "IMesh.hpp"

namespace Pht {
    struct TriangleTextureCounts {
        Vec2 mZSides {1.0f, 1.0f};
        Vec2 mXSide {1.0f, 1.0f};
        Vec2 mYSide {1.0f, 1.0f};
        Vec2 mTiltedSide {1.0f, 1.0f};
    };
    
    class TriangleMesh: public IMesh {
    public:
        TriangleMesh(float size, float depth);
        TriangleMesh(float size, float depth, const Pht::Vec3& offset);
        TriangleMesh(float size, float depth, const TriangleTextureCounts& textureCounts);
        
        Optional<std::string> GetName() const override;
        VertexBuffer GetVertices(VertexFlags flags) const override;
        
    private:
        float mSize;
        float mDepth;
        Pht::Vec3 mOffset {0.0f, 0.0f, 0.0f};
        TriangleTextureCounts mTextureCounts;
    };
}

#endif
