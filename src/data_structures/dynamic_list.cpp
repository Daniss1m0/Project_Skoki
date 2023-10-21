#include "dynamic_list.h"

struct NODE* NODE_New(void* data)
{
	struct NODE* node = (struct NODE*)malloc(sizeof(struct NODE));
	node->Data = data;

	return node;
}

void LIST_Add(struct LIST* self, void* data)
{
	struct NODE* node = NODE_New(data);

	if (self->Tail)
	{
		self->Tail->Next = node;
		node->Previous = self->Tail;
		node->Next = self->Head;
		self->Head->Previous = node;
	}
	else
	{
		self->Head = node;
		self->Tail = node;
		
		self->Head->Next = node;
		self->Head->Previous = node;
	}

	self->__Size++;
}

void LIST_RemoveByValue(struct LIST* self, void* data)
{

}

void LIST_Remove(struct LIST* self, unsigned int id)
{

}

void* LIST_GetElement(struct LIST* self, unsigned int id)
{
	if (id > self->__Size)
		return (void*)NULL;

	// Szybki dostęp dla n -> n + 1
	if (id == self->__LastId + 1)
	{
		self->__LastPtr = self->__LastPtr->Next;
		self->__LastId = id;

		return self->__LastPtr->Data;
	}

	struct NODE* jumper = self->Head;

	if (self->__Size / 2 > id)
		for (int i = 0; i < id; i++)
			jumper = jumper->Next;
	else
		for (int i = self->__Size; i > id; i--)
			jumper = jumper->Previous;

	self->__LastPtr = jumper;
	self->__LastId = id;

	return jumper->Data;
}

unsigned int LIST_GetSize(struct LIST* self)
{
	return self->__Size;
}
