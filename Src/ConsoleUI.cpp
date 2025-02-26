#include "ConsoleUI.h"
#include <conio.h>
#include <string>
#include <clocale>

NS_START｛

ConsoleUI * Console::consoleUI() const
{
	return m_pParent;
}



const std::string& GL::Console::getID() const
{
	return m_id;
}

void GL::Console::setID(const char* id)
{
	m_id = id;
}

void GL::Console::setConsoleUI(ConsoleUI *pUI)
{
	m_pParent = pUI;
}

Event::Event()
	: m_bGlobalEvent(false)
{

}

bool GL::Event::isGlobalEvent() const
{
	return m_bGlobalEvent;
}

void GL::Event::setGlobalEvent(bool b)
{
	m_bGlobalEvent = b;
}

std::vector<Event::EVENT_TYPE> GL::Event::getAllEventType()
{
	std::vector<Event::EVENT_TYPE> vctRet;
	vctRet.push_back(KEY_EVENT);
	vctRet.push_back(MOUSE_EVENT);
	vctRet.push_back(WINDOW_BUFFER_SIZE_EVENT);
	vctRet.push_back(MENU_EVENT);
	vctRet.push_back(FOCUS_EVENT);

	return vctRet;
}

GL::Event::INPUT_MODE GL::Event::GetInputModeByEventType(Event::EVENT_TYPE eventType)
{
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

GL::Event::~Event()
{

}


tagRect::tagRect(int x /*= 0*/, int y /*= 0*/, int width /*= 0*/, int height /*= 0*/)
{
	X = x;
	Y = y;
	nWidth = width;
	nHeight = height;
}


bool GL::tagRect::isIn(int nX, int nY) const
{
	return((nX >= X && nX < (X + nWidth)) && (nY >= Y && nY < (Y + nHeight)));
}

GL::tagRect::tagRect(const tagRect& rect)
{
	X = rect.X;
	Y = rect.Y;
	nWidth = rect.nWidth;
	nHeight = rect.nHeight;
}


ConsoleBrush::ConsoleBrush(COLOR bkColor /*= color_default*/, COLOR textColor /*= color_default*/, STYLE style /*= style_default*/)
	: m_bkColor(bkColor)
	, m_textColor(textColor)
	, m_Style(style)
{

}

Control::Control()
	: m_bVisible(true)
	, m_bEnable(true)
{
	m_rect.X = 0;
	m_rect.Y = 0;
	m_rect.nWidth = 1;
	m_rect.nHeight = 1;
}

void GL::Control::setRect(int x, int y, int nWidth, int nHeight)
{
	m_rect.X = x;
	m_rect.Y = y;
	m_rect.nWidth = nWidth;
	m_rect.nHeight = nHeight;
}

void GL::Control::getPointColorAndStyle(int x, int y, COLOR& bkClolor, COLOR& textColor, STYLE& style) const
{
	bkClolor = this->getBkColor();
	textColor = this->getTextColor();
	style = this->getStyle();
}


ConsoleUI::ConsoleUI(COLOR bkColor /*= color_default*/, COLOR textColor /*= color_default*/, STYLE style /*= style_default*/)
	: m_loop(true)
	, ConsoleBrush(bkColor, textColor, style)
{
	m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hIn = GetStdHandle(STD_INPUT_HANDLE);

	//关闭“快速编辑模式”以支持鼠标事件
	DWORD consoleMode = 0;
	if (GetConsoleMode(m_hIn, &consoleMode))
	{
		// 清除快速编辑模式标志
		consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
		SetConsoleMode(m_hIn, consoleMode);
	}

	if (m_textColor == color_default)
		m_textColor = white;
	if (m_bkColor == color_default)
		m_bkColor = black;
	if (m_Style == style_default)
		m_Style = style_default;

	clearScreen(m_textColor, m_bkColor, m_Style);
}


GL::ConsoleUI::~ConsoleUI()
{
	clearScreen();
}

COORD GL::ConsoleUI::popPosition()
{
	COORD pt; pt.X = -1; pt.Y = -1;

	if (m_stackPostion.size() < 1)
		return pt;

	pt = m_stackPostion[m_stackPostion.size() - 1];
	m_stackPostion.pop_back();

	setCurPosition(pt.X, pt.Y);

	return pt;
}

void GL::ConsoleUI::pushPosition(int x /*= -1*/, int y /*= -1*/)
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

void GL::ConsoleUI::redraw()
{
	clearScreen(m_textColor, m_bkColor, m_Style);
	for (int i = 0; i < m_vctControls.size(); ++i)
	{
		m_vctControls[i]->draw();
	}
}

void GL::ConsoleUI::startLoopEvent()
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

void GL::ConsoleUI::setControlVisible(Control* pControl, bool bVisible, bool redrawTop /*= true*/, bool redrawBottom /*= true*/)
{
	pControl->setVisible(bVisible);
	redrawControl(pControl, redrawTop, redrawBottom);
}

BOOL GL::ConsoleUI::redrawRelateRectControls(Control* pControlBase)
{
	Rect rect = pControlBase->getRect();
	float aMin[2];
	aMin[0] = (float)rect.X;
	aMin[1] = (float)rect.Y;
	float aMax[2];
	aMax[0] = (float)(rect.X + rect.nWidth - 1);
	aMax[1] = (float)(rect.Y + rect.nHeight - 1);
	std::set<Control*> setControls;
	m_rtRect2Control.Search(aMin, aMax, &setControls);
	setControls.erase(pControlBase);
	if (setControls.size() < 1)
		return FALSE;

	BOOL bRet = FALSE;
	std::vector<Control*> vctWillDraw;
	for (int i = (int)m_vctControls.size() - 1; i >= 0; --i)
	{
		Control* pCtrl = m_vctControls[i];
		if (setControls.find(pCtrl) == setControls.end())
			continue;
		vctWillDraw.push_back(m_vctControls[i]);
	}
	for (int i = (int)vctWillDraw.size() - 1; i >= 0; --i)
	{
		vctWillDraw[i]->draw();
		bRet = TRUE;
	}

	return bRet;
}

BOOL GL::ConsoleUI::redrawBottomControls(Control* pControlBase)
{
	Rect rect = pControlBase->getRect();
	float aMin[2];
	aMin[0] = (float)rect.X;
	aMin[1] = (float)rect.Y;
	float aMax[2];
	aMax[0] = (float)(rect.X + rect.nWidth - 1);
	aMax[1] = (float)(rect.Y + rect.nHeight - 1);
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

BOOL GL::ConsoleUI::redrawTopControls(Control* pControlBase)
{
	Rect rect = pControlBase->getRect();
	float aMin[2];
	aMin[0] = (float)rect.X;
	aMin[1] = (float)rect.Y;
	float aMax[2];
	aMax[0] = (float)(rect.X + rect.nWidth - 1);
	aMax[1] = (float)(rect.Y + rect.nHeight - 1);
	std::set<Control*> setControls;
	m_rtRect2Control.Search(aMin, aMax, &setControls);
	setControls.erase(pControlBase);
	if (setControls.size() < 1)
		return FALSE;

	BOOL bRet = FALSE;
	std::vector<Control*> vctWillDraw;
	for (int i = (int)m_vctControls.size() - 1; i >= 0; --i)
	{
		Control* pCtrl = m_vctControls[i];
		if (pCtrl == pControlBase)
			break;
		if (setControls.find(pCtrl) == setControls.end())
			continue;
		vctWillDraw.push_back(m_vctControls[i]);
	}
	for (int i = (int)vctWillDraw.size() - 1; i >= 0; --i)
	{
		vctWillDraw[i]->draw();
		bRet = TRUE;
	}

	return bRet;
}

BOOL GL::ConsoleUI::redrawControl(Control* pControl, bool redrawTop /*= true*/, bool redrawBottom /*= true*/)
{
	if (redrawBottom)
		redrawBottomControls(pControl);

	pControl->draw();

	if (redrawTop)
		redrawTopControls(pControl);

	return TRUE;
}

void GL::ConsoleUI::sweepRectColorAndStyle(int x1, int y1, int x2, int y2, Control* pExclude /*= NULL*/,
	COLOR textColor /*= color_default*/, COLOR bkColor /*= color_default*/, STYLE style /*= style_default*/)
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

void GL::ConsoleUI::sweepControlColorAndStyle(Control* pControl)
{
	Rect rect = pControl->getRect();
	sweepRectColorAndStyle(rect.X, rect.Y, rect.X + rect.nWidth, rect.Y + rect.nHeight, pControl);
}

void ConsoleUI::sweepRectText(int x1, int y1, int x2, int y2)
{
	for (int x = x1; x < x2; ++x)
	{
		for (int y = y1; y < y2; ++y)
		{
			this->drawText(x, y, " ");
		}
	}
}

void ConsoleUI::sweepControlText(Control* pControl)
{
	Rect rect = pControl->getRect();
	sweepRectText(rect.X, rect.Y, rect.X + rect.nWidth, rect.Y + rect.nHeight);
}

void ConsoleUI::sweepRectAll(int x1, int y1, int x2, int y2, Control* pExclude, COLOR textColor, COLOR bkColor, STYLE style)
{
	sweepRectText(x1, y1, x2, y2);
	sweepRectColorAndStyle(x1, y1, x2, y2, pExclude, textColor, bkColor, style);
}

void ConsoleUI::sweepControlAll(Control* pControl)
{
	sweepControlText(pControl);
	sweepControlColorAndStyle(pControl);
}

BOOL GL::ConsoleUI::setControlRect(Control* pControl, int tox /*= -1*/, int toy /*= -1*/, int toWidth /*= -1*/, int toHeight /*= -1*/,
	bool redraw /*= false*/, bool bFailOverWidth /*= true*/)
{
	return setControlRect(pControl, Rect(tox, toy, toWidth, toHeight), redraw, bFailOverWidth);
}

BOOL GL::ConsoleUI::setControlRect(Control* pControl, const Rect& rectTo, bool redraw /*= false*/, bool bFailOverWidth /*= true*/)
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

Control* GL::ConsoleUI::getControlAtPoint(int x, int y) const
{
	return getTopControl(x, y);
}

bool GL::ConsoleUI::isControlActive(Control* pCtrl) const
{
	return getActiveControl() == pCtrl;
}

Control* GL::ConsoleUI::getControlByLayerId(int nLayerId)
{
	if (0 > nLayerId || nLayerId >= m_vctControls.size())
		return NULL;
	return m_vctControls[nLayerId];
}

int GL::ConsoleUI::getControlLayerId(Control* pCtrl)
{
	std::vector<Control*>::iterator itFinder = std::find(m_vctControls.begin(), m_vctControls.end(), pCtrl);
	if (itFinder == m_vctControls.end())
		return -1;
	size_t nLayer = itFinder - m_vctControls.begin();
	if (nLayer < m_vctControls.size())
		return (int)nLayer;
	return -1;
}

Control* GL::ConsoleUI::getActiveControl() const
{
	COORD pt = getCurPosition();
	return getTopControl(pt.X, pt.Y);
}

Control* GL::ConsoleUI::getTopControl(int x, int y, Control* pExclude /*= NULL*/) const
{
	float point[2] = { (float)x, (float)y };
	std::set<Control*> setControls;
	m_rtRect2Control.Search(point, point, &setControls);

	//找到顶层开始查找
	for (int i = (int)m_vctControls.size() - 1; i >= 0; --i)
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

BOOL GL::ConsoleUI::removeControl(Control* pControl)
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
	float aMin[2] = { (float)rect.X, (float)rect.Y };
	float aMax[2] = { (float)(rect.X + rect.nWidth/*-1*/), (float)(rect.Y + rect.nHeight/*-1*/) };
	bRet2 = m_rtRect2Control.Remove(aMin, aMax, pControl) ? TRUE : FALSE;

	return bRet1 && bRet2;
}

Console* GL::ConsoleUI::getConsoleById(const std::string& id)
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

bool GL::ConsoleUI::updateControlRect(Control* pControl)
{
	removeControlRect(pControl);
	return addControlRect(pControl);
}

bool GL::ConsoleUI::removeControlRect(Control* pControl)
{
	Rect rect = pControl->getRect();
	float aMin[2] = { (float)rect.X, (float)rect.Y };
	float aMax[2] = { (float)(rect.X + rect.nWidth), (float)(rect.Y + rect.nHeight) };
	return m_rtRect2Control.Remove(aMin, aMax, pControl);
}

bool GL::ConsoleUI::addControlRect(Control* pControl)
{
	Rect rect = pControl->getRect();
	float aMin[2] = { (float)rect.X, (float)rect.Y };
	float aMax[2] = { (float)(rect.X + rect.nWidth - 1), (float)(rect.Y + rect.nHeight - 1) };
	return m_rtRect2Control.Insert(aMin, aMax, pControl);
}

BOOL GL::ConsoleUI::addControl(Control* pControl, bool bDraw /*= true*/, bool bInit /*= true*/)
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

BOOL GL::ConsoleUI::addEvent(Event *pEvent)
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

bool GL::ConsoleUI::createBoxWithBorder(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
	COLOR textColor, COLOR boxColor, STYLE style /*= style_default*/)
{
	if (x1 >= x2 || y1 >= y2)
	{
		return false;
	}

	createBox(x1, y1, x2, y2, textColor, boxColor, style);
	createBorder(x1, y1, x2, y2);

	return true;
}

bool GL::ConsoleUI::createBorder(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
	if (x1 >= x2 || y1 >= y2)
	{
		return false;
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
			return true;
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
		return true;
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
		return true;
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

	return true;
}

bool GL::ConsoleUI::createBox(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
	COLOR textCol, COLOR bkcol, STYLE style /*= style_default*/)
{
	if (x1 >= x2 || y1 >= y2)
		return false;

	int /*x,*/ y;
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

	return true;
}

void GL::ConsoleUI::clearScreen(COLOR textColor /*= color_default*/, COLOR bkColor /*= color_default*/,
	STYLE style /*= style_default*/, char cClearText /*= ' '*/)
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

void GL::ConsoleUI::setStyle(STYLE style, bool redraw)
{
	m_Style = style;
	if (redraw)
		clearScreen(m_textColor, m_bkColor, m_Style);
}

void GL::ConsoleUI::setTextColor(COLOR textColor, bool redraw)
{
	m_textColor = textColor;
	if (redraw)
		clearScreen(m_textColor, m_bkColor, m_Style);
}

void GL::ConsoleUI::setBkColor(COLOR bkColor, bool redraw)
{
	m_bkColor = bkColor;
	if (redraw)
		clearScreen(m_textColor, m_bkColor, m_Style);
}

void GL::ConsoleUI::drawTextW(int x, int y, const WCHAR* txt)
{
	setlocale(LC_ALL, "chs");
	setCurPosition(x, y);
	wprintf(txt);
}

void GL::ConsoleUI::drawText(int x, int y, const char* txt)
{
	setCurPosition(x, y);
	printf(txt);
}

void GL::ConsoleUI::drawPoint(int x, int y, const char* txt, COLOR textColor /*= color_default*/,
	COLOR bkColor /*= color_default*/, STYLE style /*= style_default*/)
{
	setCurColor(textColor, bkColor);
	setCurStyle(style);

	setCurPosition(x, y);
	_cprintf(txt);
}

void GL::ConsoleUI::fillStyle(int x, int y, DWORD dwCellFillCount, STYLE style)
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

void GL::ConsoleUI::fillColorAndStyle(int x, int y, DWORD dwCellFillCount, COLOR textColor /*= color_default*/,
	COLOR bkColor /*= color_default*/, STYLE style /*= style_default*/)
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

void GL::ConsoleUI::fillColor(int x, int y, DWORD dwCellFillCount, COLOR textColor, COLOR bkColor)
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

void GL::ConsoleUI::clearCurStyle()
{
	setCurStyle(style_default);
}

void GL::ConsoleUI::clearCurColor()
{
	setCurColor(color_default, color_default);
}

void GL::ConsoleUI::getCurStyle(STYLE &style) const
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
	{
		WORD wStyle = csbiInfo.wAttributes & 0xffff00;//clear color bit
		style = (STYLE)wStyle;
	}
}

void GL::ConsoleUI::getCurColor(COLOR &textColor, COLOR &bkColor) const
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	if (GetConsoleScreenBufferInfo(m_hOut, &csbiInfo))
	{
		WORD wColor = csbiInfo.wAttributes & 0x00ff;//clear style bit
		textColor = (COLOR)(wColor & 0x0f);
		bkColor = (COLOR)(wColor & 0xf0);
	}
}

void GL::ConsoleUI::setCurColor(COLOR textColor, COLOR bkColor)
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

void GL::ConsoleUI::setCurStyle(STYLE style)
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

COORD GL::ConsoleUI::getCurPosition() const
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(m_hOut, &csbi);
	return csbi.dwCursorPosition;
}

