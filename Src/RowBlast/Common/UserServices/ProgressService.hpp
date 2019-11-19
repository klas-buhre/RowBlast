#ifndef ProgressService_hpp
#define ProgressService_hpp

#include <vector>
#include <array>

namespace RowBlast {
    class ProgressService {
    public:
        ProgressService();
        
        void StartLevel(int levelId);
        void CompleteLevel(int levelId, int numStars);
        int GetNumStars(int levelId) const;
        int GetProgress() const;
        
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
