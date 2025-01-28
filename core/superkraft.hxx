#pragma once

#include "sk_common.hxx"

BEGIN_SK_NAMESPACE

class Superkraft {
public:
	SK_Project_BinaryData binaryData;

	SK_Machine machine;
	SK_WebView_Initializer wvinit;
	SK_Window_Mngr wndMngr;
	SK_Module_System modsys;

	SK_Communication comm;

	Superkraft::Superkraft() {
		SK_Path_Utils::init();
		SK_Colors_Init();

		modsys.bdfs->binaryData = &binaryData;
		modsys.viewMngr.wndMngr = &wndMngr;
		modsys.proton.wndMngr = &wndMngr;

		wvinit.modsys = &modsys;

		SK_File configFile;
		if (!configFile.loadFromDisk(SK_Path_Utils::paths["config"])) {
			throw std::runtime_error("[SK++] No config file found!");
		}
		sk_config = nlohmann::json::parse(std::string(configFile));

		SK_Machine::init();


		comm.modsys = &modsys;
		comm.wndMngr = &wndMngr;
	}

	Superkraft::~Superkraft() {
	}
};

END_SK_NAMESPACE
