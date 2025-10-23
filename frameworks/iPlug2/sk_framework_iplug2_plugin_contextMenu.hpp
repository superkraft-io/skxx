#pragma once

#include "../../core/sk_common.hpp"

#if __has_include("IPlugWebUI_SK.h")
    #include "IPlugWebUI_SK.h"
#else
    #include "SK_DAW_Plugin_Project.h"
#endif

#include "IPlugPlatform.h"

#if defined(SK_APP_TYPE_vst)
    #include "./iPlug2_SK/IPlug/VST3/IPlugVST3.h"

    #include "./iPlug2_SK/Dependencies/IPlug/VST3_SDK/pluginterfaces/base/istringresult.h"
    #include "./iPlug2_SK/Dependencies/IPlug/VST3_SDK/pluginterfaces/base/ipersistent.h"
    #include "./iPlug2_SK/Dependencies/IPlug/VST3_SDK/pluginterfaces/base/funknown.h"
    #include "./iPlug2_SK/Dependencies/IPlug/VST3_SDK/pluginterfaces/vst/vsttypes.h"
    #include "./iPlug2_SK/Dependencies/IPlug/VST3_SDK/pluginterfaces/vst/ivstcontextmenu.h"
#elif defined(SK_APP_TYPE_au)
    #include <AudioUnit/AudioUnit.h>
#endif

using namespace iplug;

class SK_Framework_iPlug2_Plugin_ContextMenu {
public:
    SK::SK_Global* skg;


    SK_Framework_iPlug2_Plugin_ContextMenu(SK::SK_Global* _skg) {
        skg = _skg;

        skg->popupContextMenu = [&](int paramIdx, int x, int y) {
            popup(paramIdx, x, y);
        };
    }

    void popup(int paramIdx, int x, int y){


        #if defined(SK_APP_TYPE_vst)
            if (iplug::IPlugVST3* pVST3 = static_cast<iplug::IPlugVST3*>(skg->getPluginInstance())) {
                Steinberg::FUnknownPtr<Steinberg::Vst::IComponentHandler3>handler(pVST3->GetComponentHandler());

                if (handler == 0) {
                    return;
                }

                // Query the IComponentHandler3 interface
                Steinberg::Vst::ParamID pid = static_cast<Steinberg::Vst::ParamID>(paramIdx);
                Steinberg::Vst::IContextMenu* menu = handler->createContextMenu(pVST3->GetView(), &pid);
                if (menu) {
                    // (Optional) Add custom items:
                    // Steinberg::Vst::IContextMenu::Item item = {0};
                    // UString128("My Item").copyTo(item.name, 128);
                    // item.tag = 1;
                    // menu->addItem(item, myTarget); 
                    menu->popup(static_cast<Steinberg::UCoord>(x), static_cast<Steinberg::UCoord>(y));
                    menu->release();
                }
            }
        
        #elif defined(SK_APP_TYPE_au)
            #ifdef __OBJC__
                //Not available in AU
            #endif
        #endif
    }
};
