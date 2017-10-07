#include "ObjMesh.hpp"

#include "ObjLoader.hpp"

using namespace Pht;

ObjMesh::ObjMesh(const std::string& filename, float scale) :
    mFilename {filename},
    mScale {scale} {}
        
VertexBuffer ObjMesh::GetVertices(VertexFlags flags) const {
    return ObjLoader::Load(mFilename, flags, mScale);
}
