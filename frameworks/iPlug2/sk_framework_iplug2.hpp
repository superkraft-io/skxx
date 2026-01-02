#pragma once

#include "../../skxx/core/sk_common.hpp"
#include "../../skxx/core/superkraft.hpp"
#include "sk_framework_iplug2_plugin_contextMenu.hpp"


#include "../../iPlug2_SK/Examples/IPlugWebUI_SK/IPlugWebUI_SK.h"


#include "IPlugPlatform.h"
#include "IPlugParameter.h"

#if defined(SK_OS_windows)
    #include "WebView2.h"
#endif


using namespace iplug;

class SK_Framework_iPlug2_Base {
public:
    std::unique_ptr<iplug::Timer> uiTimer_;
    SK_Global* skg;

    bool initialized = false;
    bool onSoftBackend_isReady_executed = false;
    
    IPlugAPIBase* instance;

    SK_Framework_iPlug2_Plugin_ContextMenu* plugCtxMenu;

    std::vector<float> paramValues;
    

    SK_Window* parameterListener = NULL;
    
    SK_Framework_iPlug2_Base(SK_Global* _skg) {
        skg = _skg;
        plugCtxMenu = new SK_Framework_iPlug2_Plugin_ContextMenu(skg);
    }

    void init(IPlugAPIBase* _instance = nullptr){
        if (initialized) return;

        if (_instance) {
            instance = _instance;
        }
        
        skg->sk_config["host"] = getHostName();

        skg->getPluginInstance = [&]() {
            return instance;
        };
        
        skg->findPluginParamByName = [&](const std::string& paramName) -> iplug::IParam* {
            for (int i = 0; i < instance->NParams(); i++) {
                IParam* param = instance->GetParam(i);
                if (SK_String(param->GetName()) == paramName) {
                    return param;
                }
            }
            
            return nullptr;
        };
        
        skg->findPluginParamIdxByName = [&](const std::string& paramName) {
            IParam*  targetParam = skg->findPluginParamByName(paramName);
            
            for (int i = 0; i < instance->NParams(); i++) {
               if (instance->GetParam(i) == targetParam){
                   return i;
               }
            }
            
            return -1; // Not found
        };
        
        SK_Machine* skm = static_cast<SK_Machine*>(skg->machine);
        skm->cpuInfo = skm->getCPUInformation();

        skg->sendMsgToWebview = [&](const SK_String& target, const SK_String& data) {
            SK_String str = "sk_api.ipc.handleIncoming(" + data + ")";

            if (target == "sk:sb") {
                instance->EvaluateJavaScript(str.c_str());
            }
            else {
                #if defined(SK_OS_windows)
                    str = data;
                #endif

                SK_Window* view = static_cast<Superkraft*>(skg->sk)->wndMngr->findWindowByTag(target);
                if (view) view->webview.sendMsgAsJSON(str, NULL);
            }
        };

        
        
        skg->handlePluginParamEvent = [&](const nlohmann::json& payload, SK_Communication_Response& respondWith){
            SK_String pluginParamID = payload["dawPluginParamID"];
            iplug::IParam* param = skg->findPluginParamByName(pluginParamID);
            int paramIdx = skg->findPluginParamIdxByName(pluginParamID);

            if (param == NULL) {
                respondWith.error(404, "invalid_plugin_param_id");
                return;
            }

            SK_String event = payload["event"];

            if (event == "contextmenu") {
                if (skg->runningAs == "app") {
                    respondWith.error(404, "standalone_runtime");
                    return;
                }

                int left = payload["left"];
                int top = payload["top"];

                plugCtxMenu->popup(paramIdx, left, top);

            }
            else if (event == "read") {
                respondWith.JSON(nlohmann::json{
                    {"value", param->Value()}
                });
                
                return;
            }
            else if (event == "write") {
                float value = payload["value"];
                double normalizedValue = param->ToNormalized(value);

                instance->SendParameterValueFromUI(paramIdx, normalizedValue);
                //instance->SetParameterValue(paramIdx, normalizedValue); //this is for VST3

                //The two lines below are for AU if the above line doesn't work for AU
                //param->SetNormalized(normalizedValue);
                //instance->EndInformHostOfParamChangeFromUI(paramIdx);
            }
            else if (event == "mousedown"){
                instance->BeginInformHostOfParamChangeFromUI(paramIdx);
            }
            else if (event == "mouseup"){
                instance->EndInformHostOfParamChangeFromUI(paramIdx);
            }
                                             
            respondWith.JSON_OK();
        };
        
        
        
        
        
        
        
        skg->getMainWindowSize = [&]() {
            SK_Point size{instance->GetEditorWidth(), instance->GetEditorHeight()};
            return size;
        };

        skg->setMainWindowSize = [&](int w, int h) {
            instance->SetEditorSize(w, h);
            instance->OnParentWindowResize(w, h);


            skg->resizeAllMainWindowViews(0, 0, w, h, 1);

            #if defined(SK_OS_windows)
                float scale = skg->mainWindow->getHWNDScale(skg->mainWindow->wndHandle);
                SetWindowPos(skg->mainWindow->wndHandle, NULL, 0, 0, w * scale, h * scale, SWP_NOMOVE | SWP_NOZORDER);
                SendMessage(skg->mainWindow->wndHandle, WM_SIZE, SIZE_RESTORED, MAKELPARAM(w * scale, h * scale));
            #endif
        };


        skg->onMainWindowHWNDAcquired = [&](void* handle, bool isView = false) {
           
            static_cast<Superkraft*>(skg->sk)->wndMngr->newWindow([&](SK_Window* wnd) {
                skg->mainWindow = wnd;
                
                wnd->config.data["width"] = instance->GetEditorWidth();
                wnd->config.data["height"] = instance->GetEditorHeight();

                wnd->tag = "sb";
                wnd->config["visible"] = true;
                  
                #if defined(SK_OS_windows)
                    wnd->wndHandle = static_cast<HWND>(handle);
                    skg->updateWebViewHWNDListForView(wnd->windowClassName);
                #elif defined(SK_OS_apple)
                    #ifdef __OBJC__
                        if (!isView){
                            wnd->wndHandle = (__bridge NSWindow*) handle;
                            NSWindow* _wndHandle = wnd->wndHandle;
                            wnd->contentView = _wndHandle.contentView;
                        } else {
                            wnd->contentView = (__bridge NSView*) handle;
                            wnd->wndHandle = wnd->contentView.window;
                        }
                
                        wnd->finalizeCreation();
                    #endif
                #endif
                  
                skg->sb_ipc = wnd->ipc;
                
                wnd->onDestroyed = [&, wnd](){
                    /*if (wnd->tag == "sb") {
                        skg->sb_ipc = nullptr; //<<<<<<<------ HERE
                    }*/
                };

                static_cast<Superkraft*>(skg->sk)->comm->sb_ipc = wnd->ipc;

                onSoftBackend_isReady();
            });
        };

          
          
          
          

        skg->onWebViewReady = [&](void* wnd, void* webview, bool isHardBackend) {
            //generate ordered list of parameter names
            SK_String pluginParameters = "";
            if (instance->NParams() == 1){
                pluginParameters = "'" + SK_String(instance->GetParam(0)->GetName()) + "'";
            }
            else {
                auto makeParameterObj = [](IParam* param) {
                    SK_String id = SK_String(param->GetName());
                    SK_String value = SK_String(param->Value());

                    std::map<IParam::EParamType, std::string> types {
                        {IParam::EParamType::kTypeNone, "none"},
                        {IParam::EParamType::kTypeBool, "boolean"},
                        {IParam::EParamType::kTypeInt, "integer"},
                        {IParam::EParamType::kTypeEnum, "list"},
                        {IParam::EParamType::kTypeDouble, "double"},
                    };

                    std::string type = types[param->Type()];



                    std::map<IParam::EParamUnit, std::string> units {
                        {IParam::EParamUnit::kUnitPercentage, "percentage"},
                        {IParam::EParamUnit::kUnitSeconds, "seconds"},
                        {IParam::EParamUnit::kUnitMilliseconds, "milliseconds"},
                        {IParam::EParamUnit::kUnitSamples, "samples"},
                        {IParam::EParamUnit::kUnitDB, "decibels"},
                        {IParam::EParamUnit::kUnitLinearGain, "linear_gain"},
                        {IParam::EParamUnit::kUnitPan, "pan"},
                        {IParam::EParamUnit::kUnitPhase, "phase"},
                        {IParam::EParamUnit::kUnitDegrees, "degrees"},
                        {IParam::EParamUnit::kUnitMeters, "meters"},
                        {IParam::EParamUnit::kUnitRate, "rate"},
                        {IParam::EParamUnit::kUnitRatio, "ratio"},
                        {IParam::EParamUnit::kUnitFrequency, "frequency"},
                        {IParam::EParamUnit::kUnitOctaves, "octaves"},
                        {IParam::EParamUnit::kUnitCents, "cents"},
                        {IParam::EParamUnit::kUnitAbsCents, "absolute_cents"},
                        {IParam::EParamUnit::kUnitSemitones, "semitones"},
                        {IParam::EParamUnit::kUnitMIDINote, "midi_note"},
                        {IParam::EParamUnit::kUnitMIDICtrlNum, "midi_ctrl_number"},
                        {IParam::EParamUnit::kUnitBPM, "bpm"},
                        {IParam::EParamUnit::kUnitBeats, "beats"},
                        {IParam::EParamUnit::kUnitCustom, "custom"}
                    };

                    std::string unit = units[param->Unit()];


                    std::map<IParam::EDisplayType, std::string> displayTypes{
                       {IParam::EDisplayType::kDisplayLinear, "linear"},
                       {IParam::EDisplayType::kDisplayLog, "log"},
                       {IParam::EDisplayType::kDisplayExp, "exponential"},
                       {IParam::EDisplayType::kDisplaySquared, "squared"},
                       {IParam::EDisplayType::kDisplaySquareRoot, "squared_root"},
                       {IParam::EDisplayType::kDisplayCubed, "cubed"},
                       {IParam::EDisplayType::kDisplayCubeRoot, "cubed_root"}
                    };

                    std::string displayType = displayTypes[param->DisplayType()];


                    int flags = param->GetFlags();

                    bool cannotAutomate = (flags & IParam::EFlags::kFlagCannotAutomate) != 0;
                    bool stepped = (flags & IParam::EFlags::kFlagStepped) != 0;
                    bool negateDisplay = (flags & IParam::EFlags::kFlagNegateDisplay) != 0;
                    bool signDisplay = (flags & IParam::EFlags::kFlagSignDisplay) != 0; // <- fix scope
                    bool meta = (flags & IParam::EFlags::kFlagMeta) != 0;

                    // "none" means none of the real bits are set
                    constexpr int kAllFlags =
                        IParam::EFlags::kFlagCannotAutomate |
                        IParam::EFlags::kFlagStepped |
                        IParam::EFlags::kFlagNegateDisplay |
                        IParam::EFlags::kFlagSignDisplay |
                        IParam::EFlags::kFlagMeta;

                    bool none = (flags & kAllFlags) == 0;

                    // Build the object string (JSON-like). If you need strict JSON, add quotes around keys.
                    SK_String flagsObject = "null";

                    if (!none){
                        flagsObject = "{cannotAutomate:" + SK_String(cannotAutomate ? "true" : "false") +
                        ",stepped:" + SK_String(stepped ? "true" : "false") +
                        ",negateDisplay:" + SK_String(negateDisplay ? "true" : "false") +
                        ",signDisplay:" + SK_String(signDisplay ? "true" : "false") +
                        ",meta:" + SK_String(meta ? "true" : "false") +
                        "}";
                    }

                    return "{id:'" + id + "',value:" + value + ",type:'" + type + "',unit:'" + unit + "',displayType:'" + displayType + "',flags:" + flagsObject + "}";
                };

                for (int i = 0; i < instance->NParams() - 1; i++) {
                    IParam* param = instance->GetParam(i);
                    pluginParameters += makeParameterObj(param) + ",";
                }

                IParam* lastParam = instance->GetParam(instance->NParams() - 1);
                if (lastParam) pluginParameters += makeParameterObj(lastParam);
            }

            pluginParameters = "[" + pluginParameters + "]";
            
            static_cast<Superkraft*>(skg->sk)->wvinit->pluginParameters = pluginParameters;




            //initialize the webview
            SK_Window* _wnd = static_cast<SK_Window*>(wnd);

            static_cast<Superkraft*>(skg->sk)->wvinit->init(webview, _wnd);

            if (_wnd->config.data.contains("accessPluginParameters") && _wnd->config.data["accessPluginParameters"] == true) {
                for (int i = 0; i < instance->NParams(); i++) {
                    IParam* param = instance->GetParam(i);
                    paramValues.push_back(param->Value());
                }


                parameterListener = _wnd;
            }
        };


        
        
        skg->onPreConfigWnd = [](SK_Window* wnd, nlohmann::json constructorOpts){
           //code here
        };

        skg->onBeforeWndResize = [this](SK_Window* wnd){
            if (wnd->config.data.contains("mainWindow") && wnd->config.data["mainWindow"] == true){
                int w = wnd->config.data["width"];
                int h = wnd->config.data["height"];
                
                #if defined(SK_APP_TYPE_vst3)
                    IPlugVST3* _instance = static_cast<IPlugVST3*>(instance);
                    _instance->GetView()->Resize(w, h);
                #elif defined(SK_APP_TYPE_au2)
                    IPlugAU* plugin = static_cast<IPlugAU*>(instance); // cast directly to your plugin class
                    plugin->Resize(w, h);
                #endif
                
                return SK_Point(-1, -1);
            }
            
            return SK_Point(-2, -2);
        };
        
        #if defined(SK_APP_TYPE_plugin)
            skg->onPostConfigWnd = [&](SK_Window* wnd) {
                for (auto& [key, value] : wnd->config_updateTracker.items()) {
                    value = false;
                }
                
                if (wnd->config.data.contains("mainWindow") && wnd->config.data["mainWindow"] == true) {
                    //Custom host specific styling can happen here
                    /*if (skg->sk_config["host"] == "Studio One"){
                        wnd->config.data["frame"] = false;
                        wnd->config.data["roundness"] = 0.0f;
                    }*/
                }
            };

            skg->onWndCreated = [](SK_Window* wnd) {
                wnd->updateWebView();
            };
        #endif


        skg->tickSK_TimerMngr = [&]() {
            if (skg && !skg->terminating) skg->timerMngr->tick();
        };

        
        uiTimer_ = std::unique_ptr<Timer>(Timer::Create([this](iplug::Timer&) {
            bool isRunningInMainThread = skg->threadPool->thisFunctionRunningInMainThread();
            if (isRunningInMainThread) skg->threadPool_processMainThreadTasks();

            skg->tickSK_TimerMngr();
        }, 1));
        


        skg->displaySyncedTimer->on([this]() {
            #if defined(SK_OS_windows)
                updateParamValues();
            #elif defined(SK_OS_apple)
                if (skg->OBJCPPSafeTicker) skg->OBJCPPSafeTicker();
            #endif
        });
        

        initialized = true;
    }
    


