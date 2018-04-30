
#ifndef  CONSOLEAPP_H 
#define CONSOLEAPP_H 
#include <windows.h>
#include <conio.h>
#include <vector>
#include <map>
#include <time.h>
#include <locale.h>
#include <Convertor.h>
#include <string>
#include "RTreeEx.h"

#define NS_START｛ namespace GL{
#define ｝NS_END }

NS_START｛

enum COLOR
{
	/*  0x0000FF
	 *  颜色占低8位
	 */

	color_default = -1,
	black = 0,
	dark_blue = 1,
	dark_green = 2,
	dark_aqua, dark_cyan = 3,
	dark_red = 4,
	dark_purple = 5, dark_pink = 5, dark_magenta = 5,
	dark_yellow = 6,
	dark_white = 7,
	gray = 8,
	blue = 9,
	green = 10,
	aqua = 11, cyan = 11,
	red = 12,
	purple = 13, pink = 13, magenta = 13,
	yellow = 14,
	white = 15,
};

enum STYLE
{
	/*  0xFFFF00
	 *  样式占高16位，其中：
	 *    0x000F00 - 影响文本颜色
	 *    0x00F000 - 影响背景颜色
	 *    0xFF0000 - 影响格子样式
	 */

	style_default = 0,
	//affact text color
	foreground_blue = 0x0001, //text color contains blue.
	foreground_green = 0x0002, //text color contains green.
	foreground_red = 0x0004, //text color contains red.
	foreground_intensity = 0x0008, //text color is intensified.
	//affact backgroup color
	background_blue = 0x0010, //background color contains blue.
	background_green = 0x0020, //background color contains green.
	background_red = 0x0040, //background color contains red.
	background_intensity = 0x0080, //background color is intensified.
	//affact grid
	common_lvb_leading_byte = 0x0100, //leading byte of dbcs
	common_lvb_trailing_byte = 0x0200, //trailing byte of dbcs
	common_lvb_grid_horizontal = 0x0400, //dbcs: grid attribute: top horizontal.
	common_lvb_grid_lvertical = 0x0800, //dbcs: grid attribute: left vertical.
	common_lvb_grid_rvertical = 0x1000, //dbcs: grid attribute: right vertical.
	common_lvb_reverse_video = 0x4000, //dbcs: reverse fore/back ground attribute.
	common_lvb_underscore = 0x8000, //dbcs: underscore.
	common_lvb_sbcsdbcs = 0x0300, //sbcs or dbcs flag.
};

class ConsoleUI;
class Control;

class Console
{
public:
	Console()
	{
	}
	virtual ~Console() {}

public:
	//Holder
	ConsoleUI *consoleUI() const
	{
		return m_pParent;
	}

	void setConsoleUI(ConsoleUI *pUI)
	{ 
		m_pParent = pUI;
	}

	void setID(const char* id)
	{
		m_id = id;
	}

	const std::string& getID() const
	{
		return m_id;
	}

	template<class T>
	T* CAST_TO()
	{
		return dynamic_cast<T*>(this);
	}

private:
	ConsoleUI *m_pParent;

	std::string m_id;
};


class Event
	: virtual public Console
{
	friend class ConsoleUI;

public:
	typedef int EVENT_TYPE;
	typedef DWORD INPUT_MODE;

	Event()
		: m_bGlobalEvent(false)
	{

	}
	virtual ~Event()
	{

	}

protected:
	//#define KEY_EVENT         0x0001 // Event contains key event record
	//#define MOUSE_EVENT       0x0002 // Event contains mouse event record
	//#define WINDOW_BUFFER_SIZE_EVENT 0x0004 // Event contains window change event record
	//#define MENU_EVENT 0x0008 // Event contains menu event record
	//#define FOCUS_EVENT 0x0010 // event contains focus change
	// use | combine them
	virtual EVENT_TYPE eventType() = 0;

	//on initialize
	virtual bool onPreInitEvent() { return true; }
	virtual void onInitEvent() { }

	//on begin event
	virtual void onBeginEvent(INPUT_RECORD &input_record) { }
	//on event
	// loop event break if return value is false
	virtual void onEvent(INPUT_RECORD &input_record) { }
	//on end event
	virtual void onEndEvent(INPUT_RECORD &input_record) { }

public:
	static INPUT_MODE GetInputModeByEventType(EVENT_TYPE eventType)
	{
		//#define KEY_EVENT         0x0001 // Event contains key event record
		//#define MOUSE_EVENT       0x0002 // Event contains mouse event record
		//#define WINDOW_BUFFER_SIZE_EVENT 0x0004 // Event contains window change event record
		//#define MENU_EVENT 0x0008 // Event contains menu event record
		//#define FOCUS_EVENT 0x0010 // event contains focus change

		//#define ENABLE_PROCESSED_INPUT  0x0001
		//#define ENABLE_LINE_INPUT       0x0002
		//#define ENABLE_ECHO_INPUT       0x0004
		//#define ENABLE_WINDOW_INPUT     0x0008
		//#define ENABLE_MOUSE_INPUT      0x0010
		//#define ENABLE_INSERT_MODE      0x0020
		//#define ENABLE_QUICK_EDIT_MODE  0x0040
		//#define ENABLE_EXTENDED_FLAGS   0x0080
		//#define ENABLE_AUTO_POSITION    0x0100
		switch (eventType)
		{
		case KEY_EVENT:
			return 0;
			break;
		case MOUSE_EVENT:
			return ENABLE_MOUSE_INPUT;
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			return ENABLE_WINDOW_INPUT;
			break;
		case MENU_EVENT:
			return 0;
			break;
		case FOCUS_EVENT:
			return 0;
			break;
		}

		return 0;
	}

	static std::vector<EVENT_TYPE> getAllEventType()
	{
		//#define KEY_EVENT         0x0001 // Event contains key event record
		//#define MOUSE_EVENT       0x0002 // Event contains mouse event record
		//#define WINDOW_BUFFER_SIZE_EVENT 0x0004 // Event contains window change event record
		//#define MENU_EVENT 0x0008 // Event contains menu event record
		//#define FOCUS_EVENT 0x0010 // event contains focus change
		std::vector<EVENT_TYPE> vctRet;
		vctRet.push_back(KEY_EVENT);
		vctRet.push_back(MOUSE_EVENT);
		vctRet.push_back(WINDOW_BUFFER_SIZE_EVENT);
		vctRet.push_back(MENU_EVENT);
		vctRet.push_back(FOCUS_EVENT);

		return vctRet;
	}

	void setGlobalEvent(bool b)
	{
		m_bGlobalEvent = b;
	}
	bool isGlobalEvent() const
	{
		return m_bGlobalEvent;
	}

private:
	bool m_bGlobalEvent;//是否是全局事件，如果是全局事件，则无论焦点是不是在该控件上等情况都会触发，非必要不要用
};


