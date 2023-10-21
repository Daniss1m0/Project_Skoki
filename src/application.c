#include "application.h"

void APPLICATION_Init(struct APPLICATION* self)
{
	WINDOW_Init(&self->Window);
	EVENTS_Init(&self->Events);
	EVENTS_Register(&self->Events, &self->Window);

	self->Running = true;
}

int APPLICATION_Run(struct APPLICATION* self)
{
	// Inicjuje bibliotekę.
	al_init();

	// Inicjuje aplikację.
	APPLICATION_Init(self);

	WINDOW_SetTitle(&self->Window, "Skoki");

	// Pętla główna
	while (self->Window.Running)
	{
		// Rysowanie
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_flip_display();

		// Logika
		EVENTS_Listen(&self->Events);
		EVENTS_Process(&self->Events, &self->Window);
	}

	// Czyści pamięć.
	al_destroy_display(self->Window.Display);
	al_destroy_event_queue(self->Events.Queue);

	return 0;
}

