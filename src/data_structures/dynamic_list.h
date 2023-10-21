#pragma once

#include <memory>

struct NODE
{
	/* Logistyka */
	struct NODE* Next;
	struct NODE* Previous;
	
	/* Dane */
	void* Data;
};

struct NODE* NODE_New(void* data);

struct LIST
{
	/* Punkty wejścia */
	struct NODE* Head;
	struct NODE* Tail;

	/* Wewnętrzne (nie ruszać) */
	unsigned int __Size; // Rozmiar
	struct NODE* __LastPtr; // Pomocnik przy iterowaniu n -> n+1
	unsigned int __LastId; // Pomocnik przy iterowaniu n -> n+1
};

/* Dodaje element do listy. */
void LIST_Add(struct LIST* self, void* data);

/* Usuwa element z listy. */
void LIST_RemoveByValue(struct LIST* self, void* data);
void LIST_Remove(struct LIST* self, unsigned int id);

/* Znajduje element na liście. */
void* LIST_GetElement(struct LIST* self, unsigned int id);

/* Zwraca ilość elementów w liście. */
unsigned int LIST_GetSize(struct LIST* self);