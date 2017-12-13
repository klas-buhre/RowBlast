#include "LifeManager.hpp"

#include <algorithm>

// Engine includes.
#include "JsonUtil.hpp"
#include "FileStorage.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto fullNumLives {5};
    constexpr std::chrono::seconds lifeWaitDuration {180};
    const std::string filename {"lives.dat"};
    const std::string stateMember {"state"};
    const std::string numLivesMember {"numLives"};
    const std::string lifeLostTimePointMember {"lifeLostTimePoint"};
}

LifeManager::LifeManager() :
    mNumLives {fullNumLives} {
    
    LoadState();
}

void LifeManager::Update() {
    if (mState != State::CountingDown) {
        if (HasFullNumLives()) {
            return;
        }
        
        mState = State::CountingDown;
    }
    
    auto now {std::chrono::steady_clock::now()};
    
    if (now > mLifeLostTimePoint + lifeWaitDuration) {
        auto waitedDuration {
            std::chrono::duration_cast<std::chrono::seconds>(now - mLifeLostTimePoint)
        };
        
        auto numNewLives {static_cast<int>(waitedDuration.count() / lifeWaitDuration.count())};
        
        for (auto i {0}; i < std::min(numNewLives, fullNumLives); ++i) {
            IncreaseNumLives();
        }
        
        if (HasFullNumLives()) {
            mState = State::Idle;
        } else {
            StartCountDown(now - (waitedDuration - numNewLives * lifeWaitDuration));
        }
        
        SaveState();
    }
}

void LifeManager::StartLevel() {
    if (mState == State::Idle) {
        mLifeLostTimePoint = std::chrono::steady_clock::now();
        mState = State::StartedPlayingWithFullLives;
    }
    
    if (mNumLives > 0) {
        --mNumLives;
    }
    
    SaveState();
}

void LifeManager::CompleteLevel() {
    IncreaseNumLives();
    
    if (mState == State::StartedPlayingWithFullLives) {
        mState = State::Idle;
    }
    
    SaveState();
}

void LifeManager::FailLevel() {
    if (mState == State::StartedPlayingWithFullLives) {
        StartCountDown(std::chrono::steady_clock::now());
    }
    
    SaveState();
}

void LifeManager::RefillLives() {
    mNumLives = fullNumLives;
    mState = State::Idle;
    SaveState();
}

bool LifeManager::HasFullNumLives() const {
    return mNumLives >= fullNumLives;
}

std::chrono::seconds LifeManager::GetDurationUntilNewLife() const {
    if (HasFullNumLives()) {
        return std::chrono::seconds {0};
    }
    
    auto duration {mLifeLostTimePoint + lifeWaitDuration - std::chrono::steady_clock::now()};
    return std::chrono::duration_cast<std::chrono::seconds>(duration);
}

void LifeManager::IncreaseNumLives() {
    if (mNumLives < fullNumLives) {
        ++mNumLives;
    }
}

void LifeManager::StartCountDown(std::chrono::steady_clock::time_point lifeLostTimePoint) {
    mLifeLostTimePoint = lifeLostTimePoint;
    mState = State::CountingDown;
}

void LifeManager::SaveState() {
    rapidjson::Document document;
    auto& allocator = document.GetAllocator();
    document.SetObject();
    
    Pht::Json::AddInt(document, stateMember, static_cast<int>(mState), allocator);
    Pht::Json::AddInt(document, numLivesMember, mNumLives, allocator);
    
    auto lifeLostTimePointInSeconds {
        std::chrono::duration_cast<std::chrono::seconds>(mLifeLostTimePoint.time_since_epoch()).
        count()
    };
    
    Pht::Json::AddUInt64(document, lifeLostTimePointMember, lifeLostTimePointInSeconds, allocator);

    std::string jsonString;
    Pht::Json::EncodeDocument(document, jsonString);
    Pht::FileStorage::Save(filename, jsonString);
}

bool LifeManager::LoadState() {
    std::string jsonString;
    
    if (!Pht::FileStorage::Load(filename, jsonString)) {
        return false;
    }
    
    rapidjson::Document document;
    Pht::Json::ParseDocument(document, jsonString);
    
    mState = static_cast<State>(Pht::Json::ReadInt(document, stateMember));
    mNumLives = Pht::Json::ReadInt(document, numLivesMember);
    auto lifeLostTimePointInSeconds {Pht::Json::ReadUInt64(document, lifeLostTimePointMember)};
    mLifeLostTimePoint = std::chrono::steady_clock::time_point {
        std::chrono::seconds(lifeLostTimePointInSeconds)
    };
    
    return true;
}
