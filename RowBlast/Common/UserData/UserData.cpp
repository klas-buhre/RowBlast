#include "UserData.hpp"

using namespace RowBlast;

void UserData::Update() {
    mLifeManager.Update();
}

void UserData::StartLevel(int levelId) {
    mProgressManager.StartLevel(levelId);
    mLifeManager.StartLevel();
}

void UserData::CompleteLevel(int levelId, int numStars) {
    mProgressManager.CompleteLevel(levelId, numStars);
    mLifeManager.CompleteLevel();
}
