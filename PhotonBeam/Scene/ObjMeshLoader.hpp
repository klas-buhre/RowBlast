#ifndef ObjMeshLoader_hpp
#define ObjMeshLoader_hpp

#include "VertexBuffer.hpp"

namespace Pht {
    namespace ObjMeshLoader {
        VertexBuffer Load(const std::string& filename, VertexFlags flags, float scale);
    }
}

#endif
