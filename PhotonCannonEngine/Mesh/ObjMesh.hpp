#ifndef ObjMesh_hpp
#define ObjMesh_hpp

#include "IMesh.hpp"

namespace Pht {
    class ObjMesh: public IMesh {
    public:
        ObjMesh(const std::string& filename, float scale = 1.0f);
        
        VertexBuffer GetVertices(VertexFlags flags) const override;
        
    private:
        std::string mFilename;
        float mScale {1.0f};
    };
}

#endif
