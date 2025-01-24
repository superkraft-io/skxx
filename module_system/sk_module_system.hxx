#pragma once

#include "../core/sk_common.hxx"

BEGIN_SK_NAMESPACE

class SK_Module_System {
public:
	SK_Module_os os;
	SK_Module_application application;
	SK_Module_vfs* vfs;
	SK_Module_bdfs* bdfs;
	SK_Module_fs fs;
	SK_Module_web web;
	SK_Module_viewMngr viewMngr;
	SK_Module_NativeActions nativeActions;
	SK_Module_debugMngr debugMngr;

	SK_Module_System() {
		vfs = new SK_Module_vfs();
		fs.vfs = vfs;

		bdfs = new SK_Module_bdfs();
	}

	~SK_Module_System() {
		delete bdfs;
		delete vfs;
	}

	void performOperation(const SK_String& module, const SK_String& operation, const nlohmann::json& payload, SK_Communication_Response& respondWith) {
		if (module == "os") os.handleOperation(operation, payload, respondWith);
		else if (module == "application") application.handleOperation(operation, payload, respondWith);
		else if (module == "fs") fs.handleOperation(operation, payload, respondWith);
		else if (module == "bdfs") bdfs->handleOperation(operation, payload, respondWith);
		else if (module == "vfs") vfs->handleOperation(operation, payload, respondWith);
		else if (module == "web") web.handleOperation(operation, payload, respondWith);
		else if (module == "viewMngr") viewMngr.handleOperation(operation, payload, respondWith);
		else if (module == "nativeActions") nativeActions.handleOperation(operation, payload, respondWith);
		else if (module == "debugMngr") debugMngr.handleOperation(operation, payload, respondWith);

		else respondWith.error(404, "Module not found");
	};
};

END_SK_NAMESPACE
