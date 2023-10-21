#pragma once

#include "window.h"
#include "events.h"
#include <allegro5/allegro5.h>

/* Struktura aplikacji. Zawiera główne elementy programu. */
struct APPLICATION
{
	struct WINDOW Window; // Okno gry
	struct EVENTS Events; // Kolejka zdarzeń gry
	bool Running; // Czy gra trwa?
} Game;

/* Inicjuje aplikację. */
void APPLICATION_Init(struct APPLICATION* self);

/* Główny kod aplikacji, posiada pętlę główną. */
int APPLICATION_Run(struct APPLICATION* self);