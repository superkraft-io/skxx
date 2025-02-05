#pragma once

#include "../sk_common.hpp"

BEGIN_SK_NAMESPACE


using SK_Profiler_Event_onDestroy = std::function<void(long long idx)>;

static inline long long SK_Profiler_nowTime() {
	// Get the current time point
	auto now = std::chrono::system_clock::now();

	// Convert to milliseconds since epoch
	auto duration = now.time_since_epoch();

	long long res = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	return res;
}


class SK_Profiler_Event {
public:
	long long startMS = 0;
	long long endMS = 0;
	long long idx = 0;
	SK_String id = "";

	SK_String initiator = "";

	long long linkedToEvent = 0;

	SK_Profiler_Event_onDestroy onDestroy;

	~SK_Profiler_Event() {
		onDestroy(idx);
	}

	void start() {
		startMS = SK_Profiler_nowTime();
	};

	void stop() {
		endMS = SK_Profiler_nowTime();
	};

	nlohmann::json serialize() {
		return {
			{"start", startMS},
			{"end", endMS},
			{"idx", idx},
			{"id", id},
			{"initiator", initiator},
			{"initiator", linkedToEvent},
		};
	}
};

class SK_Profiler {
public:
	static inline long long eventIdx = 0;

	static inline std::vector<SK_Profiler_Event*> events;


	

	static inline SK_Profiler_Event* snap(const SK_String& id, const SK_String& initiator, long long linkedToEvent) {
		eventIdx++;

		SK_Profiler_Event* event = new SK_Profiler_Event();

		event->onDestroy = [](long long idx) {
			SK_Profiler::removeByID(idx, true);
		};

		event->idx = eventIdx;

		event->id = id;
		event->initiator = initiator;
		if (linkedToEvent != NULL) event->linkedToEvent = linkedToEvent;

		event->start();

		events.push_back(event);

		return event;

	}

	static inline void removeByID(long long idx, bool noDelete) {
		auto it = std::remove_if(events.begin(), events.end(), [idx, noDelete](SK_Profiler_Event* event) {
			if (event && event->idx == idx) {
				if (!noDelete) delete event; // Free memory
				return true;
			}
			return false;
		});

		events.erase(it, events.end());
	}

	static inline nlohmann::json serialize() {
		nlohmann::json res;

		for (size_t i = 0; i < events.size(); ++i) {
			if (!events[i]) continue;

			SK_Profiler_Event* event = events[i];
			res[event->idx] = event->serialize();
		}

		return res;
	}
};

END_SK_NAMESPACE
