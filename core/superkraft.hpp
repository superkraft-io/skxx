#pragma once

#include "sk_common.hpp"

BEGIN_SK_NAMESPACE

class Superkraft {
public:
	SK_Global* skg;

	SK_Project_BinaryData binaryData;

	SK_Machine* machine;
	SK_WebView_Initializer wvinit;
	SK_Window_Mngr* wndMngr;
	SK_Module_System* modsys;

	SK_Communication* comm;

   
    
	Superkraft() {
		skg = new SK_Global();
		skg->sk = this;

		machine = new SK_Machine(skg);
		skg->machine = machine;
		machine->init();


		wndMngr = new SK_Window_Mngr(skg);
		comm = new SK_Communication(skg);

		wvinit.skg = skg;

		modsys = new SK_Module_System(skg);
		modsys->bdfs->binaryData = &binaryData;
        
        modsys->proton->app->wndMngr = wndMngr;
        modsys->proton->window->wndMngr = wndMngr;

		wvinit.modsys = modsys;


		skg->pathUtils.init();

		SK_File configFile;
		if (!configFile.loadFromDisk(skg->pathUtils.paths["config"])) {
			throw std::runtime_error("[SK++] No config file found!");
		}
		skg->sk_config = nlohmann::json::parse(std::string(configFile));



		comm->modsys = modsys;
		comm->wndMngr = wndMngr;
	}

	~Superkraft() {
		delete skg;
		delete machine;
		delete wndMngr;
		delete modsys;
		delete comm;
	}
    
    
    /*static inline Superkraft* sk() {
		Superkraft* instance = static_cast<Superkraft*>(SK_Global::GetInstance().sk);
		return instance ? instance : nullptr;
	}*/
};

END_SK_NAMESPACE
 