void GL::ConsoleUI::setCurPosition(int x, int y)
{
	COORD coord;
	coord.X = x; coord.Y = y;
	SetConsoleCursorPosition(m_hOut, coord);
	return;
}

void GL::ConsoleUI::getColorAndStyleByPoint(int x, int y, COLOR &textColor, COLOR& bkColor, STYLE& style)
{
	WORD wAttrib = getAttributeByPoint(x, y);
	COLOR allColor = (COLOR)(wAttrib & 0x00ff);//clear style bit
	textColor = (COLOR)(allColor & 0x0f);
	bkColor = (COLOR)((allColor & 0xf0) >> 4);
	style = (STYLE)((wAttrib & 0xff0000) >> 8);//clear color bit
}

WORD GL::ConsoleUI::getAttributeByPoint(int x, int y)
{
	WORD wAttrib = 0;
	DWORD dwReaded = 0;
	COORD pt;
	pt.X = x;
	pt.Y = y;
	ReadConsoleOutputAttribute(m_hOut, &wAttrib, 1, pt, &dwReaded);

	return wAttrib;
}

void GL::ConsoleUI::setScreenBufferSize(int nWidth /*= -1*/, int nHeight /*= -1*/)
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

CONSOLE_SCREEN_BUFFER_INFO GL::ConsoleUI::getConsoleScreenInfo() const
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(m_hOut, &csbi);
	return csbi;
}

