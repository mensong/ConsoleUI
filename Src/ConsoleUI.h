
#ifndef  CONSOLEAPP_H 
#define CONSOLEAPP_H 
#include <windows.h>
#include <vector>
#include <map>
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

//base class 
class Console
{
public:
	Console() {}
	virtual ~Console() {}

public:
	//Holder
	ConsoleUI *consoleUI() const;

	void setConsoleUI(ConsoleUI *pUI);

	void setID(const char* id);

	const std::string& getID() const;

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

	Event();
	virtual ~Event();

protected:
	//#define KEY_EVENT         0x0001 // Event contains key event record
	//#define MOUSE_EVENT       0x0002 // Event contains mouse event record
	//#define WINDOW_BUFFER_SIZE_EVENT 0x0004 // Event contains window change event record
	//#define MENU_EVENT 0x0008 // Event contains menu event record
	//#define FOCUS_EVENT 0x0010 // event contains focus change
	// use | combine them
	virtual Event::EVENT_TYPE eventType() = 0;

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
	static INPUT_MODE GetInputModeByEventType(Event::EVENT_TYPE eventType);

	//#define KEY_EVENT         0x0001 // Event contains key event record
	//#define MOUSE_EVENT       0x0002 // Event contains mouse event record
	//#define WINDOW_BUFFER_SIZE_EVENT 0x0004 // Event contains window change event record
	//#define MENU_EVENT 0x0008 // Event contains menu event record
	//#define FOCUS_EVENT 0x0010 // event contains focus change
	static std::vector<Event::EVENT_TYPE> getAllEventType();

	void setGlobalEvent(bool b);
	bool isGlobalEvent() const;

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

	tagRect(int x = 0, int y = 0, int width = 0, int height = 0);
	tagRect(const tagRect& rect);

	bool isIn(int nX, int nY) const;
} Rect;


//Console Color
class ConsoleColor
{
public:
	ConsoleColor(COLOR bkColor = color_default, COLOR textColor = color_default, STYLE style = style_default);

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
	Control();

	virtual ~Control() {}

public:
	//on initialize
	virtual bool onPreInitControl() { return true; }
	virtual void onInitControl() {}

	virtual bool draw() { return m_bVisible; }

	virtual Rect getRect() const { return m_rect; }

	inline const Rect& rect() const { return m_rect; }

	virtual void getPointColorAndStyle(int x, int y, COLOR& bkClolor, COLOR& textColor, STYLE& style) const;

	void setVisible(bool b) { m_bVisible = b; }

	bool isVisible() const { return m_bVisible; }

	void setEnable(bool b) { m_bEnable = b; }

	bool isEnable() const { return m_bEnable; }

protected:
	bool m_bVisible;
	bool m_bEnable;

private:
	friend class ConsoleUI;
	void setRect(const Rect& rect) { m_rect = rect; }
	void setRect(int x, int y, int nWidth, int nHeight);
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
	ConsoleUI(COLOR bkColor = color_default, COLOR textColor = color_default, STYLE style = style_default);

	~ConsoleUI();

	HANDLE getOutConsole() const { return m_hOut; }
	HANDLE getInConsole() const { return m_hIn; }

	void setTitle(const char* title);

	//获得屏模信息
	CONSOLE_SCREEN_BUFFER_INFO getConsoleScreenInfo() const;

	void setScreenBufferSize(int nWidth = -1, int nHeight = -1);

	WORD getAttributeByPoint(int x, int y);

	void getColorAndStyleByPoint(int x, int y, COLOR &textColor, COLOR& bkColor, STYLE& style);

	void setCurPosition(int x, int y);

	COORD getCurPosition() const;

	void setCurStyle(STYLE style);

	void setCurColor(COLOR textColor, COLOR bkColor);

	void getCurColor(COLOR &textColor, COLOR &bkColor) const;

	void getCurStyle(STYLE &style) const;

	void clearCurColor();

	void clearCurStyle();

	void fillColor(int x, int y, DWORD dwCellFillCount, COLOR textColor, COLOR bkColor);

	void fillColorAndStyle(int x, int y, DWORD dwCellFillCount,
		COLOR textColor = color_default, COLOR bkColor = color_default, STYLE style = style_default);

	void fillStyle(int x, int y, DWORD dwCellFillCount, STYLE style);

