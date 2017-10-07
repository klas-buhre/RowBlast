#ifndef ProgressManager_hpp
#define ProgressManager_hpp

#include <vector>
#include <array>

namespace BlocksGame {
    struct StarLimits {
        int mTwo {0};
        int mThree {0};
    };
    
    class ProgressManager {
    public:
        ProgressManager();
        
        static int CalculateNumStars(int movesUsed, const StarLimits& starLimits);
        
        void StartLevel(int levelIndex);
        void CompleteLevel(int levelIndex, int numStars);
        int GetNumStars(int levelIndex);
        int GetProgress();
        
        int GetCurrentLevel() const {
            return mCurrentLevel;
        }
        
    private:
        void SaveState();
        bool LoadState();

        int mCurrentLevel {1};
        std::vector<int> mNumStars;
    };
}

#endif
