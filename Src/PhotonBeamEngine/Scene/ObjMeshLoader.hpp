#ifndef ObjMeshLoader_hpp
#define ObjMeshLoader_hpp

#include "VertexBuffer.hpp"

namespace Pht {
    enum class MoveMeshToOrigin {
        Yes,
        No
    };
    
    namespace ObjMeshLoader {
        VertexBuffer Load(const std::string& filename,
                          VertexFlags flags,
                          float scale,
                          MoveMeshToOrigin moveMeshToOrigin);
    }
}

#endif
