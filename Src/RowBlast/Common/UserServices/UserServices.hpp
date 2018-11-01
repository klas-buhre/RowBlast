#ifndef UserServices_hpp
#define UserServices_hpp

#include "LifeService.hpp"
#include "ProgressService.hpp"

namespace RowBlast {
    class UserServices {
    public:
        void Update();
        void StartLevel(int levelId);
        void CompleteLevel(int levelId, int numStars);
        
        const LifeService& GetLifeService() const {
            return mLifeService;
        }

        LifeService& GetLifeService() {
            return mLifeService;
        }

        const ProgressService& GetProgressService() const {
            return mProgressService;
        }

        ProgressService& GetProgressService() {
            return mProgressService;
        }
        
    private:
        LifeService mLifeService;
        ProgressService mProgressService;
    };
}

#endif
