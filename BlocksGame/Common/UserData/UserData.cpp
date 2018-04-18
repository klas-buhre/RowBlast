#include "UserData.hpp"

using namespace RowBlast;

void UserData::Update() {
    mLifeManager.Update();
}

void UserData::StartLevel(int levelIndex) {
    mProgressManager.StartLevel(levelIndex);
    mLifeManager.StartLevel();
}

void UserData::CompleteLevel(int levelIndex, int numStars) {
    mProgressManager.CompleteLevel(levelIndex, numStars);
    mLifeManager.CompleteLevel();
}
