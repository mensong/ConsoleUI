
/**
 * @file    ConsoleComCtrl.h
 * @brief  	命令行控件
 * @author 	LMS
 * @date   	7/12/2014
 * ------------------------------------------------------------
 * 版本历史       注释                日期
 * ------------------------------------------------------------
 * @version v1.0  初始版本            7/12/2014
 * ------------------------------------------------------------
 * Note:
 */

#pragma once
#include "ConsoleUI.h"
#include <functional>
#include <ctype.h>
#include <Convertor.h>
#include "string_utility.h"
#include "ConsoleComEvent.h"

#define BEGIN_DRAW consoleUI()->pushPosition();do{
#define END_DRAW }while(0);consoleUI()->popPosition()
#define BREAK_DRAW break

NS_START｛

static std::string CutTooLongString(const std::string& s, int maxWidth)
{
	//为什么不用直接的字符串截取呢？是因为怕有汉字的时候，汉字有可能被截断
	std::string sValidText;
	if (s.length() > maxWidth)
	{//内容多于可显示的area，则截取内容以便显示
		size_t startZh = ZH_origIdxToZhIdx(s, maxWidth);
		sValidText = ZH_subString(s, 0, startZh);
		if (sValidText.length() > maxWidth && GL::ZH_isZHChar(sValidText[sValidText.length() - 1]))
			sValidText[sValidText.length() - 2] = '\0';//防止汉字的半块飞出
	}
	else
	{//显示全部内容
		sValidText = s;
	}

	return sValidText;
}

//Console Plane
class ConsolePlane
	: public EventMouseFollow
	, public ControlSelectable
{
public:
	ConsolePlane(ConsoleUI *consoleUI = NULL)
		: m_bHasBorder(false)
	{
		setConsoleUI(consoleUI);
	}

	virtual ~ConsolePlane()
	{

	}

public:
	virtual void onInitControl()
	{

	}

	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			COLOR textColor = color_default;
			COLOR bkColor = color_default;
			STYLE style = style_default;
			getDrawColorAndStyle(textColor, bkColor, style);
			consoleUI()->createBox(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight, textColor, bkColor, style);

			if (m_bHasBorder)
				consoleUI()->createBorder(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight);

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

	void setHasBorder(bool b)
	{
		m_bHasBorder = b;
	}

	bool isBasBorder()
	{
		return m_bHasBorder;
	}

private:
	bool m_bHasBorder;
};


//button
typedef std::function<void(void)> FN_ON_CLICKED;
typedef std::function<void(void)> FN_ON_RESIZE;
class ConsoleButton
	: public Event
	, public ControlSelectable
{
public:
	ConsoleButton(ConsoleUI *consoleUI = NULL)
	{
		setConsoleUI(consoleUI);

		m_textColor = white;
		m_bkColor = gray;
		m_Style = style_default;

		m_textClickColor = m_textColor;
		m_bkClickColor = m_bkColor;
		m_ClickStyle = (STYLE)(background_blue);
	}
	virtual ~ConsoleButton() { }

	void setCaption(const std::string &sCaption)
	{
		m_sCaption = sCaption;
	}

	void setClickedEvent(FN_ON_CLICKED ev)
	{
		m_fnOnClicked = ev;
	}

	void setResizeEvent(FN_ON_RESIZE ev)
	{
		m_fnOnResize = ev;
	}

public:
	virtual void onInitControl()
	{

	}

	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			COLOR textColor = color_default;
			COLOR bkColor = color_default;
			STYLE style = style_default;
			getDrawColorAndStyle(textColor, bkColor, style);
			draw(textColor, bkColor, style);

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

	void draw(COLOR textColor, COLOR bkColor, STYLE style)
	{
		//draw box
		consoleUI()->createBox(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight, textColor, bkColor, style);

		std::string sDrawText = CutTooLongString(m_sCaption, rect().nWidth);

		int leftOffset = 0;
		leftOffset = (rect().nWidth - sDrawText.length()) / 2;
		int topOffset = 0;
		topOffset = rect().nHeight / 2;

		//必须清除旧的内容,否则在当前绘制的内容比旧的内容短时，会残留旧的内容
		char *pSpace = new char[rect().nWidth + 1];
		memset(pSpace, ' ', rect().nWidth);
		pSpace[rect().nWidth] = '\0';
		consoleUI()->drawText(rect().X, rect().Y, pSpace);
		delete[] pSpace;

		//绘制内容
		consoleUI()->drawText(rect().X + leftOffset, rect().Y + topOffset, sDrawText.c_str());

		consoleUI()->createBorder(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight);
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return MOUSE_EVENT | WINDOW_BUFFER_SIZE_EVENT;
	}

	virtual void onInitEvent()
	{

	}

	//on event
	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (MOUSE_EVENT == input_record.EventType)
		{
			if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				int nX = input_record.Event.MouseEvent.dwMousePosition.X;
				int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
				//if (parent()->getTopControl(nX, nY) == this)
				{
					//为了实现jig效果
					draw(m_textClickColor, m_bkClickColor, m_ClickStyle);
					Sleep(200);
					draw();

					//设置焦点到本控件
					consoleUI()->setCurPosition(nX, nY);

					//call onCommand
					onClicked();
				}
			}

		}
		else if (WINDOW_BUFFER_SIZE_EVENT == input_record.EventType)
		{
			if (input_record.Event.KeyEvent.wRepeatCount != 0)
			{
				onResize();
			}
		}
	}

