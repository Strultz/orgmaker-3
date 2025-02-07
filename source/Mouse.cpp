#include "Setting.h"
#include "Mouse.h"

extern int rebarHeight;

void MouseData::GetMousePosition(long *x,long *y)
{
	GetCursorPos(&point);
	ScreenToClient(hWnd,&point);
	*x = point.x;
	*y = point.y - rebarHeight;
}