//rect
typedef struct tagRect
{
	int X;
	int Y;
	int nWidth;
	int nHeight;

	tagRect(int x=0, int y=0, int width=0, int height=0)
	{
		X = x;
		Y = y;
		nWidth = width;
		nHeight = height;
	}

	tagRect(const tagRect& rect)
	{
		X = rect.X;
		Y = rect.Y;
		nWidth = rect.nWidth;
		nHeight = rect.nHeight;
	}

	bool isIn(int nX, int nY) const
	{
		return((nX >= X && nX < (X + nWidth)) && (nY >= Y && nY < (Y + nHeight)));
	}
} Rect;


//Console Color
class ConsoleColor
{
public:
	ConsoleColor(COLOR bkColor = color_default, COLOR textColor = color_default, STYLE style = style_default)
		: m_bkColor(bkColor)
		, m_textColor(textColor)
		, m_Style(style)
	{

	}

	COLOR getBkColor() const { return m_bkColor; }
	COLOR getTextColor() const { return m_textColor; }
	STYLE getStyle() const { return m_Style; }

	void setBkColor(COLOR bkColor) { m_bkColor = bkColor; }
	void setTextColor(COLOR textColor) { m_textColor = textColor; }
	void setStyle(STYLE style) { m_Style = style; }

protected:
	COLOR m_bkColor;
	COLOR m_textColor;
	STYLE m_Style;
};

//control base
class Control
	: virtual public Console
	, public ConsoleColor
{
public:
	Control()
		: m_bVisible(true)
		, m_bEnable(true)
	{
		m_rect.X = 0;
		m_rect.Y = 0;
		m_rect.nWidth = 1;
		m_rect.nHeight = 1;
	}

	virtual ~Control() {}

public:
	//on initialize
	virtual bool onPreInitControl() { return true; }
	virtual void onInitControl() {}

	virtual bool draw()
	{
		return m_bVisible;
	}

	virtual Rect getRect() const
	{
		return m_rect;
	}

	inline const Rect& rect() const
	{
		return m_rect;
	}

	virtual void getPointColorAndStyle(int x, int y, COLOR& bkClolor, COLOR& textColor, STYLE& style) const
	{
		bkClolor = this->getBkColor();
		textColor = this->getTextColor();
		style = this->getStyle();
	}

	void setVisible(bool b)
	{
		m_bVisible = b;
	}

	bool isVisible() const
	{
		return m_bVisible;
	}

	void setEnable(bool b)
	{
		m_bEnable = b;
	}

	bool isEnable() const
	{
		return m_bEnable;
	}

protected:
	bool m_bVisible;
	bool m_bEnable;

private:
	friend class ConsoleUI;
	void setRect(const Rect& rect)
	{
		m_rect = rect;
	}
	void setRect(int x, int y, int nWidth, int nHeight)
	{
		m_rect.X = x;
		m_rect.Y = y;
		m_rect.nWidth = nWidth;
		m_rect.nHeight = nHeight;
	}
	Rect m_rect;
};


