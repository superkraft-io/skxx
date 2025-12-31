#pragma once

#include "../core/sk_common.hpp"

BEGIN_SK_NAMESPACE

class SK_Module_System {
public:
	SK_Global* skg;

	SK_Module_process* process;
	SK_Module_os* os;
	SK_Module_application* application;
	SK_Module_fs* fs;
	SK_Module_vfs* vfs;
	SK_Module_bdfs* bdfs;
	SK_Module_web* web;
	SK_Module_NativeActions* nativeActions;
	SK_Module_debugMngr* debugMngr;

	SK_Module_ProtonJS* proton;

	SK_Module_System(SK_Global* _skg) {
		skg = _skg;


		process = new SK_Module_process(skg);
        os = new SK_Module_os(skg);
		application = new SK_Module_application(skg);

		fs = new SK_Module_fs(skg);
		vfs = new SK_Module_vfs(skg);
        bdfs = new SK_Module_bdfs(skg);

		web = new SK_Module_web(skg);
        nativeActions = new SK_Module_NativeActions(skg);
        debugMngr = new SK_Module_debugMngr(skg);
		proton = new SK_Module_ProtonJS(skg);
        
        skg->forwardPacketToModule = [&](const SK_String& module, const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith){
            performOperation(module, operation, payload, respondWith);
        };
	}

	~SK_Module_System() {
        delete proton;
        delete debugMngr;
        delete nativeActions;
        delete web;
        
        delete bdfs;
        delete vfs;
        delete fs;
        
        delete application;
		delete os;
		delete process;
        
        skg = nullptr;
	}

	void performOperation(const SK_String& module, const SK_String& operation, nlohmann::json& payload, SK_Communication_Response& respondWith) {
		     if (module == "process") process->handleOperation(operation, payload, respondWith);
		else if (module == "os") os->handleOperation(operation, payload, respondWith);
		else if (module == "application") application->handleOperation(operation, payload, respondWith);
		else if (module == "fs") fs->handleOperation(operation, payload, respondWith);
		else if (module == "bdfs") bdfs->handleOperation(operation, payload, respondWith);
		else if (module == "vfs") vfs->handleOperation(operation, payload, respondWith);
		else if (module == "web") web->handleOperation(operation, payload, respondWith);
		else if (module == "nativeActions") nativeActions->handleOperation(operation, payload, respondWith);
		else if (module == "debugMngr") debugMngr->handleOperation(operation, payload, respondWith);
		else if (module == "proton") proton->handleOperation(operation, payload, respondWith);

		else respondWith.error(404, "Module not found");
	};
};

END_SK_NAMESPACE
