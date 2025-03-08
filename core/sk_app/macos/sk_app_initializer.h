#pragma once

#ifdef __OBJC__

#include "../../sk_common.hpp"

#import <Foundation/Foundation.h>
#import <objc/runtime.h> // For runtime APIs

BEGIN_SK_NAMESPACE

using SK_App_Initializer_AppEvent_CB = std::function<void(nlohmann::json data)>;

class SK_App_Initializer {
public:
    NSObject* observer;
    
    static inline bool isInitialized;
    static inline bool shouldTerminate;
    
    SK_App_Initializer(const nlohmann::json& bypasses = {}) {
        // Set up Objective-C observer
        observer = [[NSObject alloc] init];
        
        // Dynamically add methods to the observer
        
        if (!bypasses.contains("applicationWillFinishLaunching")){
            class_addMethod([observer class], @selector(applicationWillFinishLaunching:), (IMP)applicationWillFinishLaunching, "v@:@");
        }
        
        if (!bypasses.contains("applicationDidFinishLaunching")){
            class_addMethod([observer class], @selector(applicationDidFinishLaunching:), (IMP)applicationDidFinishLaunching, "v@:@");
        }
        
        if (!bypasses.contains("applicationShouldTerminateAfterLastWindowClosed")){
            class_addMethod([observer class], @selector(applicationShouldTerminateAfterLastWindowClosed:), (IMP)applicationShouldTerminateAfterLastWindowClosed, "B@:@");
        }

        // Observe application lifecycle notifications
        [[NSNotificationCenter defaultCenter] addObserver:observer
                                                 selector:@selector(applicationWillFinishLaunching:)
                                                     name:NSApplicationWillFinishLaunchingNotification
                                                   object:nil];

        [[NSNotificationCenter defaultCenter] addObserver:observer
                                                 selector:@selector(applicationDidFinishLaunching:)
                                                     name:NSApplicationDidFinishLaunchingNotification
                                                   object:nil];

        [[NSNotificationCenter defaultCenter] addObserver:observer
                                                 selector:@selector(applicationShouldTerminateAfterLastWindowClosed:)
                                                     name:NSWindowWillCloseNotification // No direct notification, using window close event
                                                   object:nil];
    }

    ~SK_App_Initializer() {
        [[NSNotificationCenter defaultCenter] removeObserver:observer];
    }
    
    
    
    static void emitAppEvent(const SK_String& eventID, const nlohmann::json& data, SK_App_Initializer_AppEvent_CB cb = NULL){
        nlohmann::json payload {
            {"eventID", eventID},
            {"data", data}
        };
        
        SK_IPC_v2* sb_ipc = SK_Global::sb_ipc;
        sb_ipc->request("sk:viewIPC", "sk:sb", "sk:appEvent", payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
            
            if (cb != NULL) cb(responsePacket->data);
        });
    }
    
    // Static C++ functions to handle Objective-C callbacks
    static void applicationWillFinishLaunching(id self, SEL _cmd, NSNotification *notification) {
        if (isInitialized) return;
        isInitialized = true;
        //emitAppEvent("initialized", {});
    }

    static void applicationDidFinishLaunching(id self, SEL _cmd, NSNotification *notification) {
        //emitAppEvent("ready", {});
    }

    static BOOL applicationShouldTerminateAfterLastWindowClosed(id self, SEL _cmd, NSNotification *notification) {
        
        /*
        if (!shouldTerminate){
            emitAppEvent("close",{}, [self, _cmd, notification](nlohmann::json response){
                bool isNull = response["returnValue"] == "<null>";
                if (!isNull){
                    SK_App_Initializer::shouldTerminate = true;
                    applicationShouldTerminateAfterLastWindowClosed(self, _cmd, notification);
                }
            });
        }
        return SK_App_Initializer::shouldTerminate;
         
        */
    }
};

END_SK_NAMESPACE

#endif
