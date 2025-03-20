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

    #ifdef __OBJC__
        NSObject* observer;
    #endif
    
    nlohmann::json bypasses;
    bool isInitialized;
    bool shouldTerminate;
    
    using SK_Get_SK_SB_IPC = std::function<SK_IPC_v2*()>;
    SK_Get_SK_SB_IPC get_SK_SB_IPC_CB;

    static SK_App_Initializer& GetInstance() {
        static SK_App_Initializer instance;
        return instance;
    }


    void init() {
        SK_Global* skg = &SK_Global::GetInstance();
        skg->machine = new SK_Machine();
        skg->machine->init();
    }

    void emitAppEvent(const SK_String& eventID, const nlohmann::json& data, SK_App_Initializer_AppEvent_CB cb = NULL) {
        nlohmann::json payload{
            {"eventID", eventID},
            {"data", data}
        };

        SK_IPC_v2* sb_ipc = get_SK_SB_IPC_CB();
        if (sb_ipc == nullptr) return;

        sb_ipc->request("sk:viewIPC", "sk:sb", "sk:appEvent", payload, [cb](const SK_String& _sender, SK_Communication_Packet* responsePacket) {
            if (cb != NULL) cb(responsePacket->data);
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
            SK_App_Initializer(const nlohmann::json& _bypasses = {}) {
                init();

                bypasses = _bypasses;

                // Set up Objective-C observer
                observer = [[NSObject alloc] init];
        
                
                CFRunLoopObserverContext context = {0, nullptr, nullptr, nullptr, nullptr};
                CFRunLoopObserverRef sk_observer = CFRunLoopObserverCreate(
                    kCFAllocatorDefault,
                    kCFRunLoopAllActivities, // Listen to all states
                    true, // Repeats
                    0,
                    runLoopCallback,
                    &context
                );

                CFRunLoopAddObserver(CFRunLoopGetCurrent(), sk_observer, kCFRunLoopCommonModes);
                CFRelease(sk_observer);
                
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
                return false;
            }

            void runLoopCallback(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
                SK_Global::GetInstance().threadPool_processMainThreadTasks();
            }
        #endif
    #endif
private:
    SK_App_Initializer(const SK_App_Initializer&) = delete;
    SK_App_Initializer& operator=(const SK_App_Initializer&) = delete;
};

END_SK_NAMESPACE

