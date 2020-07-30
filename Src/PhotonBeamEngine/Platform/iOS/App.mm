#import <Foundation/Foundation.h>

namespace {
    auto isDbChecked = false;
    auto isFirstLaunch = false;
}

namespace Pht {
    namespace App {
        bool IsFirstLaunch() {
            if (isDbChecked) {
                return isFirstLaunch;
            }
            
            isDbChecked = true;
            
            if ([[NSUserDefaults standardUserDefaults] boolForKey:@"rowBlastLaunchedBefore"]) {
                isFirstLaunch = false;
            } else {
                [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"rowBlastLaunchedBefore"];
                isFirstLaunch = true;
            }
            
            return isFirstLaunch;
        }
    }
}