    void updateParamValues() {
        if (parameterListener == NULL) return;
        
        #if defined(SK_OS_windows)
            bool anyParamHasChanged = false;
        
            for (int i = 0; i < instance->NParams(); i++) {
                IParam* param = instance->GetParam(i);
                float value = param->Value();
                if (paramValues[i] != value) {
                    paramValues[i] = value;
                    anyParamHasChanged = true;
                }
            }

            if (!anyParamHasChanged) return;


            UINT bufferSize = static_cast<UINT>(paramValues.size() * sizeof(float));
            parameterListener->webview.sendSharedBuffer(bufferSize, paramValues.data(), { {"id", "dawPluginParamUpdate"} });
        #elif defined(SK_OS_macos)
            #ifdef __OBJC__
                if (!instance) return;
        
                for (int i = 0; i < instance->NParams(); i++) {
                    IParam* param = instance->GetParam(i);
                    float value = param->Value();
                    if (paramValues[i] != value) {
                        paramValues[i] = value;
                        parameterListener->webview.evaluateScript("window.sk_api.dawPluginMngr.updateParameter(" + SK_String(i) + "," + SK_String(value) + ")", NULL);
                    }
                }
            #endif
        #endif
    }

    void onSoftBackend_isReady(){
        if (onSoftBackend_isReady_executed) return;

        SK_IPC* ipc = static_cast<SK_IPC*>(skg->sb_ipc);

        SK_Global* _skg = skg;
        ipc->on("sk_app_is_ready", [&, _skg](nlohmann::json data, SK_Communication_Packet* packet) {
            packet->response()->JSON_OK();

            SK_App_Initializer* appInitializer = static_cast<SK_App_Initializer*>(skg->appInitializer);
            appInitializer->skg = _skg;
            appInitializer->emitAppEvent("ready", {});
        });
        
        
        
        ipc->on("valid_event_id", [](nlohmann::json data, SK_Communication_Packet* packet) {
            nlohmann::json json;

            std::string frontend_message = std::string(data["key"]);

            json["backend_said"] = "hello frontend :)";
            packet->response()->JSON(json);
        });

        ipc->once("valid_event_id_once", [](nlohmann::json data, SK_Communication_Packet* packet) {
            nlohmann::json json;

            SK_String frontend_message = data["key"];

            json["backend_said"] = "hello frontend :) deleting this event now";
            packet->response()->JSON(json);
        });


        ipc->onMessage = [&](const SK_String& sender, SK_Communication_Packet* packet) {
            SK_String action = packet->data["action"];

            if (action == "reqFromBE") {
                nlohmann::json be_data;
                be_data["this_is"] = "a backend request :)";
                
                ipc->request("sk:hb", "sk:sb", "requestFromBackend", be_data, false, [](const SK_String& sender, SK_Communication_Packet* packet) {
                    SK_String key = packet->data["key"];
                    DBGMSG("key = %s\n", key.c_str());
                });
            }
            else if (action == "msgFromBE")
            {
                nlohmann::json be_data;
                be_data["this_is"] = "a message from backend :)";

                ipc->message(be_data);
            }
            else {
                DBGMSG("data = %s\n", packet->data.dump().c_str());
            }
        };
    }
    
    
    SK_String getHostName() {
        int hostID = instance->GetHost();
        
        switch (hostID) {
            case kHostUnknown:           return "Unknown";
            case kHostReaper:            return "Reaper";
            case kHostProTools:          return "Pro Tools";
            case kHostCubase:            return "Cubase";
            case kHostNuendo:            return "Nuendo";
            case kHostSonar:             return "Sonar";
            case kHostVegas:             return "Vegas";
            case kHostFL:                return "FL Studio";
            case kHostSamplitude:        return "Samplitude";
            case kHostAbletonLive:       return "Ableton Live";
            case kHostTracktion:         return "Tracktion";
            case kHostNTracks:           return "NTracks";
            case kHostMelodyneStudio:    return "Melodyne Studio";
            case kHostVSTScanner:        return "VST Scanner";
            case kHostAULab:             return "AU Lab";
            case kHostForte:             return "Forte";
            case kHostChainer:           return "Chainer";
            case kHostAudition:          return "Audition";
            case kHostOrion:             return "Orion";
            case kHostBias:              return "Bias";
            case kHostSAWStudio:         return "SAWStudio";
            case kHostLogic:             return "Logic";
            case kHostGarageBand:        return "GarageBand";
            case kHostDigitalPerformer:  return "Digital Performer";
            case kHostStandalone:        return "Standalone";
            case kHostAudioMulch:        return "AudioMulch";
            case kHostStudioOne:         return "Studio One";
            case kHostVST3TestHost:      return "VST3 Test Host";
            case kHostArdour:            return "Ardour";
            case kHostRenoise:           return "Renoise";
            case kHostOpenMPT:           return "OpenMPT";
            case kHostWaveLab:           return "WaveLab";
            case kHostWaveLabElements:   return "WaveLab Elements";
            case kHostTwistedWave:       return "TwistedWave";
            case kHostBitwig:            return "Bitwig";
            case kHostWWW:               return "WWW (Web Host)";
            case kHostReason:            return "Reason";
            case kHostGoldWave5x:        return "GoldWave 5.x";
            case kHostWaveform:          return "Waveform";
            case kHostAudacity:          return "Audacity";
            case kHostAcoustica:         return "Acoustica";
            case kHostPluginDoctor:      return "Plugin Doctor";
            case kHostiZotopeRX:         return "iZotope RX";
            case kHostSAVIHost:          return "SAVIHost";
            case kHostBlueCat:           return "BlueCat";
            case kHostMixbus32C:         return "Mixbus32C";
            default:                     return "Unknown Host";
        }
    }
};