public:	//supernumerary event function
	//on clicked
	virtual void onClicked()
	{
		if (m_fnOnClicked)
			m_fnOnClicked();
	}
	//on resize
	virtual void onResize()
	{
		if (m_fnOnResize)
			m_fnOnResize();
	}

private:
	std::string m_sCaption;

	FN_ON_CLICKED m_fnOnClicked;
	FN_ON_RESIZE m_fnOnResize;

	COLOR m_bkClickColor;
	COLOR m_textClickColor;
	STYLE m_ClickStyle;
};


//Console Label
class ConsoleLabel
	: public Event
	, public ControlSelectable
{
public:
	enum ALIGN
	{
		left,
		center,
		right
	};

public:
	ConsoleLabel(ConsoleUI *consoleUI = NULL)
		: m_bTransparent(true)
		, m_align(left)
		, m_bAutoWidth(false)
	{
		setConsoleUI(consoleUI);

		m_bkColor = white;

	}

	virtual ~ConsoleLabel()
	{

	}

public:
	virtual void onInitControl()
	{

	}

	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			COLOR textColor = color_default;
			COLOR bkColor = color_default;
			STYLE style = style_default;
			getDrawColorAndStyle(textColor, bkColor, style);

			if (m_bAutoWidth && rect().nWidth != m_content.length())
				consoleUI()->setControlRect(this, -1, -1, m_content.length(), -1, false);

			if (textColor == color_default)
			{
				if (m_bTransparent)
					textColor = white;
				else
					textColor = black;
			}

			std::string sDrawText = CutTooLongString(m_content, rect().nWidth);

			int leftOffset = 0;
			if (m_align == left)
				leftOffset = 0;
			else if (m_align == center)
				leftOffset = (rect().nWidth - sDrawText.length()) / 2;
			else if (m_align == right)
				leftOffset = rect().nWidth - sDrawText.length();

			//必须清除旧的内容,否则在当前绘制的内容比旧的内容短时，会残留旧的内容
			char *pSpace = new char[rect().nWidth + 1];
			memset(pSpace, ' ', rect().nWidth);
			pSpace[rect().nWidth] = '\0';
			consoleUI()->drawText(rect().X, rect().Y, pSpace);
			delete[] pSpace;

			//绘制内容
			consoleUI()->drawText(rect().X + leftOffset, rect().Y, sDrawText.c_str());

			if (m_bTransparent)
				consoleUI()->sweepRectColorAndStyle(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight, this, textColor);
			else
				consoleUI()->fillColorAndStyle(rect().X, rect().Y, rect().nWidth, textColor, bkColor, style);

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

public:
	void setContent(const char* content)
	{
		m_content = content;
	}
	std::string getContent() const
	{
		return m_content;
	}

	void setPosition(int x, int y)
	{
		consoleUI()->setControlRect(this, x, y, -1, -1, false);
	}

	void setWidth(int nWidth)
	{
		//rect().nWidth = nWidth;
		consoleUI()->setControlRect(this, -1, -1, nWidth, -1, false);
	}
	int getWidth() const
	{
		return rect().nWidth;
	}

	void setTextColor(COLOR textColor)
	{
		m_textColor = textColor;
	}
	COLOR getTextColor() const
	{
		return m_textColor;
	}

	void setAlign(ALIGN align)
	{
		m_align = align;
	}
	ALIGN getAlign() const
	{
		return m_align;
	}

	void setTransparent(bool b)
	{
		m_bTransparent = b;
	}
	bool isTransparent()
	{
		return m_bTransparent;
	}

	void setAutoWidth(bool b)
	{
		m_bAutoWidth = b;
	}
	bool isAutoWidth()
	{
		return m_bAutoWidth;
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return MOUSE_EVENT | KEY_EVENT;
	}

	virtual void onInitEvent()
	{

	}

	//on event
	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (MOUSE_EVENT == input_record.EventType)
		{
			if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				int nX = input_record.Event.MouseEvent.dwMousePosition.X;
				int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
				//if (parent()->getTopControl(nX, nY) == this)
				{
					//设置焦点到本控件
					consoleUI()->setCurPosition(nX, nY);
				}
			}

		}
		else if (KEY_EVENT == input_record.EventType)
		{
			COORD pt = consoleUI()->getCurPosition();

			if (input_record.Event.KeyEvent.bKeyDown)
			{
				switch (input_record.Event.KeyEvent.wVirtualKeyCode)
				{
				}
			}
		}
	}

private:
	std::string m_content;
	bool m_bTransparent;

	ALIGN m_align;
	bool m_bAutoWidth;
};


