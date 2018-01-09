#ifndef QuadMesh_hpp
#define QuadMesh_hpp

#include "IMesh.hpp"

namespace Pht {
    class QuadMesh: public IMesh {
    public:
        struct Vertex {
            Vec3 mVertex;
            Vec4 mColor;
        };
        
        struct Vertices {
            Vertex mV0;
            Vertex mV1;
            Vertex mV2;
            Vertex mV3;
        };
        
        QuadMesh(float xSize, float ySize);
        QuadMesh(float xSize, float ySize, const Vec2& textureCount);
        QuadMesh(const Vertices& vertices);
        
        Optional<std::string> GetName() const override;
        VertexBuffer GetVertices(VertexFlags flags) const override;
        
    private:
        float mXSize {0.0f};
        float mYSize {0.0f};
        Vertices mVertices;
        Vec2 mTextureCount;
    };
}

#endif
