#pragma once
#include <windows.h>
#include <conio.h>
#include "ConsoleUI.h"
#include "Timer.h"

namespace GL
{

//mouse pointer follow
class EventMouseFollow
	: public Event
{
public:
	EventMouseFollow(ConsoleUI *consoleUI = NULL)
	{
		setConsoleUI(consoleUI);
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return MOUSE_EVENT | KEY_EVENT;
	}

	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (MOUSE_EVENT == input_record.EventType)
		{
			if (FROM_LEFT_1ST_BUTTON_PRESSED == input_record.Event.MouseEvent.dwButtonState
				|| RIGHTMOST_BUTTON_PRESSED == input_record.Event.MouseEvent.dwButtonState)
			{
				//the pointer follow mouse click
				consoleUI()->setCurPosition(
					input_record.Event.MouseEvent.dwMousePosition.X,
					input_record.Event.MouseEvent.dwMousePosition.Y
				);
			}
		}
		else if (KEY_EVENT == input_record.EventType)
		{
			if (!input_record.Event.KeyEvent.bKeyDown)
			{
				return;
			}

			if (consoleUI()->getActiveControl() != NULL)
				return;

			COORD curPt = consoleUI()->getCurPosition();

			WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);
			switch (k)
			{
			case 38://ио
			{
				if (curPt.Y > 0)
					consoleUI()->setCurPosition(curPt.X, curPt.Y - 1);
				break;
			}
			case 40://об
			{
				consoleUI()->setCurPosition(curPt.X, curPt.Y + 1);
				break;
			}
			case 37://вС
			{
				if (curPt.X > 0)
					consoleUI()->setCurPosition(curPt.X - 1, curPt.Y);
				break;
			}
			case 39://ср
			{
				consoleUI()->setCurPosition(curPt.X + 1, curPt.Y);
				break;
			}
			case 36://home
			{
				consoleUI()->setCurPosition(0, curPt.Y);
				break;
			}
			case 35://end
			{
				consoleUI()->setCurPosition(consoleUI()->getConsoleScreenInfo().dwSize.X - 1, curPt.Y);
				break;
			}
			}
		}
	}
};

//Resize window
class EventResizeWindow
	: public Event
{
public:
	EventResizeWindow(ConsoleUI *consoleUI = NULL)
		: m_dwLast(0)
		, m_dwSpTime(500)
	{
		setConsoleUI(consoleUI);
	}
	
protected:
	virtual EVENT_TYPE eventType()
	{
		return WINDOW_BUFFER_SIZE_EVENT;
	}

	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (WINDOW_BUFFER_SIZE_EVENT == input_record.EventType)
		{
			m_timer.StopTimer();
			m_timer.StartTimer(500, [&](void)->void {
				m_timer.StopTimer();
				consoleUI()->redraw();
			});
			m_dwLast = GetTickCount();
		}		
	}

private:
	DWORD m_dwLast;
	Timer m_timer;
	DWORD m_dwSpTime;
};

}