//Check box
typedef std::function<void(void)> FN_ON_CHECKED;
class ConsoleCheckBox
	: public Event
	, public ControlSelectable
{
public:
	ConsoleCheckBox(ConsoleUI *consoleUI = NULL)
		: m_bChecked(true)
		, m_bAutoWidth(false)
		, m_bTransparent(false)
	{
		setConsoleUI(consoleUI);

		consoleUI->setControlRect(this, -1, -1, 2, 1, false);

		m_textColor = white;
		m_bkColor = gray;
	}

	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			COLOR textColor = color_default;
			COLOR bkColor = color_default;
			STYLE style = style_default;
			getDrawColorAndStyle(textColor, bkColor, style);

			if (m_bAutoWidth && rect().nWidth != m_sContent.length() + 2)
				consoleUI()->setControlRect(this, -1, -1, m_sContent.length() + 2, -1, false);

			//draw box 
			consoleUI()->createBox(rect().X, rect().Y, rect().X + 2, rect().Y + rect().nHeight, textColor, bkColor, style);
			if (m_bChecked)
				consoleUI()->drawText(rect().X, rect().Y, "√");
			else
				consoleUI()->drawText(rect().X, rect().Y, "  ");
			consoleUI()->createBorder(rect().X, rect().Y, rect().X + 2, rect().Y + rect().nHeight);

			//draw text
			if (rect().nWidth > 2)
			{
				std::string sDrawText = CutTooLongString(m_sContent, rect().nWidth - 2);
				consoleUI()->drawText(rect().X + 2, rect().Y, sDrawText.c_str());

				if (m_bTransparent)
					consoleUI()->sweepRectColorAndStyle(rect().X + 2, rect().Y, rect().X + 2 + rect().nWidth - 2, rect().Y + rect().nHeight, this);
				else
					consoleUI()->fillColorAndStyle(rect().X + 2, rect().Y, rect().nWidth - 2, textColor, bkColor, style);
			}

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

	void setPosition(int x, int y)
	{
		consoleUI()->setControlRect(this, x, y, -1, -1, false);
	}

	virtual void setCheck(bool b)
	{
		m_bChecked = b;
	}
	bool isChecked() const
	{
		return m_bChecked;
	}

	void setContent(const char* txt)
	{
		m_sContent = txt;
	}
	std::string getContent() const
	{
		return m_sContent;
	}

	void setCheckEvent(FN_ON_CHECKED ev)
	{
		m_fnOnChecked = ev;
	}

	void setWidth(int n)
	{
		consoleUI()->setControlRect(this, -1, -1, n, -1, false);
	}

	void setAutoWidth(bool b)
	{
		m_bAutoWidth = b;
	}
	bool isAutoWidth() const
	{
		return m_bAutoWidth;
	}

	void setTransparent(bool b)
	{
		m_bTransparent = b;
	}
	bool isTransparent()
	{
		return m_bTransparent;
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return KEY_EVENT | MOUSE_EVENT;
	}

	virtual void onInitEvent()
	{

	}

	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (input_record.EventType == KEY_EVENT)
		{
			if (!input_record.Event.KeyEvent.bKeyDown)
			{
				return;
			}

			WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);
			switch (k)
			{
				
			}
		}
		else if (MOUSE_EVENT == input_record.EventType)
		{
			if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				int nX = input_record.Event.MouseEvent.dwMousePosition.X;
				int nY = input_record.Event.MouseEvent.dwMousePosition.Y;

				setCheck(!isChecked());
				consoleUI()->redrawControl(this, false, false);

				onCheck();
			}
		}
	}

public:
	virtual void onCheck()
	{
		if (m_fnOnChecked)
			m_fnOnChecked();
	}

protected:
	bool m_bChecked;
	std::string m_sContent;

	FN_ON_CHECKED m_fnOnChecked;
	bool m_bAutoWidth;
	bool m_bTransparent;

};


