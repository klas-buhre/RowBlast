#ifndef UserData_hpp
#define UserData_hpp

#include "LifeManager.hpp"
#include "ProgressManager.hpp"

namespace RowBlast {
    class UserData {
    public:
        void Update();
        void StartLevel(int levelId);
        void CompleteLevel(int levelId, int numStars);
        
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
