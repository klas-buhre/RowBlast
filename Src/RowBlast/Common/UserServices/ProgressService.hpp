#ifndef ProgressService_hpp
#define ProgressService_hpp

#include <vector>
#include <array>

namespace RowBlast {
    struct StarLimits {
        int mTwo {0};
        int mThree {0};
    };
    
    class ProgressService {
    public:
        ProgressService();
        
        static int CalculateNumStars(int movesUsed, const StarLimits& starLimits);
        
        void StartLevel(int levelId);
        void CompleteLevel(int levelId, int numStars);
        int GetNumStars(int levelId);
        int GetProgress();
        
        int GetCurrentLevel() const {
            return mCurrentLevel;
        }
        
        bool ProgressedAtPreviousGameRound() const {
            return mProgressedAtPreviousGameRound;
        }
        
    private:
        void SaveState();
        bool LoadState();

        int mCurrentLevel {1};
        std::vector<int> mNumStars;
        bool mProgressedAtPreviousGameRound {false};
    };
}

#endif
