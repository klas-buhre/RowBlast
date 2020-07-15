#import <Foundation/Foundation.h>

#import "NetworkConnection.h"

namespace Pht {
    namespace NetworkStatus {
        bool IsConnected() {
            Reachability* reachability = [Reachability reachabilityForInternetConnection];
            enum NetworkStatus networkStatus = [reachability currentReachabilityStatus];
            return networkStatus != NotReachable;
        }
    }
}
