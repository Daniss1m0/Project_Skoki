#pragma once

#include "window.h"
#include "util.h"
#include <stdint.h>
#include <allegro5/allegro5.h>

#define EVENT_NAME_LENGTH 12
/* Zdarzenia gry, takie jak np. kolizje obiektów. */
struct GAME_EVENTS
{
	char Name[EVENT_NAME_LENGTH];
};

void GAME_EVENTS_Call();

/* Struktura zdarzeń biblioteki (klawiatura, okno) */
struct EVENTS
{
	ALLEGRO_EVENT_QUEUE* Queue;
	uint32_t EventType;
};

/* Inicjuje strukturę zdarzeń. */
void EVENTS_Init(struct EVENTS* self);

/* Rejestruje źródła zdarzeń. */
void EVENTS_Register(struct EVENTS* self, struct WINDOW* window);

/* Przetwarza zdarzenia. */
void EVENTS_Process(struct EVENTS* self, struct WINDOW* window);

/* Nasłuchuje zdarzeń */
uint32_t EVENTS_Listen(struct EVENTS* self);