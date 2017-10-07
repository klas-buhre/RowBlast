#ifndef ParametricSurface_hpp
#define ParametricSurface_hpp

#include "IMesh.hpp"
#include "Vector.hpp"

namespace Pht {
    class ParametricSurface: public IMesh {
    public:
        VertexBuffer GetVertices(VertexFlags flags) const override;
        
    protected:
        ParametricSurface(const IVec2& divisions, const Vec2& upperBound, const Vec2& textureCount);
        
        virtual Vec3 Evaluate(const Vec2& domain) const = 0;
        
    private:
        Vec2 ComputeDomain(float i, float j) const;
        void GenerateTriangleIndices(VertexBuffer& vertexBuffer) const;
        
        IVec2 mDivisions;
        Vec2 mUpperBound;
        IVec2 mSlices;
        Vec2 mTextureCount;
    };
}

#endif
