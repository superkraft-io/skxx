#pragma once

#include "../sk_common.hpp"

#ifdef __OBJC__
    #import <Foundation/Foundation.h>
    #import <objc/runtime.h> // For runtime APIs
#endif

BEGIN_SK_NAMESPACE

using SK_App_Initializer_AppEvent_CB = std::function<void(nlohmann::json data)>;

class SK_App_Initializer {
public:
    SK_Global* skg;
    
    #ifdef __OBJC__
        NSObject* observer;
        CFRunLoopObserverRef sk_observer;
    #endif
    
    nlohmann::json bypasses;
    bool isInitialized;
    bool shouldTerminate;
    
    using SK_Get_SK_SB_IPC = std::function<SK_IPC_v2*()>;
    SK_Get_SK_SB_IPC get_SK_SB_IPC_CB;


    void init() {
        SK_Colors_Init();
    }

    void emitAppEvent(const SK_String& eventID, const nlohmann::json& data, SK_App_Initializer_AppEvent_CB cb = NULL) {
        nlohmann::json payload{
            {"eventID", eventID},
            {"data", data}
        };

        SK_IPC_v2* sb_ipc = get_SK_SB_IPC_CB();
        if (sb_ipc == nullptr) return;

        int x = 0;

        sb_ipc->request("sk:viewIPC", "sk:sb", "sk:appEvent", payload, false, [this, cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
            if (cb != NULL) {
                cb(responsePacket->data);
            } else {
                skg->deleteCommPacketWithPID(responsePacket->pid);
            }
        });
    }

    #if defined(SK_OS_windows)
        SK_App_Initializer(const nlohmann::json& _bypasses = {}, SK_Get_SK_SB_IPC _Get_SK_SB_IPC_CB = NULL) {
            init();

            bypasses = _bypasses;
            get_SK_SB_IPC_CB = _Get_SK_SB_IPC_CB;
        }
    #elif defined(SK_OS_apple)
        #ifdef __OBJC__
    
    
            SK_App_Initializer(const nlohmann::json& _bypasses = {}, SK_Get_SK_SB_IPC _Get_SK_SB_IPC_CB = NULL) {
                init();

                bypasses = _bypasses;
                get_SK_SB_IPC_CB = _Get_SK_SB_IPC_CB;

                
                // Set up Objective-C observer
                observer = [[NSObject alloc] init];
        
                #if defined(SK_APP_TYPE_app)
                    //⚠️ ☢️ This causes crash in Studio One when used in plugins
                
                    CFRunLoopObserverContext context = {0, static_cast<void*>(skg), nullptr, nullptr, nullptr};
                    sk_observer = CFRunLoopObserverCreate(
                        kCFAllocatorDefault,
                        kCFRunLoopAllActivities, // Listen to all states
                        true, // Repeats
                        0,
                        runLoopCallback,
                        &context
                    );

                    CFRunLoopAddObserver(CFRunLoopGetCurrent(), sk_observer, kCFRunLoopCommonModes);
                #endif
                
                
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
                // 1. Stop all callbacks
                if (observer) {
                    [[NSNotificationCenter defaultCenter] removeObserver:observer];
                    observer = nil;
                }

                // 2. Clean up CoreFoundation resources
                if (sk_observer) {
                    CFRunLoopRemoveObserver(CFRunLoopGetCurrent(), sk_observer, kCFRunLoopCommonModes);
                    CFRelease(sk_observer);
                    sk_observer = nullptr;
                }

                // 3. Clear other members
                bypasses.clear();
                get_SK_SB_IPC_CB = nullptr;
                skg = nullptr;
            }
    
    
    
   
    
            // Static C++ functions to handle Objective-C callbacks
            static void applicationWillFinishLaunching(id self, SEL _cmd, NSNotification *notification) {
                SK_App_Initializer* skai = (__bridge SK_App_Initializer*)self;
                
                if (skai->isInitialized) return;
                skai->isInitialized = true;
                //emitAppEvent("initialized", {});
            }

            static void applicationDidFinishLaunching(id self, SEL _cmd, NSNotification *notification) {
                SK_App_Initializer* skai = (__bridge SK_App_Initializer*)self;
                //emitAppEvent("ready", {});
            }

            static bool applicationShouldTerminateAfterLastWindowClosed(id self, SEL _cmd, NSNotification *notification) {
                SK_App_Initializer* skai = (__bridge SK_App_Initializer*)self;
        
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
                return false;
            }

            static void runLoopCallback(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
                SK_Global* skg = static_cast<SK_Global*>(info);
                
                if (!skg) return;
                
                skg->threadPool_processMainThreadTasks();
            }
        #endif
    #endif
};

END_SK_NAMESPACE

