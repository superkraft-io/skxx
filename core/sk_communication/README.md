#SK Communication (SKC)

This component acts a router for both IPC and Web request.

It routes an IPC request or a web request to either an internally reserved endpoints:

- `sk.hb`
- `sk.sb`
- `sk.modsys`
- `sk.modop`
- `sk.project`
- `sk.view`
- `*`

SKC also handles relaying between the views and the soft backend.

Relaying means that for example `myView` wants to send an IPC message to the `soft backend` and receive a response.

SKC also allows you to create event listeners in the `hard backend` by doing something like this:
```cpp
SK_Common::sb_ipc->on("valid_event_id", [](nlohmann::json data, SK_Communication_Packet* packet) {
	nlohmann::json json;

	std::string frontend_message = std::string(data["key"]);

	json["backend_said"] = "hello frontend :)";

	packet->response()->JSON(json);
});
```

This allows you to listen for messages pretty much anywhere in your program and also respond to said messages.

The `soft backend` acts like a hidden view, and **all** views can have their own event listeners.

