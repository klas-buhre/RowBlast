#include "UserServices.hpp"

using namespace RowBlast;

UserServices::UserServices(Pht::IEngine& engine) :
    mPurchasingService {engine} {}

void UserServices::Update() {
    mPurchasingService.Update();
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
