#pragma once

#include "sk_common.hpp"

BEGIN_SK_NAMESPACE

class Superkraft {
public:
	SK_Global* skg;

	SK_Machine* machine;
	SK_WebView_Initializer* wvinit;
	SK_Window_Mngr* wndMngr;
	SK_Module_System* modsys;

	SK_Communication* comm;
    
    SK_Timer* fpsWatcher;

    #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
        SK_SoftBackend_Bundle_Library* bundle_library;
    #endif
    
    bool isReady = false;
    
	Superkraft() {
		skg = new SK_Global();
		skg->sk = this;

        skg->timerMngr = new SK_TimerMngr();
        skg->displaySyncedTimer = skg->timerMngr->add(1000);
        skg->displaySyncedTimer->id = "displaySyncedTimer";
        skg->displaySyncedTimer->start();
        

        fpsWatcher = skg->timerMngr->add(1000);
        fpsWatcher->id = "fpsWatcher";
        fpsWatcher->on([this]() {
            SK_DisplayUtils::tick();
        });
        fpsWatcher->start();

        SK_DisplayUtils::beginMonitoringHighestFPS(
            [this](double oldHz, double newHz) {
                double interval = 1000 / newHz;
                skg->displaySyncedTimer->setInterval(interval);
            },
            true,
            true,
            std::chrono::seconds(1)
        );
        
        #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
            bundle_library = new SK_SoftBackend_Bundle_Library();
            skg->bundle_library = bundle_library;

            #if defined(SK_BUNDLE_MODE_SHALLOW)
                SK_Timer* bundleFlushTimer = skg->timerMngr->add(5000);
                bundleFlushTimer->on([this]() {
                    if (bundle_library) {
                        bundle_library->flushCache();
                    }
                });
                bundleFlushTimer->start();
            #endif
        #endif
        
		machine = new SK_Machine(skg);
		skg->machine = machine;
		machine->init();


		wndMngr = new SK_Window_Mngr(skg);
		comm = new SK_Communication(skg);

        wvinit = new SK_WebView_Initializer();
		wvinit->skg = skg;

		modsys = new SK_Module_System(skg);
        
        modsys->proton->app->wndMngr = wndMngr;
        modsys->proton->window->wndMngr = wndMngr;

		wvinit->modsys = modsys;


		skg->pathUtils.init();

		SK_File configFile;
        
        #if defined(SK_ROUTE_FS_TO_BDFS)
            SK_SoftBackend_Bundle_Entry_Info* configEntry = bundle_library->findByPath("/config.json");
            SK_String configAsStr = configEntry->dataAs_SKString();
            configFile.data = configAsStr.data;
        #else
            if (!configFile.loadFromDisk(skg->pathUtils.paths["config"])) {
                throw std::runtime_error("[SK++] No config file found!");
            }
        #endif
        
		skg->sk_config = nlohmann::json::parse(configFile.data);

		comm->modsys = modsys;
		comm->wndMngr = wndMngr;

        skg->getNativeAction = [this](const SK_String& actionName) -> void* {
            return modsys->nativeActions->appActions->actions[actionName];
        };
	}

	~Superkraft() {
        SK_DisplayUtils::endMonitoringHighestFPS();

		delete machine;
        skg->machine = nullptr;
        
        delete wvinit;
        wvinit = nullptr;
        
		delete wndMngr;
        wndMngr = nullptr;
       
		delete modsys;
        modsys = nullptr;
       
		delete comm;
        comm = nullptr;
        
        delete skg;
        skg = nullptr;
        
        #if defined(SK_BUNDLE_MODE_DEEP) || defined(SK_BUNDLE_MODE_SHALLOW)
            delete bundle_library;
            bundle_library = nullptr;
        #endif
	}
};

END_SK_NAMESPACE
 
