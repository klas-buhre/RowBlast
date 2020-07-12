#include "PurchaseEvent.hpp"

using namespace Pht;

PurchaseEvent::PurchaseEvent(Kind kind) :
    mKind {kind} {}

ProductsResponseEvent::ProductsResponseEvent(const std::vector<Product>& products) :
    PurchaseEvent {PurchaseEvent::ProductsResponse},
    mProducts {products} {}

PurchaseErrorEvent::PurchaseErrorEvent(PurchaseError errorCode) :
    PurchaseEvent {PurchaseEvent::Error},
    mErrorCode {errorCode} {}
