#include <stdlib.h>

#include "event.h"
#include "player.h"
#include "card.h"

Event * Event_init(EventType type, const char *msg, Player *source, Player *target, const char *objName) {
	Event *new = malloc(sizeof(Event));
	new->type = type;
	new->msg = msg;
	new->source = source;
	new->target = target;
	new->objName = objName;
	return new;
}

void Event_free(Event *this) {
	free(this);
}

