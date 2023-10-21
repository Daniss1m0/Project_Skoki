#pragma once

#include <stdbool.h>
#include <allegro5/allegro5.h>

/* Struktura okienka. */
struct WINDOW
{
	ALLEGRO_DISPLAY* Display;
	bool Running;
	const char* Title;
};

/* Inicjuje okienko. */
void WINDOW_Init(struct WINDOW* self);

/* Ustawia tytuł okna. */
void WINDOW_SetTitle(struct WINDOW* self, const char* title);