typedef struct event Event;
typedef enum eventType EventType;

#include "player.h"
#include "card.h"

enum eventType {
	PLAY, RESPOND,
	JUDGE,
	ABILITY,
	TAKEN, GET,
	HURT, DEAD,
	WARNING,
	WIN
};

struct event {
	EventType type;
	const char *msg;
	Player *source;
	Player *target;
	const char *objName;
};

Event * Event_init(EventType type, const char *msg, Player *source, Player *target, const char *objName);
// New event

void Event_free(Event *this);

