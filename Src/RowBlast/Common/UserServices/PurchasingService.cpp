#include "PurchasingService.hpp"

using namespace RowBlast;

PurchasingService::PurchasingService()
    : mBalance {0} {}

void PurchasingService::SaveState() {

}

bool PurchasingService::LoadState() {
    return true;
}