//Check box
typedef std::function<void(void)> FN_ON_CHECKED;
class ConsoleRadioBox
	: public ConsoleCheckBox
{
public:
	class Group
	{
	public:
		Group() 
		{
		}

		void addRadioBox(ConsoleRadioBox* pRadioBox)
		{
			pRadioBox->setGroup(this);

			m_vctRadioBoxs.push_back(pRadioBox);

			if (m_vctRadioBoxs.size() == 1)
			{
				setCheck(pRadioBox);
			}
		}

		void removeRadioBox(ConsoleRadioBox* pRadioBox)
		{
			for (int i = 0; i < m_vctRadioBoxs.size(); ++i)
			{
				if (m_vctRadioBoxs[i] == pRadioBox)
				{
					m_vctRadioBoxs.erase(m_vctRadioBoxs.begin() + i);
					return;
				}
			}
		}
		void removeRadioBox(int n)
		{
			for (int i = 0; i < m_vctRadioBoxs.size(); ++i)
			{
				if (i == n)
				{
					m_vctRadioBoxs.erase(m_vctRadioBoxs.begin() + i);
					return;
				}
			}
		}

		int getCheckedIdx()
		{
			for (int i = 0; i < m_vctRadioBoxs.size(); ++i)
			{
				if (m_vctRadioBoxs[i]->isChecked())
					return i;
			}

			return -1;
		}

		ConsoleRadioBox* getCheckedRadioBox()
		{
			return m_vctRadioBoxs[getCheckedIdx()];
		}

		void setCheck(int n)
		{
			for (int i = 0; i < m_vctRadioBoxs.size(); ++i)
			{
				if (i == n)
					m_vctRadioBoxs[i]->m_bChecked = true;
				else
					m_vctRadioBoxs[i]->m_bChecked = false;

				m_vctRadioBoxs[i]->draw();
			}
		}

		void setCheck(ConsoleRadioBox* pRadioBox)
		{
			for (int i = 0; i < m_vctRadioBoxs.size(); ++i)
			{
				if (m_vctRadioBoxs[i] == pRadioBox)
					m_vctRadioBoxs[i]->m_bChecked = true;
				else
					m_vctRadioBoxs[i]->m_bChecked = false;

				m_vctRadioBoxs[i]->draw();
			}
		}

	private:
		std::vector<ConsoleRadioBox*> m_vctRadioBoxs;
	};

public:
	ConsoleRadioBox(ConsoleUI *consoleUI = NULL, Group* pGroup=NULL)
		: ConsoleCheckBox(consoleUI)
		, m_pGroup(pGroup)
	{
		setConsoleUI(consoleUI);
		m_bChecked = false;
	}

	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			COLOR textColor = color_default;
			COLOR bkColor = color_default;
			STYLE style = style_default;
			getDrawColorAndStyle(textColor, bkColor, style);

			if (m_bAutoWidth && rect().nWidth != m_sContent.length() + 2)
				consoleUI()->setControlRect(this, -1, -1, m_sContent.length() + 2, -1, false);

			//draw box 
			consoleUI()->createBox(rect().X, rect().Y, rect().X + 2, rect().Y + rect().nHeight, textColor, bkColor, style);
			if (m_bChecked)
				consoleUI()->drawText(rect().X, rect().Y, "◆");
			else
				consoleUI()->drawText(rect().X, rect().Y, "  ");
			consoleUI()->createBorder(rect().X, rect().Y, rect().X + 2, rect().Y + rect().nHeight);

			//draw text
			if (rect().nWidth > 2)
			{
				std::string sDrawText = CutTooLongString(m_sContent, rect().nWidth - 2);
				consoleUI()->drawText(rect().X + 2, rect().Y, sDrawText.c_str());

				if (m_bTransparent)
					consoleUI()->sweepRectColorAndStyle(rect().X + 2, rect().Y, rect().X + 2 + rect().nWidth - 2, rect().Y + rect().nHeight, this);
				else
					consoleUI()->fillColorAndStyle(rect().X + 2, rect().Y, rect().nWidth - 2, textColor, bkColor, style);
			}

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

	virtual void setCheck(bool b)
	{
		if (NULL == m_pGroup)
		{
			m_bChecked = b;
		}
		else
		{
			m_pGroup->setCheck(this);
		}
	}

	void setGroup(ConsoleRadioBox::Group* pGroup)
	{
		m_pGroup = pGroup;
	}
	ConsoleRadioBox::Group* getGroup() 
	{
		return m_pGroup;
	}

private:
	ConsoleRadioBox::Group* m_pGroup;
};


