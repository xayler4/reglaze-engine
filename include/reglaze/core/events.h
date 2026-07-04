#ifndef RGLZ_EVENTS_H
#define RGLZ_EVENTS_H

#include "base.h"

namespace rglz {
	enum class EventType {
		Quit
	};

	struct Event {
		EventType type;	
	};
}

#endif	// RGLZ_EVENTS_H
