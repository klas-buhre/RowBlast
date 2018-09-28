#include "ProgressManager.hpp"

#include <assert.h>

// Engine includes.
#include "JsonUtil.hpp"
#include "FileStorage.hpp"

using namespace RowBlast;

namespace {
    const std::string filename {"progress.dat"};
    const std::string currentLevelMember {"currentLevel"};
    const std::string numStarsMember {"numStars"};
}

ProgressManager::ProgressManager() {
    if (!LoadState()) {
        mNumStars = {0};
    }
    // mNumStars = {2, 3, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 3, 2, 2, 1, 2, 2, 1, 3, 2, 2, 1, 3, 1, 3, 2, 1, 3, 1, 2, 3, 2, 1, 3, 2, 1, 3, 2, 2, 3, 1, 1, 2, 3, 3, 2, 3, 3, 1, 1, 2, 3, 3, 2, 3, 0};
    // mNumStars = {2, 3, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 3, 2, 3, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 2, 3, 1, 3, 2, 2, 0};
    // mNumStars = {2, 3, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 3, 2, 3, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 2, 3, 2, 1, 0};
    mNumStars = {2, 3, 1, 3, 2, 0};
    // mNumStars = {0};
}

int ProgressManager::CalculateNumStars(int movesUsed, const StarLimits& starLimits) {
    if (movesUsed <= starLimits.mThree) {
        return 3;
    }
    
    if (movesUsed <= starLimits.mTwo) {
        return 2;
    }
    
    return 1;
}

void ProgressManager::StartLevel(int levelId) {
    mCurrentLevel = levelId;
    mProgressedAtPreviousGameRound = false;
    SaveState();
}

void ProgressManager::CompleteLevel(int levelId, int numStars) {
    assert(levelId > 0);
    assert(numStars > 0 && numStars <= 3);
    
    auto progress {GetProgress()};
    if (levelId == progress) {
        mNumStars[progress - 1] = numStars;
        mNumStars.push_back(0);
        mProgressedAtPreviousGameRound = true;
    } else if (levelId < progress) {
        if (numStars > mNumStars[levelId - 1]) {
            mNumStars[levelId - 1] = numStars;
        }
    } else {
        assert(!"Completed wrong level.");
    }
    
    SaveState();
}

int ProgressManager::GetNumStars(int levelId) {
    assert(levelId > 0);
    
    if (levelId <= GetProgress()) {
        return mNumStars[levelId - 1];
    }
    
    return 0;
}

int ProgressManager::GetProgress() {
    return static_cast<int>(mNumStars.size());
}

void ProgressManager::SaveState() {
    rapidjson::Document document;
    auto& allocator = document.GetAllocator();
    document.SetObject();
    
    Pht::Json::AddInt(document, currentLevelMember, mCurrentLevel, allocator);
    
    rapidjson::Value numStars(rapidjson::kArrayType);
    
    for (auto numStarsForLevel: mNumStars) {
        numStars.PushBack(numStarsForLevel, allocator);
    }
    
    Pht::Json::AddValue(document, numStarsMember, numStars, allocator);
    
    std::string jsonString;
    Pht::Json::EncodeDocument(document, jsonString);
    Pht::FileStorage::Save(filename, jsonString);
}

bool ProgressManager::LoadState() {
    std::string jsonString;
    
    if (!Pht::FileStorage::Load(filename, jsonString)) {
        return false;
    }
    
    rapidjson::Document document;
    Pht::Json::ParseDocument(document, jsonString);
    
    mCurrentLevel = Pht::Json::ReadInt(document, currentLevelMember);
    
    assert(document.HasMember(numStarsMember.c_str()));
    
    const auto& numStarsArray {document[numStarsMember.c_str()]};
    assert(numStarsArray.IsArray());
    
    for (const auto& numStarsForLevel: numStarsArray.GetArray()) {
        assert(numStarsForLevel.IsInt());
        mNumStars.push_back(numStarsForLevel.GetInt());
    }
    
    return true;
}
