#pragma once
#include <windows.h>
#include <conio.h>
#include "ConsoleUI.h"
#include "Timer.h"

NS_START£û

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
				//Control* pCtrlOld = consoleUI()->getActiveControl();

				//Set current mouse pointer
				//  make sure getActiveControl right
				consoleUI()->setCurPosition(
					input_record.Event.MouseEvent.dwMousePosition.X,
					input_record.Event.MouseEvent.dwMousePosition.Y
				);

				//Control* pCtrl = consoleUI()->getActiveControl();
				//ControlSelectable* pSelectableOld = dynamic_cast<ControlSelectable*>(pCtrlOld);
				//ControlSelectable* pSelectable = dynamic_cast<ControlSelectable*>(pCtrl);
				//if (pCtrlOld && pCtrlOld != pCtrl && pSelectableOld != NULL && pSelectableOld->isSelectable())
				//{
				//	consoleUI()->redrawControl(pCtrlOld);
				//}
				//if (pCtrl && pSelectable != NULL && pSelectable->isSelectable())
				//{
				//	consoleUI()->redrawControl(pCtrl);
				//}
			}
		}
		else if (KEY_EVENT == input_record.EventType)
		{
			if (!input_record.Event.KeyEvent.bKeyDown)
			{
				return;
			}

			if (!isGlobalEvent() && consoleUI()->getActiveControl() != NULL)
				return;

			COORD curPt = consoleUI()->getCurPosition();

			WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);
			switch (k)
			{
			case 38://ÉÏ
			{
				if (curPt.Y > 0)
					consoleUI()->setCurPosition(curPt.X, curPt.Y - 1);
				break;
			}
			case 40://ÏÂ
			{
				consoleUI()->setCurPosition(curPt.X, curPt.Y + 1);
				break;
			}
			case 37://×ó
			{
				if (curPt.X > 0)
					consoleUI()->setCurPosition(curPt.X - 1, curPt.Y);
				break;
			}
			case 39://ÓÒ
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

		setGlobalEvent(true);
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

//Debug event
class EventDebug
	: public Event
{
public:
	EventDebug(ConsoleUI *consoleUI = NULL)
	{
		setConsoleUI(consoleUI);

		m_rect.X = 0;
		m_rect.Y = 0;
		m_rect.nWidth = 32;
		m_rect.nHeight = 6;

		m_event = MOUSE_EVENT | KEY_EVENT;
	}

	void setRect(const Rect& rect)
	{
		m_rect = rect;
	}

	void setPosition(int x, int y)
	{
		m_rect.X = x;
		m_rect.Y = y;
	}

	void setEvent(DWORD dwEvent)
	{
		m_event = dwEvent;
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return m_event;
	}

	virtual void onEvent(INPUT_RECORD &input_record)
	{
		consoleUI()->pushPosition();

		int x = m_rect.X;
		int y = m_rect.Y;

		consoleUI()->createBox(m_rect.X, m_rect.Y, m_rect.X + m_rect.nWidth, m_rect.Y + m_rect.nHeight, black, white);

		std::string sTxt = " Type:";
		sTxt += std::to_string(input_record.EventType);
		consoleUI()->drawText(x, y++, sTxt.c_str());

		if (input_record.EventType == KEY_EVENT)
		{
			sTxt = " ControlKey:";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) ? "LA(1)" : "LA(0)";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED) ? "RA(1)" : "RA(0)";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) ? "LC(1)" : "LC(0)";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) ? "RC(1)" : "RC(0)";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = "            ";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) ? "S(1)" : "S(0)";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & CAPSLOCK_ON) ? "CO(1)" : "CO(0)";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & NUMLOCK_ON) ? "NO(1)" : "NO(0)";
			sTxt += (input_record.Event.KeyEvent.dwControlKeyState & SCROLLLOCK_ON) ? "SO(1)" : "SO(0)";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = " KeyDownOrUp:";
			sTxt += input_record.Event.KeyEvent.bKeyDown? "Down" : "Up";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = " Repeat:";
			sTxt += std::to_string(input_record.Event.KeyEvent.wRepeatCount);
			sTxt += " VKCode:";
			sTxt += std::to_string(input_record.Event.KeyEvent.wVirtualKeyCode);
			sTxt += " VSCode:";
			sTxt += std::to_string(input_record.Event.KeyEvent.wVirtualScanCode);
			consoleUI()->drawText(x, y++, sTxt.c_str());

			static std::string sTxtOld;
			if (input_record.Event.KeyEvent.bKeyDown)
			{
				sTxt = " IsUnicode:";
				sTxt += (input_record.Event.KeyEvent.uChar.AsciiChar == input_record.Event.KeyEvent.uChar.UnicodeChar) ? "false" : "true";
				sTxt += " Char:";
				if (input_record.Event.KeyEvent.uChar.AsciiChar == input_record.Event.KeyEvent.uChar.UnicodeChar)
				{
					sTxt += input_record.Event.KeyEvent.uChar.AsciiChar;
				}
				else
				{
					WCHAR c[2];
					wsprintfW(c, L"%c", input_record.Event.KeyEvent.uChar.UnicodeChar);
					std::string ss;
					GL::WideByte2Ansi(ss, c);
					sTxt += ss;
				}
				sTxtOld = sTxt;
			}
			consoleUI()->drawText(x, y++, sTxtOld.c_str());
		}
		else
		{
			sTxt = " ControlKey:";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & LEFT_ALT_PRESSED) ? "LA(1)" : "LA(0)";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & RIGHT_ALT_PRESSED) ? "RA(1)" : "RA(0)";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & LEFT_CTRL_PRESSED) ? "LC(1)" : "LC(0)";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) ? "RC(1)" : "RC(0)";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = "            ";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & SHIFT_PRESSED) ? "S(1)" : "S(0)";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & CAPSLOCK_ON) ? "CO(1)" : "CO(0)";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & NUMLOCK_ON) ? "NO(1)" : "NO(0)";
			sTxt += (input_record.Event.MouseEvent.dwControlKeyState & SCROLLLOCK_ON) ? "SO(1)" : "SO(0)";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = " Button:";
			sTxt += (input_record.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) ? "L1(1)" : "L1(0)";
			sTxt += (input_record.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) ? "L2(1)" : "L2(0)";
			sTxt += (input_record.Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED) ? "L3(1)" : "L3(0)";
			sTxt += (input_record.Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED) ? "L4(1)" : "L4(0)";
			sTxt += (input_record.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) ? "R(1)" : "R(0)";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = " Action:";
			sTxt += (input_record.Event.MouseEvent.dwEventFlags & MOUSE_MOVED) ? "MM(1)" : "MM(0)";
			sTxt += (input_record.Event.MouseEvent.dwEventFlags & DOUBLE_CLICK) ? "DC(1)" : "DC(0)";
			sTxt += (input_record.Event.MouseEvent.dwEventFlags & MOUSE_WHEELED) ? "MW(1)" : "MW(0)";
			sTxt += (input_record.Event.MouseEvent.dwEventFlags & MOUSE_HWHEELED) ? "MH(1)" : "MH(0)";
			consoleUI()->drawText(x, y++, sTxt.c_str());
			sTxt = " MousePosition:";
			sTxt += std::to_string(input_record.Event.MouseEvent.dwMousePosition.X) + ',' +
				std::to_string(input_record.Event.MouseEvent.dwMousePosition.Y);
			consoleUI()->drawText(x, y++, sTxt.c_str());
		}

		consoleUI()->popPosition();
	}

private:
	Rect m_rect;
	DWORD m_event;
};

£ýNS_END