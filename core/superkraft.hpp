#pragma once

#include "sk_common.hpp"

BEGIN_SK_NAMESPACE

class Superkraft {
public:
	SK_Project_BinaryData binaryData;

	SK_Machine machine;
	SK_WebView_Initializer wvinit;
	SK_Window_Mngr wndMngr;
	SK_Module_System modsys;

	SK_Communication comm;

   
    
	Superkraft() {
		modsys.bdfs->binaryData = &binaryData;
        
        modsys.proton.app.wndMngr = &wndMngr;
        modsys.proton.window.wndMngr = &wndMngr;

		wvinit.modsys = &modsys;

		SK_File configFile;
		if (!configFile.loadFromDisk(SK_Global::GetInstance().pathUtils.paths["config"])) {
			throw std::runtime_error("[SK++] No config file found!");
		}
        SK_Global::GetInstance().sk_config = nlohmann::json::parse(std::string(configFile));

		static_cast<SK_Machine*>(SK_Global::GetInstance().machine)->init();


		comm.modsys = &modsys;
		comm.wndMngr = &wndMngr;
	}

	~Superkraft() {
	}
    
    
    static inline Superkraft* sk(){
		Superkraft* instance = static_cast<Superkraft*>(SK_Global::GetInstance().sk);
		return instance ? instance : nullptr;
	}
};

END_SK_NAMESPACE
 
