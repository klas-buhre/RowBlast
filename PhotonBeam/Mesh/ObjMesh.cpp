#include "ObjMesh.hpp"

#include "ObjMeshLoader.hpp"

using namespace Pht;

ObjMesh::ObjMesh(const std::string& filename, float scale) :
    mFilename {filename},
    mScale {scale} {}

Optional<std::string> ObjMesh::GetName() const {
    return Optional<std::string> {mFilename + std::to_string(mScale)};
}

VertexBuffer ObjMesh::GetVertices(VertexFlags flags) const {
    return ObjMeshLoader::Load(mFilename, flags, mScale);
}