//main class
class ConsoleUI
	: public ConsoleColor
{
public:
	typedef std::vector<Event *> EVENT_LIST;
	typedef std::map<Event::EVENT_TYPE, EVENT_LIST> EVENT_MAP;

public:
	ConsoleUI(COLOR bkColor = color_default, COLOR textColor = color_default, STYLE style = style_default)
		: m_loop(true)
		, ConsoleColor(bkColor, textColor, style)
	{
		m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hIn = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleMode(m_hIn, 0);

		if (m_textColor == color_default)
			m_textColor = white;
		if (m_bkColor == color_default)
			m_bkColor = black;
		if (m_Style == style_default)
			m_Style = style_default;

		clearScreen(m_textColor, m_bkColor, m_Style);
	}

	~ConsoleUI() {}

	HANDLE getOutConsole() const { return m_hOut; }
	HANDLE getInConsole() const { return m_hIn; }

	void setTitle(const char* title)
	{
		SetConsoleTitleA(title);
	}

	//获得屏模信息
	CONSOLE_SCREEN_BUFFER_INFO getConsoleScreenInfo() const
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(m_hOut, &csbi);
		return csbi;
	}

	void setScreenBufferSize(int nWidth=-1, int nHeight=-1)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi = getConsoleScreenInfo();
		if (nWidth > 0)
		{
			csbi.dwSize.X = nWidth;
		}
		if (nHeight > 0)
		{
			csbi.dwSize.Y = nHeight;
		}
		
		SetConsoleScreenBufferSize(m_hOut, csbi.dwSize);
	}

	WORD getAttributeByPoint(int x, int y)
	{
		WORD wAttrib = 0;
		DWORD dwReaded = 0;
		COORD pt;
		pt.X = x;
		pt.Y = y;
		ReadConsoleOutputAttribute(m_hOut, &wAttrib, 1, pt, &dwReaded);

		return wAttrib;
	}

	void getColorAndStyleByPoint(int x, int y, COLOR &textColor, COLOR& bkColor, STYLE& style)
	{
		WORD wAttrib = getAttributeByPoint(x, y);
		COLOR allColor = (COLOR)(wAttrib & 0x00ff);//clear style bit
		textColor = (COLOR)(allColor & 0x0f);
		bkColor = (COLOR)((allColor & 0xf0) >> 4);
		style = (STYLE)((wAttrib & 0xff0000) >> 8);//clear color bit
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void setCurPosition(int x, int y)
	{
		COORD coord;
		coord.X = x; coord.Y = y;
		SetConsoleCursorPosition(m_hOut, coord);
		return;
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	COORD getCurPosition() const
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(m_hOut, &csbi);
		return csbi.dwCursorPosition;
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void setCurStyle(STYLE style)
	{
		WORD wStyle = (WORD)style;

		//keep color
		WORD wAttribute = 0;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
		{
			wAttribute = csbiInfo.wAttributes & 0x00ff;//clear style bit
		}
		wAttribute |= wStyle;

		SetConsoleTextAttribute(m_hOut, wAttribute);
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void setCurColor(COLOR textColor, COLOR bkColor)
	{
		if (textColor == color_default)
			textColor = white;
		if (bkColor == color_default)
			bkColor = black;

		WORD wColor = ((unsigned int)bkColor << 4) | (unsigned int)textColor;

		//keep style
		WORD wAttribute = 0;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
		{
			wAttribute = csbiInfo.wAttributes & 0xffff00;//clear color bit
		}
		wAttribute |= wColor;

		SetConsoleTextAttribute(m_hOut, wAttribute);
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void getCurColor(COLOR &textColor, COLOR &bkColor) const
	{
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
		{
			WORD wColor = csbiInfo.wAttributes & 0x00ff;//clear style bit
			textColor = (COLOR)(wColor & 0x0f);
			bkColor = (COLOR)(wColor & 0xf0);
		}
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************

	void getCurStyle(STYLE &style) const
	{
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
		{
			WORD wStyle = csbiInfo.wAttributes & 0xffff00;//clear color bit
			style = (STYLE)wStyle;
		}
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void clearCurColor()
	{
		setCurColor(color_default, color_default);
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void clearCurStyle()
	{
		setCurStyle(style_default);
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void fillColor(int x, int y, DWORD dwCellFillCount, COLOR textColor, COLOR bkColor)
	{
		if (textColor == color_default)
			textColor = white;
		if (bkColor == color_default)
			bkColor = black;

		WORD wColor = ((unsigned int)bkColor << 4) | (unsigned int)textColor;

		//keep style
		WORD wAttribute = 0;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
		{
			wAttribute = csbiInfo.wAttributes & 0xffff00;//clear color bit
		}
		wAttribute |= wColor;

		COORD coordStart;
		coordStart.X = x;
		coordStart.Y = y;

		DWORD dwWritted = 0;
		FillConsoleOutputAttribute(
			m_hOut,          // screen buffer handle 
			wAttribute,      // color to fill with 
			dwCellFillCount, // number of cells to fill 
			coordStart,      // first cell to write to 
			&dwWritted);           // actual number written 
	}

	void fillColorAndStyle(int x, int y, DWORD dwCellFillCount, COLOR textColor = color_default, COLOR bkColor = color_default, STYLE style = style_default)
	{
		if (textColor == color_default)
			textColor = white;
		if (bkColor == color_default)
			bkColor = black;

		WORD wColor = ((unsigned int)bkColor << 4) | (unsigned int)textColor;
		
		WORD wAttribute = style;
		wAttribute |= wColor;

		COORD coordStart;
		coordStart.X = x;
		coordStart.Y = y;

		DWORD dwWritted = 0;
		FillConsoleOutputAttribute(
			m_hOut,          // screen buffer handle 
			wAttribute,      // color to fill with 
			dwCellFillCount, // number of cells to fill 
			coordStart,      // first cell to write to 
			&dwWritted);           // actual number written 
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void fillStyle(int x, int y, DWORD dwCellFillCount, STYLE style)
	{
		WORD wStyle = (WORD)style;

		//keep color
		WORD wAttribute = 0;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
		{
			wAttribute = csbiInfo.wAttributes & 0x0000ff;//clear style bit
		}
		wAttribute |= wStyle;

		COORD coordStart;
		coordStart.X = x;
		coordStart.Y = y;

		DWORD dwWritted = 0;
		FillConsoleOutputAttribute(
			m_hOut,          // screen buffer handle 
			wAttribute,      // color to fill with 
			dwCellFillCount, // number of cells to fill 
			coordStart,      // first cell to write to 
			&dwWritted);     // actual number written 
	}

	void drawPoint(int x, int y, const char* txt, COLOR textColor = color_default, COLOR bkColor = color_default, STYLE style = style_default)
	{
		setCurColor(textColor, bkColor);
		setCurStyle(style);

		setCurPosition(x, y);
		_cprintf(txt);
	}

	void drawText(int x, int y, const char* txt)
	{
		setCurPosition(x, y);
		printf(txt);
	}

	void drawTextW(int x, int y, const WCHAR* txt)
	{
		setlocale(LC_ALL, "chs");
		setCurPosition(x, y);
		wprintf(txt);
	}

	void setBkColor(COLOR bkColor, bool redraw) 
	{
		m_bkColor = bkColor;
		if (redraw)
			clearScreen(m_textColor, m_bkColor, m_Style);
	}
	void setTextColor(COLOR textColor, bool redraw)
	{ 
		m_textColor = textColor; 
		if (redraw)
			clearScreen(m_textColor, m_bkColor, m_Style);
	}
	void setStyle(STYLE style, bool redraw)
	{ 
		m_Style = style; 
		if (redraw)
			clearScreen(m_textColor, m_bkColor, m_Style);
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void clearScreen(COLOR textColor = color_default, COLOR bkColor = color_default,
		STYLE style = style_default, char cClearText = ' ')
	{
		COORD coordScreen = { 0, 0 };
		DWORD cCharsWritten;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		DWORD dwConSize;
		WORD wAttribute = 0;
		HANDLE hConsole = m_hOut;

		GetConsoleScreenBufferInfo(hConsole, &csbi);
		dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
		FillConsoleOutputCharacterA(hConsole, cClearText, dwConSize, coordScreen, &cCharsWritten);
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		wAttribute = csbi.wAttributes;
		if (textColor != color_default)
		{
			wAttribute &= 0xfff0;
			wAttribute |= (unsigned int)textColor;
		}
		if (bkColor != color_default)
		{
			wAttribute &= 0xff0f;
			wAttribute |= ((unsigned int)bkColor << 4);
		}
		if (style != style_default)
		{
			wAttribute &= 0x00ff;
			wAttribute |= style;
		}
		FillConsoleOutputAttribute(hConsole, wAttribute, dwConSize, coordScreen, &cCharsWritten);
		SetConsoleCursorPosition(hConsole, coordScreen);
		return;
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void createBox(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
		COLOR textCol, COLOR bkcol, STYLE style = style_default)
	{
		int x, y;
		setCurColor(textCol, bkcol);                       //Set to color bkcol
		setCurStyle(style);

		//for (y = y1; y < y2; y++)                    //Fill Y Region Loop
		//{
		//	for (x = x1; x < x2; x++)               //Fill X region Loop
		//	{
		//		setCurPosition(x, y); _cprintf(" ");       //Draw Solid space
		//	}
		//}

		std::string sSpaces(x2 - x1, ' ');

		for (y = y1; y < y2; ++y)
		{
			setCurPosition(x1, y);
			_cprintf(sSpaces.c_str());
		}
	}

	void createBorder(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
	{
		if (x1 >= x2 || y1 >= y2)
		{
			return;
		}

		--x2;
		--y2;

		COLOR textColor = color_default;
		COLOR boxColor = color_default;
		STYLE style = style_default;

		COORD pt;
		if (x1 == x2)
		{
			pt.X = x1;
			pt.Y = y1;
			if (y1 == y2)
			{//只画一点
				getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
				fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor,
					(STYLE)(style | common_lvb_grid_horizontal | common_lvb_underscore | common_lvb_grid_lvertical | common_lvb_grid_rvertical));
				return;
			}

			//第一行
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_horizontal | common_lvb_grid_lvertical | common_lvb_grid_rvertical));
			//中间的行
			for (int i = 1; i < y2 - y1; ++i)
			{
				++pt.Y;
				getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
				fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_lvertical | common_lvb_grid_rvertical));
			}
			//最下行
			++pt.Y;
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_underscore | common_lvb_grid_lvertical | common_lvb_grid_rvertical));
			return;
		}
		else if (y1 == y2)
		{
			pt.X = x1;
			pt.Y = y1;

			//到此没有x1==x2了

			//第一列
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor,
				(STYLE)(style | common_lvb_grid_horizontal | common_lvb_underscore | common_lvb_grid_lvertical));
			//中间的列
			for (int i = 1; i < x2 - x1; ++i)
			{
				++pt.X;
				getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
				fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_horizontal | common_lvb_underscore));
			}
			//最后一列
			++pt.X;
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor,
				(STYLE)(style | common_lvb_grid_horizontal | common_lvb_underscore | common_lvb_grid_rvertical));
			return;
		}
		else /* 行数与列数都不为1 */
		{
			pt.X = x1;
			pt.Y = y1;

			//第一行
			//  第一行的第一列格子
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_horizontal | common_lvb_grid_lvertical));
			//  第一行的中间列格子
			++pt.X;
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, x2 - x1 - 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_horizontal));
			//  第一行最后列的格子
			pt.X = x2;
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_horizontal | common_lvb_grid_rvertical));

			//中间行的格子
			for (int i = 1; i < y2 - y1; ++i)
			{
				pt.X = x1;
				++pt.Y;

				//  第一列格子
				getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
				fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_lvertical));

				//  中间列格子
				++pt.X;
				getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
				fillColorAndStyle(pt.X, pt.Y, x2 - x1 - 1, textColor, boxColor, style);

				//  最后列的格子
				pt.X = x2;
				getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
				fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_grid_rvertical));

			}

			//最后行的格子
			pt.X = x1;
			++pt.Y;
			//  最后一行的第一列格子
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_underscore | common_lvb_grid_lvertical));
			//  最后一行的中间列格子
			++pt.X;
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, x2 - x1 - 1, textColor, boxColor, (STYLE)(style | common_lvb_underscore));
			//  最后一行最后列的格子
			pt.X = x2;
			getColorAndStyleByPoint(pt.X, pt.Y, textColor, boxColor, style);
			fillColorAndStyle(pt.X, pt.Y, 1, textColor, boxColor, (STYLE)(style | common_lvb_underscore | common_lvb_grid_rvertical));
		}
	}

	//里面没有内容
	void createBoxWithBorder(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
		COLOR textColor, COLOR boxColor, STYLE style = style_default)
	{
		if (x1 >= x2 || y1 >= y2)
		{
			return;
		}

		createBox(x1, y1, x2, y2, textColor, boxColor, style);
		createBorder(x1, y1, x2, y2);
	}



	//*****************************************************************************
	//* registerEvent: These two reson case faild:                                *
	//*   1.pEvent is NULL                                                        *
	//*   2.Event::eventType() is invaild                                         *
	//*****************************************************************************
	BOOL addEvent(Event *pEvent)
	{
		if (NULL == pEvent)
		{
			return FALSE;
		}

		if (!pEvent->onPreInitEvent())
			return FALSE;

		BOOL bAdded = FALSE;

		//demolition the event type
		Event::EVENT_TYPE eventType = pEvent->eventType();
		std::vector<Event::EVENT_TYPE> vctEventType = Event::getAllEventType();
		int nSizeEventType = (int)vctEventType.size();
		Event::EVENT_TYPE eventTypeComp;
		for (int i = 0; i < nSizeEventType; ++i)
		{
			eventTypeComp = vctEventType[i];
			if (eventTypeComp & eventType) // if contain
			{
				m_mapEvents[eventTypeComp].push_back(pEvent);//Add to exectute list
				bAdded = TRUE;
			}
		}

		if (TRUE == bAdded)
		{
			//initialize
			pEvent->onInitEvent();
		}

		return bAdded;
	}

	BOOL addControl(Control* pControl, bool bDraw = true, bool bInit = true)
	{
		if (!pControl->onPreInitControl())
			return FALSE;

		updateControlRect(pControl);

		m_vctControls.push_back(pControl);

		if (bInit)
			pControl->onInitControl();

		if (bDraw)
			pControl->draw();

		return TRUE;
	}

	const std::vector<Control*>& getControls() const
	{
		return m_vctControls;
	}

	bool addControlRect(Control* pControl)
	{
		Rect rect = pControl->getRect();
		float aMin[2] = { rect.X, rect.Y };
		float aMax[2] = { rect.X + rect.nWidth - 1, rect.Y + rect.nHeight - 1 };
		return m_rtRect2Control.Insert(aMin, aMax, pControl);
	}

	bool removeControlRect(Control* pControl)
	{
		Rect rect = pControl->getRect();
		float aMin[2] = { rect.X, rect.Y };
		float aMax[2] = { rect.X + rect.nWidth, rect.Y + rect.nHeight };
		return m_rtRect2Control.Remove(aMin, aMax, pControl);
	}

	bool updateControlRect(Control* pControl)
	{
		removeControlRect(pControl);
		return addControlRect(pControl);
	}

	Console* getConsoleById(const std::string& id)
	{
		for (int i = 0; i < m_vctControls.size(); ++i)
		{
			if (m_vctControls[i]->getID() == id)
				return m_vctControls[i];
		}

		for (EVENT_MAP::const_iterator cit = m_mapEvents.begin(); cit != m_mapEvents.end(); ++cit)
		{
			for (int i = 0; i < cit->second.size(); ++i)
			{
				if (cit->second[i]->getID() == id)
					return cit->second[i];
			}
		}

		return NULL;
	}

	BOOL removeControl(Control* pControl)
	{
		sweepControlColorAndStyle(pControl);

		BOOL bRet1 = FALSE;
		BOOL bRet2 = FALSE;

		std::vector<Control*>::iterator itFind = std::find(m_vctControls.begin(), m_vctControls.end(), pControl);
		if (itFind != m_vctControls.end())
		{
			m_vctControls.erase(itFind);
			bRet1 = TRUE;
		}

		Rect rect = pControl->getRect();
		float aMin[2] = { rect.X, rect.Y };
		float aMax[2] = { rect.X + rect.nWidth/*-1*/, rect.Y + rect.nHeight/*-1*/ };
		bRet2 = m_rtRect2Control.Remove(aMin, aMax, pControl) ? TRUE : FALSE;

		return bRet1 && bRet2;
	}

	Control* getTopControl(int x, int y, Control* pExclude = NULL)  const
	{
		float point[2] = { x, y };
		std::set<Control*> setControls;
		m_rtRect2Control.Search(point, point, &setControls);

		//找到顶层开始查找
		for (int i = m_vctControls.size() - 1; i >= 0; --i)
		{
			if (setControls.find(m_vctControls[i]) != setControls.end())
			{
				Control* pCtrl = m_vctControls[i];
				if (pCtrl != pExclude && pCtrl->isVisible() && pCtrl->getRect().isIn(x, y))
					return m_vctControls[i];
			}
		}

		return NULL;
	}

	Control* getActiveControl() const
	{
		COORD pt = getCurPosition();
		return getTopControl(pt.X, pt.Y);
	}

	int getControlLayerId(Control* pCtrl)
	{
		int nLayer = std::find(m_vctControls.begin(), m_vctControls.end(), pCtrl) - m_vctControls.begin();
		if (nLayer < m_vctControls.size())
			return nLayer;
		return -1;
	}

	Control* getControlByLayerId(int nLayerId)
	{
		if (0 > nLayerId || nLayerId >= m_vctControls.size())
			return NULL;
		return m_vctControls[nLayerId];
	}

	bool isControlActive(Control* pCtrl) const
	{
		return getActiveControl() == pCtrl;
	}

	Control* getControlAtPoint(int x, int y) const
	{
		return getTopControl(x, y);
	}

	//如果rectTo中的属性为-1时不改变原值
	BOOL setControlRect(Control* pControl, const Rect& rectTo, bool redraw=false, bool bFailOverWidth=true)
	{
		if (bFailOverWidth && rectTo.X + rectTo.nWidth > getConsoleScreenInfo().dwSize.X)
		{
			return FALSE;
		}

		Rect _rectFrom = pControl->getRect();
		Rect _rectTo(
			rectTo.X == -1 ? _rectFrom.X : rectTo.X, rectTo.Y == -1 ? _rectFrom.Y : rectTo.Y,
			rectTo.nWidth == -1 ? _rectFrom.nWidth : rectTo.nWidth, rectTo.nHeight == -1 ? _rectFrom.nHeight : rectTo.nHeight);

		if (redraw)
		{
			//先移除旧区域，再绘制新区域
			//  移除旧区域时，获得旧区域下的颜色，再根据这颜色去刷新
			for (int x = _rectFrom.X; x < _rectFrom.X + _rectFrom.nWidth; ++x)
			{
				for (int y = _rectFrom.Y; y < _rectFrom.Y + _rectFrom.nHeight; ++y)
				{
					if (!_rectTo.isIn(x, y))
					{
						Control* pTopControl = getTopControl(x, y, pControl);
						if (pTopControl)
						{
							COLOR bkClolor; COLOR textColor; STYLE style;
							pTopControl->getPointColorAndStyle(x, y, bkClolor, textColor, style);
							drawPoint(x, y, " ", textColor, bkClolor, style);
						}
						else
						{
							drawPoint(x, y, " ", getTextColor(), getBkColor(), getStyle());
						}
					}
				}
			}
		}
		
		pControl->setRect(_rectTo);
		bool bRet = updateControlRect(pControl);

		if (redraw)
			redrawControl(pControl, redraw, redraw);

		return bRet;
	}

	BOOL setControlRect(Control* pControl, int tox=-1, int toy=-1, int toWidth=-1, int toHeight=-1, bool redraw = false, bool bFailOverWidth = true)
	{
		return setControlRect(pControl, Rect(tox, toy, toWidth, toHeight), redraw, bFailOverWidth);
	}

	void sweepControlColorAndStyle(Control* pControl)
	{
		Rect rect = pControl->getRect();
		sweepRectColorAndStyle(rect.X, rect.Y, rect.X + rect.nWidth, rect.Y + rect.nHeight, pControl);
	}

	//清除区域
	/* 获得给定的区域x1,y1,x2,y2下的每一点的颜色，并根据这些颜色进行刷新到每一个点
	 * pExclude - 排除的控件，一般为x1,y1,x2,y2这个区域所在的控件
	 * textColor bkColor style - 如果值为default，则获取此区域下顶层的除pExclude外的第一个控件作为清除参考；否则使用指定的参数进行清除
	 */
	void sweepRectColorAndStyle(int x1, int y1, int x2, int y2, Control* pExclude = NULL,
		COLOR textColor = color_default, COLOR bkColor = color_default, STYLE style = style_default)
	{
		for (int x = x1; x < x2; ++x)
		{
			for (int y = y1; y < y2; ++y)
			{
				COLOR atextColor = textColor;
				COLOR abkColor = bkColor;
				STYLE astyle = style;

				Control* pTopControl = getTopControl(x, y, pExclude);
				if (pTopControl)
				{
					if (atextColor != color_default || abkColor != color_default || astyle != style_default)
					{
						COLOR _bkColor; COLOR _textColor; STYLE _style;
						pTopControl->getPointColorAndStyle(x, y, _bkColor, _textColor, _style);
						if (atextColor == color_default)
							atextColor = _textColor;
						if (abkColor == color_default)
							abkColor = _bkColor;
						if (astyle == style_default)
							astyle = _style;
					}
				}
				else
				{
					if (atextColor == color_default)
						atextColor = getTextColor();
					if (abkColor == color_default)
						abkColor = getBkColor();
					if (astyle == style_default)
						astyle = getStyle();
				}

				//drawPoint(x, y, " ", textColor, bkColor, style);
				fillColorAndStyle(x, y, 1, atextColor, abkColor, astyle);
			}
		}
	}

	//重绘控件，只是重绘，不会更新位置信息
	BOOL redrawControl(Control* pControl, bool redrawTop = true, bool redrawBottom = true)
	{
		if (redrawBottom)
			redrawBottomControls(pControl);

		pControl->draw();

		if (redrawTop)
			redrawTopControls(pControl);

		return TRUE;
	}

	//刷新控件上面的控件
	BOOL redrawTopControls(Control* pControlBase)
	{
		Rect rect = pControlBase->getRect();
		float aMin[2];
		aMin[0] = rect.X;
		aMin[1] = rect.Y;
		float aMax[2];
		aMax[0] = rect.X + rect.nWidth - 1;
		aMax[1] = rect.Y + rect.nHeight - 1;
		std::set<Control*> setControls;
		m_rtRect2Control.Search(aMin, aMax, &setControls);
		setControls.erase(pControlBase);
		if (setControls.size() < 1)
			return FALSE;

		BOOL bRet = FALSE;
		std::vector<Control*> vctWillDraw;
		for (int i = m_vctControls.size() - 1; i >= 0; --i)
		{
			Control* pCtrl = m_vctControls[i];
			if (pCtrl == pControlBase)
				break;
			if (setControls.find(pCtrl) == setControls.end())
				continue;
			vctWillDraw.push_back(m_vctControls[i]);
		}
		for (int i = vctWillDraw.size() - 1; i >= 0; --i)
		{
			vctWillDraw[i]->draw();
			bRet = TRUE;
		}

		return bRet;
	}

	//刷新控件下面的控件
	BOOL redrawBottomControls(Control* pControlBase)
	{
		Rect rect = pControlBase->getRect();
		float aMin[2];
		aMin[0] = rect.X;
		aMin[1] = rect.Y;
		float aMax[2];
		aMax[0] = rect.X + rect.nWidth - 1;
		aMax[1] = rect.Y + rect.nHeight - 1;
		std::set<Control*> setControls;
		m_rtRect2Control.Search(aMin, aMax, &setControls);
		setControls.erase(pControlBase);
		if (setControls.size() < 1)
			return FALSE;

		BOOL bRet = FALSE;
		for (int i = 0; i < m_vctControls.size(); ++i)
		{
			Control* pCtrl = m_vctControls[i];
			if (pCtrl == pControlBase)
				break;
			if (setControls.find(pCtrl) == setControls.end())
				continue;

			m_vctControls[i]->draw();
			bRet = TRUE;
		}

		return bRet;
	}

	//重绘所有关联的控件
	BOOL redrawRelateRectControls(Control* pControlBase)
	{
		Rect rect = pControlBase->getRect();
		float aMin[2];
		aMin[0] = rect.X;
		aMin[1] = rect.Y;
		float aMax[2];
		aMax[0] = rect.X + rect.nWidth - 1;
		aMax[1] = rect.Y + rect.nHeight - 1;
		std::set<Control*> setControls;
		m_rtRect2Control.Search(aMin, aMax, &setControls);
		setControls.erase(pControlBase);
		if (setControls.size() < 1)
			return FALSE;

		BOOL bRet = FALSE;
		std::vector<Control*> vctWillDraw;
		for (int i = m_vctControls.size() - 1; i >= 0; --i)
		{
			Control* pCtrl = m_vctControls[i];
			if (setControls.find(pCtrl) == setControls.end())
				continue;
			vctWillDraw.push_back(m_vctControls[i]);
		}
		for (int i = vctWillDraw.size() - 1; i >= 0; --i)
		{
			vctWillDraw[i]->draw();
			bRet = TRUE;
		}

		return bRet;
	}

	void setControlVisible(Control* pControl, bool bVisible, bool redrawTop = true, bool redrawBottom = true)
	{
		pControl->setVisible(bVisible);
		redrawControl(pControl, redrawTop, redrawBottom);
	}

	//*****************************************************************************
	//*                                                                           *
	//*****************************************************************************
	void startLoopEvent()
	{
		m_loop = true;
		//init
		Event::INPUT_MODE oldMode = 0;
		GetConsoleMode(m_hIn, &oldMode);
		Event::INPUT_MODE newMode = oldMode;
		for (EVENT_MAP::iterator it = m_mapEvents.begin();
			it != m_mapEvents.end(); ++it)
		{
			//combine all input mode and build a new input mode
			newMode |= Event::GetInputModeByEventType(it->first);
		}
		//set new input mode
		SetConsoleMode(m_hIn, newMode);

		//loop events
		INPUT_RECORD input_record;
		DWORD events;
		EVENT_MAP::iterator itFinder;
		EVENT_MAP::iterator itEnd = m_mapEvents.end();
		for (; m_loop; )
		{
			//使用ReadConsoleInputA时不能获得中文，所有这里不使用ReadConsoleInput
			if (ReadConsoleInputW(m_hIn, &input_record, 1, &events) == FALSE)
			{
				continue;
			}

			itFinder = m_mapEvents.find(input_record.EventType);
			if (itFinder == itEnd)
			{
				continue;
			}

			EVENT_LIST &vctEvents = itFinder->second;
			int nSizeEvents = (int)vctEvents.size();
			for (int i = 0; i < nSizeEvents; ++i)
			{
				Event *pEvent = vctEvents[i];
				if (NULL == pEvent)
					continue;

				bool bExe = false;

				Control* pCtrl = dynamic_cast<Control*>(pEvent);
				if (NULL == pCtrl)
				{//此处为控制台直系事件，它们在任何时候都能触发
					bExe = true;
				}
				else
				{
					//if (input_record.Event.MouseEvent.dwButtonState != 0)
					//	int n = 0;

					//检测控件enable
					if (!pCtrl->isEnable())
						continue;

					if (pEvent->isGlobalEvent())
					{//如果是全局事件，则无论如何都执行
						bExe = true;
					}
					else
					{
						//控件鼠标事件只有在鼠标在控件上面时才触发
						if (MOUSE_EVENT == input_record.EventType
							&& (getControlAtPoint(
								input_record.Event.MouseEvent.dwMousePosition.X,
								input_record.Event.MouseEvent.dwMousePosition.Y) == pCtrl)
							)
						{
							bExe = true;
						}
						else if (KEY_EVENT == input_record.EventType && isControlActive(pCtrl))
						{//控件键盘事件只有在获得焦点时才触发
							bExe = true;
						}
					}
				}
				if (bExe)
				{
					pEvent->onBeginEvent(input_record);
					vctEvents[i]->onEvent(input_record);//execute
					pEvent->onEndEvent(input_record);
				}
			}
		}

		//reset input mode
		SetConsoleMode(m_hIn, oldMode);
	}

	void redraw()
	{
		clearScreen(m_textColor, m_bkColor, m_Style);
		for (int i = 0; i < m_vctControls.size(); ++i)
		{
			m_vctControls[i]->draw();
		}
	}

	void endLoopEvent()
	{
		m_loop = false;
	}

	void pushPosition(int x=-1, int y=-1)
	{
		COORD pt;
		if (x == -1 || y == -1)
		{
			pt = getCurPosition();
		}
		else
		{
			pt.X = x;
			pt.Y = y;
		}

		m_stackPostion.push_back(pt);
	}

	COORD popPosition()
	{
		COORD pt; pt.X = -1; pt.Y = -1;

		if (m_stackPostion.size() < 1)
			return pt;

		pt = m_stackPostion[m_stackPostion.size() - 1];
		m_stackPostion.pop_back();

		setCurPosition(pt.X, pt.Y);

		return pt;
	}