//ConsoleListBox
class ConsoleListBox
	: public Event
	, public Control
{
public:
	class ListBoxButton
		: public ConsoleButton
	{
	public:
		ListBoxButton(ConsoleUI *consoleUI, ConsoleListBox* pListBox)
			: ConsoleButton(consoleUI)
			, m_pListBox(pListBox)
		{

		}
	protected:
		virtual EVENT_TYPE eventType()
		{
			return ConsoleButton::eventType() | KEY_EVENT;
		}

		//on event
		virtual void onEvent(INPUT_RECORD &input_record)
		{
			if (KEY_EVENT == input_record.EventType)
			{
				//在按钮激活的情况下也能按上下键切换列表item
				if (m_pListBox)
					m_pListBox->onEvent(input_record);
			}
			else
			{
				ConsoleButton::onEvent(input_record);
			}
		}

	private:
		ConsoleListBox* m_pListBox;
	};

public:
	ConsoleListBox(ConsoleUI *consoleUI = NULL)
		: m_nIdx(5)
		, m_nVisibleStart(3)
		, borderWidth(1)
		, m_btPrePage(consoleUI, this)
		, m_btNextPage(consoleUI, this)
		, m_btPreItem(consoleUI, this)
		, m_btNextItem(consoleUI, this)
	{
		setConsoleUI(consoleUI);

		setBkColor(white);
		setTextColor(black);

		m_broderColor.setBkColor(gray);
		m_broderColor.setTextColor(color_default);
		m_broderColor.setStyle(style_default);

		m_selectedItemColor.setBkColor(blue);
		m_selectedItemColor.setTextColor(color_default);
		m_selectedItemColor.setStyle((STYLE)(common_lvb_grid_horizontal | common_lvb_underscore));
	}

	int getContentStartX() const
	{
		return rect().X + borderWidth * 2;
	}
	int getContentEndX() const
	{
		return rect().X + rect().nWidth - borderWidth * 2;
	}
	int getContentStartY() const
	{
		return rect().Y + borderWidth;
	}
	int getContentEndY() const
	{
		return rect().Y + rect().nHeight - borderWidth;
	}
	int getContentLines() const
	{
		return getContentEndY() - getContentStartY();
	}

	virtual void onInitControl()
	{
		int nButtonCount = 4;
		int nButtonWidth = 6;
		int nPageStartX = rect().nWidth - nButtonWidth * nButtonCount - 2 * 2 - nButtonCount;//	nButtonWidth*2：两个按钮；2*2：竖向边框占2格，并且有2个竖向边框
		
		{
			m_btPrePage.setBkColor(white);
			m_btPrePage.setTextColor(black);
			m_btPrePage.setCaption("上一页");
			m_btPrePage.setSelectable(true);
			Rect rectBtPre;
			rectBtPre.X = rect().X + nPageStartX;
			rectBtPre.Y = rect().Y + rect().nHeight - 1;
			rectBtPre.nWidth = nButtonWidth;
			rectBtPre.nHeight = 1;
			consoleUI()->setControlRect(&m_btPrePage, rectBtPre, false);
			consoleUI()->addControl(&m_btPrePage, false);
			m_btPrePage.setClickedEvent(std::bind(&ConsoleListBox::onClickPrePage, this));
			consoleUI()->addEvent(&m_btPrePage);

			nPageStartX += nButtonWidth + 1;
			m_btNextPage.setBkColor(white);
			m_btNextPage.setTextColor(black);
			m_btNextPage.setCaption("下一页");
			m_btNextPage.setSelectable(true);
			Rect rectBtNext;
			rectBtNext.X = rect().X + nPageStartX;
			rectBtNext.Y = rect().Y + rect().nHeight - 1;
			rectBtNext.nWidth = nButtonWidth;
			rectBtNext.nHeight = 1;
			consoleUI()->setControlRect(&m_btNextPage, rectBtNext, false);
			consoleUI()->addControl(&m_btNextPage, false);
			m_btNextPage.setClickedEvent(std::bind(&ConsoleListBox::onClickNextPage, this));
			consoleUI()->addEvent(&m_btNextPage);
		}

		nPageStartX += 1;
		{
			nPageStartX += nButtonWidth + 1;
			m_btPreItem.setBkColor(white);
			m_btPreItem.setTextColor(black);
			m_btPreItem.setCaption("上一项");
			m_btPreItem.setSelectable(true);
			Rect rectBtPre;
			rectBtPre.X = rect().X + nPageStartX;
			rectBtPre.Y = rect().Y + rect().nHeight - 1;
			rectBtPre.nWidth = nButtonWidth;
			rectBtPre.nHeight = 1;
			consoleUI()->setControlRect(&m_btPreItem, rectBtPre, false);
			consoleUI()->addControl(&m_btPreItem, false);
			m_btPreItem.setClickedEvent(std::bind(&ConsoleListBox::onClickPreItem, this));
			consoleUI()->addEvent(&m_btPreItem);

			nPageStartX += nButtonWidth + 1;
			m_btNextItem.setBkColor(white);
			m_btNextItem.setTextColor(black);
			m_btNextItem.setCaption("下一项");
			m_btNextItem.setSelectable(true);
			Rect rectBtNext;
			rectBtNext.X = rect().X + nPageStartX;
			rectBtNext.Y = rect().Y + rect().nHeight - 1;
			rectBtNext.nWidth = nButtonWidth;
			rectBtNext.nHeight = 1;
			consoleUI()->setControlRect(&m_btNextItem, rectBtNext, false);
			consoleUI()->addControl(&m_btNextItem, false);
			m_btNextItem.setClickedEvent(std::bind(&ConsoleListBox::onClickNextItem, this));
			consoleUI()->addEvent(&m_btNextItem);
		}
	}
	
	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			//draw border rectange
			consoleUI()->createBox(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight,
				m_broderColor.getTextColor(), m_broderColor.getBkColor(), m_broderColor.getStyle());

			//draw content rectange
			consoleUI()->createBox(getContentStartX(), getContentStartY(), getContentEndX(), getContentEndY(),
				getTextColor(), getBkColor(), getStyle());

			//draw contents
			// get visible contents
			if (m_vctContents.size() > 0)
			{
				if (m_nIdx < 0)
					m_nIdx = 0;
				else if (m_nIdx >= m_vctContents.size())
					m_nIdx = m_vctContents.size() - 1;

				for (int i = 0; i < getContentLines() && (m_nVisibleStart + i) < m_vctContents.size(); ++i)
				{
					consoleUI()->drawText(getContentStartX(), getContentStartY() + i,
						CutTooLongString(m_vctContents[m_nVisibleStart + i], getContentEndX() - getContentStartX()).c_str());

					consoleUI()->fillColorAndStyle(getContentStartX(), getContentStartY() + i, getContentEndX() - getContentStartX(),
						getTextColor(), getBkColor(), (STYLE)(common_lvb_underscore));
				}

				//current select
				consoleUI()->fillColorAndStyle(getContentStartX(), getContentStartY() + (m_nIdx - m_nVisibleStart), getContentEndX() - getContentStartX(),
					m_selectedItemColor.getTextColor(), m_selectedItemColor.getBkColor(), m_selectedItemColor.getStyle());
			}

			//draw pre page
			m_btPrePage.draw();
			//draw next page
			m_btNextPage.draw();
			//draw pre item
			m_btPreItem.draw();
			//draw next item
			m_btNextItem.draw();

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

	void setBorderColor(COLOR bkColor)
	{
		m_broderColor.setBkColor(bkColor);
	}
	
	void setSelectedColor(COLOR bkColor, COLOR textColor=color_default, STYLE style=style_default)
	{
		if (color_default != bkColor)
			m_selectedItemColor.setBkColor(bkColor);
		if (textColor != color_default)
			m_selectedItemColor.setTextColor(textColor);
		if (style != style_default)
			m_selectedItemColor.setStyle(style);
	}

	virtual void getPointColorAndStyle(int x, int y, COLOR& bkClolor, COLOR& textColor, STYLE& style) const
	{
		Rect rect;
		rect.X = getContentStartX();
		rect.Y = getContentStartY();
		rect.nWidth = getContentEndX() - getContentStartX();
		rect.nHeight = getContentEndY() - getContentStartY();
		if (!rect.isIn(x, y))
		{//border
			textColor = m_broderColor.getTextColor();
			bkClolor = m_broderColor.getBkColor();
			style = m_broderColor.getStyle();
		}
		else
		{//in
			textColor = getTextColor();
			bkClolor = getBkColor();
			style = getStyle();
		}
	}

	int insertItem(const char* txt, int nRow = -1, bool redraw=true)
	{
		int nRet = -1;

		std::string sTxt = txt;
		sTxt.resize(sTxt.size() + 1);
		sTxt[sTxt.size() - 1] = '\0';

		if (nRow < 0)
			nRow = m_vctContents.size();

		if (nRow >= m_vctContents.size())
		{
			m_vctContents.push_back(sTxt);
			nRet = m_vctContents.size() - 1;
		}
		else
		{
			m_vctContents.insert(m_vctContents.begin() + nRow, sTxt);
			nRet = nRow;
		}
		
		refreshPageButtons(redraw);
		
		return nRet;
	}

	bool removeItem(int nRow, bool redraw = true)
	{
		if (nRow >= m_vctContents.size())
			return false;

		m_vctContents.erase(m_vctContents.begin() + nRow);

		refreshPageButtons(redraw);

		return true;
	}

	std::string getItemText(int nRow) const
	{
		if (0 > nRow || nRow >= m_vctContents.size())
			return "";

		return m_vctContents[nRow];
	}

	void setItemText(int nRow, const char* txt, bool redraw = true)
	{
		if (0 > nRow || nRow >= m_vctContents.size())
			return;

		m_vctContents[nRow] = txt;

		refreshPageButtons(redraw);
	}

	int getSelectedItem() const
	{
		if (m_nIdx < 0 || m_nIdx >= m_vctContents.size())
			return -1;

		return m_nIdx;
	}

	std::string getSelectedItemText() const
	{
		int n = getSelectedItem();
		if (n < 0)
			return "";
		return getItemText(n);
	}

	int getItemsCount() const
	{
		return m_vctContents.size();
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return KEY_EVENT | MOUSE_EVENT;
	}

	virtual void onInitEvent()
	{

	}

	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (input_record.EventType == KEY_EVENT)
		{
			if (!input_record.Event.KeyEvent.bKeyDown)
			{
				return;
			}

			WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);
			switch (k)
			{
			case 38://上
			{
				onClickPreItem();
			}
			break;
			case 40://下
			{
				onClickNextItem();
			}
			break;
			}
		}
		else if (MOUSE_EVENT == input_record.EventType)
		{
			if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				int nX = input_record.Event.MouseEvent.dwMousePosition.X;
				int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
								
				Rect rectContent;
				rectContent.X = getContentStartX();
				rectContent.Y = getContentStartY();
				rectContent.nWidth = getContentEndX() - getContentStartX();
				rectContent.nHeight = getContentEndY() - getContentStartY();
				if (rectContent.isIn(nX, nY))
				{
					m_nIdx = m_nVisibleStart + (nY - getContentStartY());
					//draw();
					consoleUI()->redrawControl(this);

					//设置焦点到本控件
					consoleUI()->setCurPosition(getContentStartX() - 1, getContentStartY() + (m_nIdx - m_nVisibleStart));
				}
			}
		}
	}
	
	void onClickPrePage()
	{
		if (m_nVisibleStart <= 0)
		{
			refreshPageButtons();
			return;
		}

		m_nVisibleStart = m_nVisibleStart - getContentLines();
		if (m_nVisibleStart < 0)
		{
			m_nVisibleStart = 0;
		}
		m_nIdx = m_nVisibleStart;

		refreshPageButtons(false);
		consoleUI()->redrawControl(this);
	}

	void onClickNextPage()
	{
		if (getContentLines() >= m_vctContents.size())
		{
			refreshPageButtons();
			return;
		}

		m_nVisibleStart = m_nVisibleStart + getContentLines();
		if (m_nVisibleStart > m_vctContents.size() - getContentLines())
			m_nVisibleStart = m_vctContents.size() - getContentLines();
		m_nIdx = m_nVisibleStart;

		refreshPageButtons(false);
		consoleUI()->redrawControl(this);
	}

	void onClickPreItem()
	{
		if (m_nIdx > 0)
			--m_nIdx;
		else
			m_nIdx = 0;

		if (m_nIdx < m_nVisibleStart)
		{
			m_nVisibleStart = m_nIdx;
		}
		else
		{
			if (m_nIdx >= m_nVisibleStart + getContentLines())
			{
				++m_nVisibleStart;
			}
		}

		//draw();
		refreshPageButtons(false);
		consoleUI()->redrawControl(this);

		////设置焦点到本控件
		//consoleUI()->setCurPosition(getContentStartX() - 1, getContentStartY() + (m_nIdx - m_nVisibleStart));
	}

	void onClickNextItem()
	{
		if (m_nIdx < m_vctContents.size() - 1)
			++m_nIdx;
		else
			m_nIdx = m_vctContents.size() - 1;

		if (m_nIdx > m_nVisibleStart + getContentLines())
		{
			m_nVisibleStart = m_nIdx;
		}
		else
		{
			if (m_nIdx >= m_nVisibleStart + getContentLines())
			{
				++m_nVisibleStart;
			}
		}

		//draw();
		refreshPageButtons(false);
		consoleUI()->redrawControl(this);

		////设置焦点到本控件
		//consoleUI()->setCurPosition(getContentStartX() - 1, getContentStartY() + (m_nIdx - m_nVisibleStart));
	}