void GL::ConsoleUI::setTitle(const char* title)
{
	SetConsoleTitleA(title);
}

ControlSelectable::ControlSelectable()
	: m_bCanSelectable(false)
{

}


void GL::ControlSelectable::getDrawColorAndStyle(COLOR& textColor, COLOR& bkColor, STYLE& style)
{
	if (m_bCanSelectable && consoleUI()->isControlActive(this))
	{
		textColor = m_brushSelected.getTextColor();
		bkColor = m_brushSelected.getBkColor();
		style = m_brushSelected.getStyle();
	}
	else
	{
		textColor = getTextColor();
		bkColor = getBkColor();
		style = getStyle();
	}
}

GL::STYLE GL::ControlSelectable::getDrawStyle()
{
	if (consoleUI()->isControlActive(this))
		return m_brushSelected.getStyle();
	return getStyle();
}

GL::COLOR GL::ControlSelectable::getDrawBkColor()
{
	if (consoleUI()->isControlActive(this))
		return m_brushSelected.getBkColor();
	return getBkColor();
}

GL::COLOR GL::ControlSelectable::getDrawTextColor()
{
	if (consoleUI()->isControlActive(this))
		return m_brushSelected.getTextColor();
	return getTextColor();
}

GL::STYLE GL::ControlSelectable::getSelectedStyle()
{
	return m_brushSelected.getStyle();
}

GL::COLOR GL::ControlSelectable::getSelectedBkColor()
{
	return m_brushSelected.getBkColor();
}

GL::COLOR GL::ControlSelectable::getSelectedTextColor()
{
	return m_brushSelected.getTextColor();
}

void GL::ControlSelectable::setSelectedStyle(STYLE style)
{
	m_brushSelected.setStyle(style);
}

void GL::ControlSelectable::setSelectedBkColor(COLOR color)
{
	m_brushSelected.setBkColor(color);
}

void GL::ControlSelectable::setSelectedTextColor(COLOR color)
{
	m_brushSelected.setTextColor(color);
}

｝NS_END