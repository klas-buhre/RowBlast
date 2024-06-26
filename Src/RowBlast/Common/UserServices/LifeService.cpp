#include "LifeService.hpp"

#include <algorithm>

// Engine includes.
#include "JsonUtil.hpp"
#include "FileStorage.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fullNumLives = 5;
    constexpr std::chrono::seconds lifeWaitDuration {300};
    const std::string filename {"lives.dat"};
    const std::string stateMember {"state"};
    const std::string numLivesMember {"numLives"};
    const std::string lifeLostTimePointMember {"lifeLostTimePoint"};
}

LifeService::LifeService() :
    mNumLives {fullNumLives} {
    
    LoadState();
}

void LifeService::Update() {
    if (mState != State::CountingDown) {
        if (HasFullNumLives()) {
            return;
        }
        
        mState = State::CountingDown;
    }
    
    auto now = std::chrono::system_clock::now();
    if (now > mLifeLostTimePoint + lifeWaitDuration) {
        auto waitedDuration =
            std::chrono::duration_cast<std::chrono::seconds>(now - mLifeLostTimePoint);
        
        auto numNewLives = static_cast<int>(waitedDuration.count() / lifeWaitDuration.count());
        
        for (auto i = 0; i < std::min(numNewLives, fullNumLives); ++i) {
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

void LifeService::StartLevel() {
    if (mState == State::Idle) {
        mLifeLostTimePoint = std::chrono::system_clock::now();
        mState = State::StartedPlayingWithFullLives;
    }
    
    if (mNumLives > 0) {
        --mNumLives;
    }
    
    SaveState();
}

void LifeService::CompleteLevel() {
    IncreaseNumLives();
    
    if (mState == State::StartedPlayingWithFullLives) {
        mState = State::Idle;
    }
    
    SaveState();
}

void LifeService::FailLevel() {
    if (mState == State::StartedPlayingWithFullLives) {
        StartCountDown(std::chrono::system_clock::now());
    }
    
    SaveState();
}

void LifeService::RefillLives() {
    mNumLives = fullNumLives;
    mState = State::Idle;
    SaveState();
}

bool LifeService::HasFullNumLives() const {
    return mNumLives >= fullNumLives;
}

//
// Cannot prevent time cheats since the the system_clock is affected by the user changing
// the time in the OS. One solution that was tested was to use the steady_clock, which is not
// affected by changing the time. However, the steady_clock resets at each reboot on iOS. Probably
// need to get the time from a server for reliably protecting against time cheats. For more info,
// see: https://aaltodoc.aalto.fi/bitstream/123456789/32484/1/master_Teittinen_Oskari_2018.pdf
//
std::chrono::seconds LifeService::GetDurationUntilNewLife() const {
    if (HasFullNumLives() ||
        std::chrono::system_clock::now() > mLifeLostTimePoint + lifeWaitDuration) {

        return std::chrono::seconds {0};
    }
    
    auto duration = mLifeLostTimePoint + lifeWaitDuration - std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(duration);
}

void LifeService::IncreaseNumLives() {
    if (mNumLives < fullNumLives) {
        ++mNumLives;
    }
}

void LifeService::StartCountDown(std::chrono::system_clock::time_point lifeLostTimePoint) {
    mLifeLostTimePoint = lifeLostTimePoint;
    mState = State::CountingDown;
}

void LifeService::SaveState() {
    rapidjson::Document document;
    auto& allocator = document.GetAllocator();
    document.SetObject();
    
    Pht::Json::AddInt(document, stateMember, static_cast<int>(mState), allocator);
    Pht::Json::AddInt(document, numLivesMember, mNumLives, allocator);
    
    auto lifeLostTimePointInSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(mLifeLostTimePoint.time_since_epoch()).
        count();
    
    Pht::Json::AddUInt64(document, lifeLostTimePointMember, lifeLostTimePointInSeconds, allocator);

    std::string jsonString;
    Pht::Json::EncodeDocument(document, jsonString);
    Pht::FileStorage::Save(filename, jsonString);
}

bool LifeService::LoadState() {
    std::string jsonString;
    if (!Pht::FileStorage::Load(filename, jsonString)) {
        return false;
    }
    
    rapidjson::Document document;
    Pht::Json::ParseDocument(document, jsonString);
    
    mState = static_cast<State>(Pht::Json::ReadInt(document, stateMember));
    mNumLives = Pht::Json::ReadInt(document, numLivesMember);
    auto lifeLostTimePointInSeconds {Pht::Json::ReadUInt64(document, lifeLostTimePointMember)};
    mLifeLostTimePoint = std::chrono::system_clock::time_point {
        std::chrono::seconds(lifeLostTimePointInSeconds)
    };

    return true;
}