protected:
	//如果redraw == false，可以利用redrawtop或redrawbottom代替redraw
	void refreshPageButtons(bool redraw=true)
	{
		if (getContentLines() >= m_vctContents.size())
		{
			m_btPrePage.setVisible(false);
			m_btNextPage.setVisible(false);
		}
		else
		{
			if (m_nVisibleStart <= 0)
			{
				m_btPrePage.setVisible(false);
			}
			else
			{
				m_btPrePage.setVisible(true);
			}
			
			if (m_nVisibleStart >= m_vctContents.size() - getContentLines())
			{
				m_btNextPage.setVisible(false);
			}
			else
			{
				m_btNextPage.setVisible(true);
			}
		}
		
		//parent()->redrawControl(&m_btPrePage);
		//parent()->redrawControl(&m_btNextPage);//利用redrawtop可call到这两句，所以这里可以不调用
		if (redraw)
			consoleUI()->redrawControl(this);
	}

private:
	ConsoleColor m_broderColor;
	ConsoleColor m_selectedItemColor;

	int m_nIdx;
	int m_nVisibleStart;
	std::vector<std::string> m_vctContents;

	int borderWidth;

	ListBoxButton m_btPrePage;
	ListBoxButton m_btNextPage;

	ListBoxButton m_btPreItem;
	ListBoxButton m_btNextItem;
};