	void drawPoint(int x, int y, const char* txt, COLOR textColor = color_default,
		COLOR bkColor = color_default, STYLE style = style_default);

	void drawText(int x, int y, const char* txt);

	void drawTextW(int x, int y, const WCHAR* txt);

	void setBkColor(COLOR bkColor, bool redraw);

	void setTextColor(COLOR textColor, bool redraw);

	void setStyle(STYLE style, bool redraw);

	void clearScreen(COLOR textColor = color_default, COLOR bkColor = color_default,
		STYLE style = style_default, char cClearText = ' ');

	bool createBox(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
		COLOR textCol, COLOR bkcol, STYLE style = style_default);

	bool createBorder(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);

	//里面没有内容
	bool createBoxWithBorder(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
		COLOR textColor, COLOR boxColor, STYLE style = style_default);

	//*****************************************************************************
	//* registerEvent: These two reson case faild:                                *
	//*   1.pEvent is NULL                                                        *
	//*   2.Event::eventType() is invaild                                         *
	//*****************************************************************************
	BOOL addEvent(Event *pEvent);

	BOOL addControl(Control* pControl, bool bDraw = true, bool bInit = true);

	const std::vector<Control*>& getControls() const { return m_vctControls; }

	bool addControlRect(Control* pControl);

	bool removeControlRect(Control* pControl);

	bool updateControlRect(Control* pControl);

	Console* getConsoleById(const std::string& id);

	BOOL removeControl(Control* pControl);

	Control* getTopControl(int x, int y, Control* pExclude = NULL)  const;

	Control* getActiveControl() const;

	int getControlLayerId(Control* pCtrl);

	Control* getControlByLayerId(int nLayerId);

	bool isControlActive(Control* pCtrl) const;

	Control* getControlAtPoint(int x, int y) const;

	//如果rectTo中的属性为-1时不改变原值
	BOOL setControlRect(Control* pControl, const Rect& rectTo, bool redraw = false, bool bFailOverWidth = true);

	BOOL setControlRect(Control* pControl, int tox = -1, int toy = -1, int toWidth = -1, int toHeight = -1,
		bool redraw = false, bool bFailOverWidth = true);

	void sweepControlColorAndStyle(Control* pControl);

	//清除区域
	/* 获得给定的区域x1,y1,x2,y2下的每一点的颜色，并根据这些颜色进行刷新到每一个点
	 * pExclude - 排除的控件，一般为x1,y1,x2,y2这个区域所在的控件
	 * textColor bkColor style - 如果值为default，则获取此区域下顶层的除pExclude外的第一个控件作为清除参考；否则使用指定的参数进行清除
	 */
	void sweepRectColorAndStyle(int x1, int y1, int x2, int y2, Control* pExclude = NULL,
		COLOR textColor = color_default, COLOR bkColor = color_default, STYLE style = style_default);

	//重绘控件，只是重绘，不会更新位置信息
	BOOL redrawControl(Control* pControl, bool redrawTop = true, bool redrawBottom = true);

	//刷新控件上面的控件
	BOOL redrawTopControls(Control* pControlBase);

	//刷新控件下面的控件
	BOOL redrawBottomControls(Control* pControlBase);

	//重绘所有关联的控件
	BOOL redrawRelateRectControls(Control* pControlBase);

	void setControlVisible(Control* pControl, bool bVisible, bool redrawTop = true, bool redrawBottom = true);

	void startLoopEvent();

	void redraw();

	void endLoopEvent() { m_loop = false; }

	void pushPosition(int x = -1, int y = -1);

	COORD popPosition();

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
	ControlSelectable();
	virtual ~ControlSelectable() {}

	void setSelectedTextColor(COLOR color);

	void setSelectedBkColor(COLOR color);

	void setSelectedStyle(STYLE style);

	COLOR getSelectedTextColor();

	COLOR getSelectedBkColor();

	STYLE getSelectedStyle();

	virtual COLOR getDrawTextColor();

	virtual COLOR getDrawBkColor();

	virtual STYLE getDrawStyle();

	virtual void getDrawColorAndStyle(COLOR& textColor, COLOR& bkColor, STYLE& style);

	void setSelectable(bool b) { m_bCanSelectable = b; }
	bool isSelectable() { return m_bCanSelectable; }

protected:
	ConsoleColor m_colorSelected;
	bool m_bCanSelectable;
};

｝NS_END

#endif CONSOLEAPP_H 