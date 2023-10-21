#include "events.h"

void GAME_EVENTS_Call()
{

}

void EVENTS_Init(struct EVENTS* self)
{
	self->Queue = al_create_event_queue();
}

void EVENTS_Register(struct EVENTS* self, struct WINDOW* window)
{
	ASSERT(window->Display);

	al_register_event_source(self->Queue, al_get_display_event_source(window->Display)); // Okno
	al_install_keyboard();
	al_register_event_source(self->Queue, al_get_keyboard_event_source()); // Klawiatura
}

void EVENTS_Process(struct EVENTS* self, struct WINDOW* window)
{
	switch (self->EventType)
	{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			window->Running = false;
			break;
	}
}

uint32_t EVENTS_Listen(struct EVENTS* self)
{
	ALLEGRO_EVENT newEvent;
	al_wait_for_event(self->Queue, &newEvent);
	self->EventType = (uint32_t)newEvent.type;

	return self->EventType;
}
