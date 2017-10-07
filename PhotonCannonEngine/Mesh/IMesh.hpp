#ifndef IMesh_h
#define IMesh_h

#include "VertexBuffer.hpp"

namespace Pht {
    class IMesh {
    public:
        virtual ~IMesh() {}
        
        virtual VertexBuffer GetVertices(VertexFlags flags) const = 0;
    };
}

#endif
