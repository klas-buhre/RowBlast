#ifndef LifeService_hpp
#define LifeService_hpp

#include <chrono>

namespace RowBlast {
    class LifeService {
    public:
        LifeService();
        
        void Update();
        void StartLevel();
        void CompleteLevel();
        void FailLevel();
        void RefillLives();
        bool HasFullNumLives() const;
        std::chrono::seconds GetDurationUntilNewLife() const;
        
        int GetNumLives() const {
            return mNumLives;
        }
        
    private:
        void IncreaseNumLives();
        void StartCountDown(std::chrono::steady_clock::time_point lifeLostTimePoint);
        void SaveState();
        bool LoadState();
        
        // Warning! Values in this enum are saved in a file. Do not change the values. 
        enum class State {
            Idle = 0,
            StartedPlayingWithFullLives = 1,
            CountingDown = 2
        };
        
        State mState {State::Idle};
        int mNumLives;
        std::chrono::steady_clock::time_point mLifeLostTimePoint;
    };
}

#endif
