#ifndef ObjLoader_hpp
#define ObjLoader_hpp

#include "VertexBuffer.hpp"

namespace Pht {
    namespace ObjLoader {
        VertexBuffer Load(const std::string& filename, VertexFlags flags, float scale);
    }
}

#endif
