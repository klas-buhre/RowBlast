#import "AppDelegate.h"

@implementation AppDelegate

- (void) applicationDidFinishLaunching: (UIApplication*) application {
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    mWindow = [[UIWindow alloc] initWithFrame: screenBounds];
    
    mViewController = [[GLKViewControllerExtension alloc]initWithNibName:nil bundle:nil];
    [mViewController createGLContext];
    [mWindow setRootViewController:mViewController];
    
    mView = [[GLKViewExtension alloc] initWithFrame: screenBounds 
                                      context: [mViewController getContext]];
    mView.userInteractionEnabled = YES;
    
    mViewController.view = mView;
    [mViewController configureView];
    
    [mWindow makeKeyAndVisible];
}

@end
