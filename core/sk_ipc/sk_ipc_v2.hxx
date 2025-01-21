#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

using SK_IPC_v2_forwardCallback = std::function<void(SK_Communication_Packet* packet)>;

class SK_IPC_v2_awaiter {
public:
    SK_String id;
    SK_IPC_v2_forwardCallback cb;
};

class SK_IPC_v2 {
public:
    SK_String sender_id = "sk.sb";
    long long msg_id = 0;


    using SK_IPC_v2_FrontendCallback = std::function<void(nlohmann::json data, SK_Communication_Packet* packet)>;
    using SK_IPC_v2_sendToFrontend_CB = std::function<void(const SK_String& target, const SK_String& data)>;
    using SK_IPC_v2_BackendCallback = std::function<void(const SK_String& target, SK_Communication_Packet* packet)>;

    static inline SK_IPC_v2_sendToFrontend_CB onSendToFrontend;
    SK_IPC_v2_BackendCallback onMessage;
    
    std::unordered_map<std::string, SK_IPC_v2_BackendCallback> awaitList;
    std::unordered_map<std::string, SK_IPC_v2_FrontendCallback> listeners;
    std::unordered_map<std::string, SK_IPC_v2_FrontendCallback> listeners_once;


    std::unordered_map<std::string, SK_IPC_v2_awaiter*> forwardAwaitList;


    /** Returns a standard OK IPC message*/
    static inline const SK_String OK = "{}";

    /** Returns an ERROR IPC object
    * @param error Error code
    * @param message A human readable message
    * @return Returns a stringified JSON object, e.g {error: "failed", message: "This request failed"}*/
    static inline const SK_String Error(SK_String error, SK_String message = ""){
        nlohmann::json json;

        json["error"] = error;
        json["message"] = message;

        return json.dump();
    }

    static nlohmann::json createResponseJSON(SK_Communication_Packet* packet, const SK_String& data) {
        nlohmann::json responseJSON;
        responseJSON["msg_id"] = packet->id;
        responseJSON["type"] = "response";
        responseJSON["sender"] = packet->target;
        responseJSON["target"] = packet->sender;
        responseJSON["event_id"] = packet->info["event_id"];
        responseJSON["data"] = data;
        return responseJSON;
    }

    void handle_IPC_Msg(SK_Communication_Packet* packet) {
        SK_String msg_id = packet->id;
        SK_String type = SK_String(packet->info["type"]);
        SK_String sender = packet->sender;
        SK_String target = packet->target;
        SK_String event_id = SK_String(packet->info["event_id"]);

        
        if (type == "request") {
            handleRequest(msg_id, type, sender, event_id, packet);
        }
        else if (type == "response") {
            handleResponse(msg_id, type, sender, event_id, packet);
        }
        else if (type == "message") {
           handleMessage(packet);
        }
    }

    /** Returns an event type if it exists
    * @param event_id Name of the event
    * @return "always" for a standard event, "once" for a one-time event, "" (empty string) if the event does not exist*/
    SK_String eventExists(const SK_String& event_id){
        SK_IPC_v2_FrontendCallback listener;
        SK_IPC_v2_FrontendCallback listener_once;

        if (!listeners.empty()) listener = listeners[event_id];
        if (!listeners_once.empty()) listener_once = listeners_once[event_id];

        if (listener != NULL) return "always";
        if (listener_once != NULL) return "once";

        int x = 0;

        return "";
    }

    /** Adds an event that is fired when the frontend requests a response with the specified event ID
    * @param event_id Name of the event
    * @param callback Callback with the event data
    * @return A string representing the event message ID*/
    void on(const SK_String& event_id, SK_IPC_v2_FrontendCallback callback) {
        if (eventExists(event_id) != "") return;

        listeners[event_id] = callback;
    }


  
  
    /** Removes an event
    * @param event_id Name of the event*/
    void off(const SK_String& event_id) {
        listeners.erase(event_id);
        listeners_once.erase(event_id);
    }

  
    /** Adds a one-time event that is fired when the frontend requests a response with the specified event ID. A one-time event is automatically removed once it has been fired.
    * @param event_id Name of the event
    * @param callback Callback with the event data*/
    void once(const SK_String& event_id, SK_IPC_v2_FrontendCallback callback) {
        if (eventExists(event_id) != "") return;

        listeners_once[event_id] = callback;
    }


    void handleRequest(SK_String msg_id, SK_String type, SK_String sender, SK_String event_id, SK_Communication_Packet* packet) {
        auto listener = listeners[event_id];
        auto listener_once = listeners_once[event_id];

        bool exists = false;

        if (listener != NULL) {
            exists = true;
            listener(packet->data, packet);
        }
        else {
            if (listener_once != NULL) {
                exists = true;
                listener_once(packet->data, packet);
                off(event_id);
            }
        }

        if (!exists) {
            packet->response()->error(404, "unknown_listener");
        }
    }


    void handleResponse(SK_String msg_id, SK_String type, SK_String sender, SK_String event_id, SK_Communication_Packet* packet) {
        SK_IPC_v2_BackendCallback awaiter = awaitList[msg_id];
        if (awaiter != NULL) awaiter(sender_id, packet);
    }

    void handleMessage(SK_Communication_Packet* packet) {
        if (onMessage != NULL) onMessage(sender_id, packet);
    }

    SK_String sendToFE(const SK_String& sender, const SK_String& target, const SK_String& event_id, nlohmann::json data, SK_String type, SK_IPC_v2_BackendCallback cb) {
        SK_String _type = "request";
        if (type != "") _type = type;


        if (event_id == "") throw "[SK IPC.sendToFE] Event ID cannot be empty";

        msg_id++;

        nlohmann::json req;

        req["msg_id"] = std::to_string(msg_id);
        req["type"] = _type;
        req["sender"] = sender;
        req["target"] = target;
        req["event_id"] = event_id;
        req["data"] = data;


        if (_type == "request") {
            awaitList[req["msg_id"]] = cb;
        }

        onSendToFrontend(sender_id, req.dump());

        return req["msg_id"];
    }


    void sendResponse(SK_Communication_Packet* packet) {
        nlohmann::json req;
        onSendToFrontend(sender_id, packet->asIPCMessage());
    }
  
    /** Makes a request to the frontend and awaits a response (currently indefinitely)
    * @param event_id Name of the event
    * @param data Data to send
    * @param cb Callback of the response*/
    void request(const SK_String& sender, const SK_String& target, SK_String event_id, nlohmann::json data, SK_IPC_v2_BackendCallback cb) {
        sendToFE(sender, target, event_id, data, "request", cb);
    }

    void forwardPacket(SK_Communication_Packet* packet, SK_IPC_v2_BackendCallback cb) {
        nlohmann::json req;

        req["msg_id"] = packet->id;
        req["type"] = packet->info["type"];
        req["sender"] = packet->sender;
        req["target"] = packet->target;
        req["event_id"] = packet->info["event_id"];
        req["data"] = packet->data;


        if (packet->info["type"] == "request") {
            awaitList[req["msg_id"]] = cb;
        }

        onSendToFrontend(sender_id, req.dump());
    }


    /** Sends a response-less message to the frontend. This function does NOT expect or wait for a response.
    * @param data Data to send*/
    void message(nlohmann::json data) {
        sendToFE(sender_id, "", "SK_IPC_v2_Message", data, "message", NULL);
    }

private:

   
};

END_SK_NAMESPACE
