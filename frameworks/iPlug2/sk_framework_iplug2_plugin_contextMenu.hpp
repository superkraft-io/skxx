#pragma once

#include "../../core/sk_common.hpp"

#if __has_include("IPlugWebUI_SK.h")
    #include "IPlugWebUI_SK.h"
#else
    #include "SK_DAW_Plugin_Project.h"
#endif

#include "IPlugPlatform.h"

#if defined(SK_APP_TYPE_vst)
    #include "IPlugVST3.h"

    #include "istringresult.h"
    #include "ipersistent.h"
    #include "funknown.h"
    #include "vsttypes.h"
    #include "ivstcontextmenu.h"
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
