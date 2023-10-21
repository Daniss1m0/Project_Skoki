#include "window.h"

void WINDOW_Init(struct WINDOW* self)
{
	self->Display = al_create_display(1280, 720);
	
#ifdef _MSC_VER
	if (!self->Display) __debugbreak(); // Nie udało utworzyć się okna.
#endif

	self->Running = true;
}

void WINDOW_SetTitle(struct WINDOW* self, const char* title)
{
	self->Title = title;
	al_set_window_title(self->Display, title);
}