//Console Input Box
class ConsoleInputBox
	: public Event
	, public ControlSelectable
{
public:
	ConsoleInputBox(ConsoleUI *consoleUI = NULL)
	{
		setConsoleUI(consoleUI);

		m_colorSelected.setTextColor(black);
		m_colorSelected.setBkColor(white);
	}

	virtual ~ConsoleInputBox() { }

public:
	virtual void onInitControl()
	{

	}

	virtual bool draw()
	{
		bool bRet = false;
		consoleUI()->pushPosition();
		do {
			if (!Control::draw())
				break;

			if (color_default == m_textColor)
			{
				m_textColor = white;
			}
			if (color_default == m_bkColor)
			{
				m_bkColor = GL::black;
			}

			COLOR textColor = color_default;
			COLOR bkColor = color_default;
			STYLE style = style_default;
			getDrawColorAndStyle(textColor, bkColor, style);

			//draw input box
			consoleUI()->createBox(rect().X, rect().Y, rect().X + rect().nWidth - 1, rect().Y + rect().nHeight, textColor, bkColor, style);
			//parent()->fillColorAndStyle(rect().X, rect().Y, rect().nWidth, m_textColor, m_bkColor, (STYLE)(common_lvb_underscore));

			//draw content
			int nLinesNeed = m_content.size() / getContentALineWidth();
			if (m_content.size() % getContentALineWidth() != 0)
				++nLinesNeed;//剩余的字符串还需要额外一行
			std::string sDrawText;
			if (nLinesNeed > getContentLines())
			{//内容多于可显示的area，则截取内容以便显示
				size_t startZh = ZH_origIdxToZhIdx(m_content, m_content.size() - getVisibleContentLength());
				sDrawText = ZH_subString(m_content, startZh + 1);
				nLinesNeed = getContentLines();
			}
			else
			{//显示全部内容
				sDrawText = m_content;
			}
			//展示内容
			int nLineY = rect().Y;
			for (int i = 0; i < nLinesNeed; ++i)
			{
				if (i == nLinesNeed - 1)
				{//最后一行
					consoleUI()->drawText(rect().X, nLineY, sDrawText.c_str());
				}
				else
				{
					size_t zhIdx = ZH_origIdxToZhIdx(sDrawText, getContentALineWidth());
					consoleUI()->drawText(rect().X, nLineY, ZH_subString(sDrawText, 0, zhIdx).c_str());

					sDrawText = ZH_subString(sDrawText, zhIdx);
				}
				++nLineY;
			}

			//边框
			consoleUI()->createBorder(rect().X, rect().Y, rect().X + rect().nWidth - 1, rect().Y + rect().nHeight);

			bRet = true;
		} while (0);
		consoleUI()->popPosition();

		return bRet;
	}

public:
	int getContentStartX() const
	{
		return rect().X;
	}
	int getContentStartY() const
	{
		return rect().Y;
	}
	int getContentALineWidth() const
	{
		return rect().nWidth-1;
	}
	int getContentLines() const
	{
		return rect().nHeight;
	}
	int getVisibleContentLength() const
	{
		return getContentALineWidth() * getContentLines();
	}

	//设置焦点
	bool setFocus()
	{
		////如果输入后焦点在输入框外了，就把焦点设置回输入框中
		//COORD curPt = consoleUI()->getCurPosition();
		//if (!rect().isIn(curPt.X, curPt.Y))
		//{
		//	consoleUI()->setCurPosition(rect().X + rect().nWidth - 1, curPt.Y);
		//	return true;
		//}

		int nSelX = m_content.length();
		if (nSelX > getContentALineWidth()-1)
			nSelX = getContentALineWidth()-1;
		consoleUI()->setCurPosition(rect().X + nSelX, rect().Y);

		return false;
	}

	void setContent(const char* content)
	{
		m_content = content;
	}
	std::string getContent() const
	{
		return m_content;
	}

	void setPosition(int x, int y)
	{
		consoleUI()->setControlRect(this, x, y, -1, -1, false);
	}
	void setWidth(int nWidth)
	{
		consoleUI()->setControlRect(this, -1, -1, nWidth, -1, false);
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return MOUSE_EVENT | KEY_EVENT;
	}

	virtual void onInitEvent()
	{

	}

	//on event
	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (MOUSE_EVENT == input_record.EventType)
		{
			if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				//int nX = input_record.Event.MouseEvent.dwMousePosition.X;
				//int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
				////if (parent()->getTopControl(nX, nY) == this)
				//{
				//	//设置焦点到本控件
				//	consoleUI()->setCurPosition(nX, nY);
				//}
				setFocus();
			}

		}
		else if (KEY_EVENT == input_record.EventType)
		{
			COORD pt = consoleUI()->getCurPosition();

			if (input_record.Event.KeyEvent.bKeyDown)
			{
				switch (input_record.Event.KeyEvent.wVirtualKeyCode)
				{
				case 8://回退
				{
					int nCount = ZH_getStringLength(m_content);
					m_content = ZH_subString(m_content, 0, nCount - 1);
					draw();

					setFocus();
					break;
				}
				case 38://上
				{
					COORD curPt = consoleUI()->getCurPosition();
					if (curPt.Y > rect().Y)
						consoleUI()->setCurPosition(curPt.X, curPt.Y - 1);
					break;
				}
				case 40://下
				{
					COORD curPt = consoleUI()->getCurPosition();
					if (curPt.Y < rect().Y + rect().nHeight - 1)
						consoleUI()->setCurPosition(curPt.X, curPt.Y + 1);
					break;
				}
				case 37://左
				{
					COORD curPt = consoleUI()->getCurPosition();
					if (curPt.X > rect().X)
					{
						consoleUI()->setCurPosition(curPt.X - 1, curPt.Y);
					}
					else
					{
						if (curPt.Y > rect().Y)//判断是否还可以移到上一行
						{//向上一行尾部移动
							consoleUI()->setCurPosition(rect().X + rect().nWidth - 1, curPt.Y - 1);
						}
					}
					break;
				}
				case 39://右
				{
					COORD curPt = consoleUI()->getCurPosition();
					if (curPt.X < rect().X + rect().nWidth - 1)
					{
						consoleUI()->setCurPosition(curPt.X + 1, curPt.Y);
					}
					else
					{
						if (curPt.Y < rect().Y + rect().nHeight - 1)//判断是否还可以移到下一行
						{//向下一行首部移动
							consoleUI()->setCurPosition(rect().X, curPt.Y + 1);
						}
					}
					break;
				}
				case 36://home
				{
					COORD curPt = consoleUI()->getCurPosition();
					consoleUI()->setCurPosition(rect().X, curPt.Y);
					break;
				}
				case 35://end
				{
					COORD curPt = consoleUI()->getCurPosition();
					consoleUI()->setCurPosition(rect().X + rect().nWidth - 1, curPt.Y);
					break;
				}
				default:
				{//输入字符到输入框
					if ((input_record.Event.KeyEvent.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
						|| (input_record.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
						)
					{
						break;
					}

					COORD curPt = consoleUI()->getCurPosition();
					if (input_record.Event.KeyEvent.uChar.AsciiChar == input_record.Event.KeyEvent.uChar.UnicodeChar)
					{//ansi
						if (isprint((int)input_record.Event.KeyEvent.uChar.AsciiChar) != 0)
						{
							m_content += input_record.Event.KeyEvent.uChar.AsciiChar;
							//m_content += '\0';

							char c[2];
							sprintf(c, "%c", input_record.Event.KeyEvent.uChar.AsciiChar);
							//parent()->drawText(pt.X, pt.Y, c);
							draw();

							if (curPt.X > rect().X + rect().nWidth - 2/*是否到达转下行*/
								&& curPt.Y < rect().Y + rect().nHeight - 1/*是否可以转*/)
							{
								consoleUI()->setCurPosition(rect().X, curPt.Y + 1);
							}
						}
						//else if (input_record.Event.KeyEvent.uChar.AsciiChar == 13)//回车
						//{
						//	m_content += '\n';
						//}
					}
					else
					{//unicode
						WCHAR c[2];
						wsprintfW(c, L"%c", input_record.Event.KeyEvent.uChar.UnicodeChar);

						std::string ss;
						GL::WideByte2Ansi(ss, c);
						m_content += ss;

						//parent()->drawTextW(pt.X, pt.Y, c);
						draw();

						if (curPt.X > rect().X + rect().nWidth - 4/*是否到达转下行*/
							&& curPt.Y < rect().Y + rect().nHeight - 1/*是否可以转*/)
						{
							consoleUI()->setCurPosition(rect().X, curPt.Y + 1);
						}
					}

					setFocus();

					break;
				}

				}
			}
		}
	}

private:
	std::string m_content;
};

｝NS_END