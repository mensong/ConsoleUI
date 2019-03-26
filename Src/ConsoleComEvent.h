#pragma once
#include <windows.h>
#include <conio.h>
#include "ConsoleUI.h"
#include "Timer.h"
#include "string_utility.h"

NS_START£û

//mouse pointer follow
class EventMouseFollow
	: public Event
{
public:
	EventMouseFollow(ConsoleUI *consoleUI = NULL);

protected:
	virtual EVENT_TYPE eventType() { return MOUSE_EVENT | KEY_EVENT; }

	virtual void onEvent(INPUT_RECORD &input_record);
};

//Resize window
class EventResizeWindow
	: public Event
{
public:
	EventResizeWindow(ConsoleUI *consoleUI = NULL);

protected:
	virtual EVENT_TYPE eventType() { return WINDOW_BUFFER_SIZE_EVENT; }

	virtual void onEvent(INPUT_RECORD &input_record);

private:
	DWORD m_dwLast;
	Timer m_timer;
	DWORD m_dwSpTime;
};

//Debug event
class EventDebug
	: public Event
{
public:
	EventDebug(ConsoleUI *consoleUI = NULL);

	void setRect(const Rect& rect) { m_rect = rect; }

	void setPosition(int x, int y);

	void setEvent(DWORD dwEvent);

protected:
	virtual EVENT_TYPE eventType() { return m_event; }

	virtual void onEvent(INPUT_RECORD &input_record);

private:
	Rect m_rect;
	DWORD m_event;
};

£ýNS_END