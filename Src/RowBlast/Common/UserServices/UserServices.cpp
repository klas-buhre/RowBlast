#include "UserServices.hpp"

using namespace RowBlast;

void UserServices::Update() {
    mLifeService.Update();
}

void UserServices::StartLevel(int levelId) {
    mProgressService.StartLevel(levelId);
    mLifeService.StartLevel();
}

void UserServices::CompleteLevel(int levelId, int numStars) {
    mProgressService.CompleteLevel(levelId, numStars);
    mLifeService.CompleteLevel();
}