private:
	HANDLE		m_hOut;     /* Handle to the "screen" */
	HANDLE		m_hIn;      /* Handle to the "keyboard" */

	EVENT_MAP m_mapEvents;
	bool m_loop;

	std::vector<Control*> m_vctControls;
	RT::RTreeEx<Control*, float, 2> m_rtRect2Control;

	std::vector<COORD> m_stackPostion;
};

//Selectable Control
class ControlSelectable
	: public Control
{
public:
	ControlSelectable()
		: m_bCanSelectable(false)
	{

	}
	virtual ~ControlSelectable()
	{

	}

	void setSelectedTextColor(COLOR color)
	{
		m_colorSelected.setTextColor(color);
	}

	void setSelectedBkColor(COLOR color)
	{
		m_colorSelected.setBkColor(color);
	}

	void setSelectedStyle(STYLE style)
	{
		m_colorSelected.setStyle(style);
	}

	COLOR getSelectedTextColor()
	{
		return m_colorSelected.getTextColor();
	}

	COLOR getSelectedBkColor()
	{
		return m_colorSelected.getBkColor();
	}

	STYLE getSelectedStyle()
	{
		return m_colorSelected.getStyle();
	}

	virtual COLOR getDrawTextColor()
	{
		if (consoleUI()->isControlActive(this))
			return m_colorSelected.getTextColor();
		return getTextColor();
	}

	virtual COLOR getDrawBkColor()
	{
		if (consoleUI()->isControlActive(this))
			return m_colorSelected.getBkColor();
		return getBkColor();
	}

	virtual STYLE getDrawStyle()
	{
		if (consoleUI()->isControlActive(this))
			return m_colorSelected.getStyle();
		return getStyle();
	}

	virtual void getDrawColorAndStyle(COLOR& textColor, COLOR& bkColor, STYLE& style)
	{
		if (m_bCanSelectable && consoleUI()->isControlActive(this))
		{
			textColor = m_colorSelected.getTextColor();
			bkColor = m_colorSelected.getBkColor();
			style = m_colorSelected.getStyle();
		}
		else
		{
			textColor = getTextColor();
			bkColor = getBkColor();
			style = getStyle();
		}
	}

	void setSelectable(bool b)
	{
		m_bCanSelectable = b;
	}
	bool isSelectable()
	{
		return m_bCanSelectable;
	}

protected:
	ConsoleColor m_colorSelected;
	bool m_bCanSelectable;
};

｝NS_END

#endif CONSOLEAPP_H 