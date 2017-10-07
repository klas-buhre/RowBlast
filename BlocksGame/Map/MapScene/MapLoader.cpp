#include "MapLoader.hpp"

// Engine includes.
#include "JsonUtil.hpp"

using namespace BlocksGame;

void MapLoader::Load(const std::string& filename, std::vector<MapLevel>& levels) {
    rapidjson::Document document;
    Pht::Json::ParseFile(document, filename);

    assert(document.HasMember("levels"));
    const auto& levelArray {document["levels"]};
    
    for (const auto& levelObject: levelArray.GetArray()) {
        auto levelIndex {Pht::Json::ReadInt(levelObject, "level")};
        auto position {Pht::Json::ReadVec3(levelObject, "position")};
        
        MapLevel mapLevel {levelIndex, position};
        levels.push_back(mapLevel);
    }
}
