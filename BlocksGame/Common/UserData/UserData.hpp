#ifndef UserData_hpp
#define UserData_hpp

#include "LifeManager.hpp"
#include "ProgressManager.hpp"

namespace BlocksGame {
    class UserData {
    public:
        void Update();
        void StartLevel(int levelIndex);
        void CompleteLevel(int levelIndex, int numStars);
        
        const LifeManager& GetLifeManager() const {
            return mLifeManager;
        }

        LifeManager& GetLifeManager() {
            return mLifeManager;
        }

        const ProgressManager& GetProgressManager() const {
            return mProgressManager;
        }

        ProgressManager& GetProgressManager() {
            return mProgressManager;
        }
        
    private:
        LifeManager mLifeManager;
        ProgressManager mProgressManager;
    };
}

#endif
