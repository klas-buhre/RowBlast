#ifndef IMesh_h
#define IMesh_h

#include "VertexBuffer.hpp"
#include "Optional.hpp"

namespace Pht {
    class IMesh {
    public:
        virtual ~IMesh() {}
        
        virtual Optional<std::string> GetName() const = 0;
        virtual VertexBuffer GetVertices(VertexFlags flags) const = 0;
    };
}

#endif
