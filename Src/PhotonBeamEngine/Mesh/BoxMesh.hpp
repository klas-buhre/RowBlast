#ifndef BoxMesh_hpp
#define BoxMesh_hpp

#include "IMesh.hpp"

namespace Pht {
    struct BoxTextureCounts {
        Vec2 mZSides;
        Vec2 mXSides;
        Vec2 mYSides;
    };
    
    class BoxMesh: public IMesh {
    public:
        BoxMesh(float xSize, float ySize, float zSize);
        BoxMesh(float xSize, float ySize, float zSize, const BoxTextureCounts& textureCounts);
        
        Optional<std::string> GetName() const override;
        std::unique_ptr<VertexBuffer> CreateVertexBuffer(VertexFlags attributeFlags) const override;
        
    private:
        float mXSize;
        float mYSize;
        float mZSize;
        BoxTextureCounts mTextureCounts;
    };
}

#endif
