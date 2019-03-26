
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
#include "string_utility.h"
#include "ConsoleComEvent.h"

#define BEGIN_DRAW consoleUI()->pushPosition();do{
#define END_DRAW }while(0);consoleUI()->popPosition()
#define BREAK_DRAW break

NS_START｛

//Console Plane
class ConsolePlane
	: public EventMouseFollow
	, public ControlSelectable
{
public:
	ConsolePlane(ConsoleUI *consoleUI = NULL);

	virtual ~ConsolePlane() {}

public:
	virtual void onInitControl() {}

	virtual bool draw();

	void setHasBorder(bool b) { m_bHasBorder = b; }

	bool isBasBorder() { return m_bHasBorder; }

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
	ConsoleButton(ConsoleUI *consoleUI = NULL);
	virtual ~ConsoleButton() { }

	void setCaption(const std::string &sCaption) { m_sCaption = sCaption; }

	void setClickedEvent(FN_ON_CLICKED ev) { m_fnOnClicked = ev; }

	void setResizeEvent(FN_ON_RESIZE ev) { m_fnOnResize = ev; }

public:
	virtual void onInitControl() {}

	virtual bool draw();

	void draw(COLOR textColor, COLOR bkColor, STYLE style);

protected:
	virtual EVENT_TYPE eventType() { return MOUSE_EVENT | WINDOW_BUFFER_SIZE_EVENT; }

	virtual void onInitEvent() { }

	//on event
	virtual void onEvent(INPUT_RECORD &input_record);

public:	//supernumerary event function
	//on clicked
	virtual void onClicked();
	//on resize
	virtual void onResize();

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
	ConsoleLabel(ConsoleUI *consoleUI = NULL);

	virtual ~ConsoleLabel() {}

public:
	virtual void onInitControl() {}

	virtual bool draw();

public:
	void setContent(const char* content) { m_content = content; }
	std::string getContent() const { return m_content; }

	void setPosition(int x, int y);

	void setWidth(int nWidth);
	int getWidth() const;

	void setTextColor(COLOR textColor) { m_textColor = textColor; }
	COLOR getTextColor() const { return m_textColor; }

	void setAlign(ALIGN align) { m_align = align; }
	ALIGN getAlign() const { return m_align; }

	void setTransparent(bool b) { m_bTransparent = b; }
	bool isTransparent() { return m_bTransparent; }

	void setAutoWidth(bool b) { m_bAutoWidth = b; }
	bool isAutoWidth() { return m_bAutoWidth; }

protected:
	virtual EVENT_TYPE eventType() { return MOUSE_EVENT | KEY_EVENT; }

	virtual void onInitEvent() {}

	//on event
	virtual void onEvent(INPUT_RECORD &input_record);

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
	ConsoleCheckBox(ConsoleUI *consoleUI = NULL);

	virtual bool draw();

	void setPosition(int x, int y);

	virtual void setCheck(bool b) { m_bChecked = b; }
	bool isChecked() const { return m_bChecked; }

	void setContent(const char* txt) { m_sContent = txt; }
	std::string getContent() const { return m_sContent; }

	void setCheckEvent(FN_ON_CHECKED ev) { m_fnOnChecked = ev; }

	void setWidth(int n);

	void setAutoWidth(bool b) { m_bAutoWidth = b; }
	bool isAutoWidth() const { return m_bAutoWidth; }

	void setTransparent(bool b) { m_bTransparent = b; }
	bool isTransparent() { return m_bTransparent; }

protected:
	virtual EVENT_TYPE eventType() { return KEY_EVENT | MOUSE_EVENT; }

	virtual void onInitEvent() {}

	virtual void onEvent(INPUT_RECORD &input_record);

public:
	virtual void onCheck();

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

		void addRadioBox(ConsoleRadioBox* pRadioBox);

		void removeRadioBox(ConsoleRadioBox* pRadioBox);
		void removeRadioBox(int n);

		int getCheckedIdx();

		ConsoleRadioBox* getCheckedRadioBox();

		void setCheck(int n);

		void setCheck(ConsoleRadioBox* pRadioBox);

	private:
		std::vector<ConsoleRadioBox*> m_vctRadioBoxs;
	};

public:
	ConsoleRadioBox(ConsoleUI *consoleUI = NULL, Group* pGroup = NULL);

	virtual bool draw();

	virtual void setCheck(bool b);

	void setGroup(ConsoleRadioBox::Group* pGroup) { m_pGroup = pGroup; }
	ConsoleRadioBox::Group* getGroup() { return m_pGroup; }

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
		ListBoxButton(ConsoleUI *consoleUI, ConsoleListBox* pListBox);

	protected:
		virtual EVENT_TYPE eventType()
		{
			return ConsoleButton::eventType() | KEY_EVENT;
		}

		//on event
		virtual void onEvent(INPUT_RECORD &input_record);

	private:
		ConsoleListBox* m_pListBox;
	};

public:
	ConsoleListBox(ConsoleUI *consoleUI = NULL);

	int getContentStartX() const;
	int getContentEndX() const;

	int getContentStartY() const;
	int getContentEndY() const;

	int getContentLines() const;

	virtual void onInitControl();

	virtual bool draw();

	void setBorderColor(COLOR bkColor);

	void setSelectedColor(COLOR bkColor, COLOR textColor = color_default, STYLE style = style_default);

	virtual void getPointColorAndStyle(int x, int y, COLOR& bkClolor, COLOR& textColor, STYLE& style) const;

	int insertItem(const char* txt, int nRow = -1, bool redraw = true);

	bool removeItem(int nRow, bool redraw = true);

	std::string getItemText(int nRow) const;

	void setItemText(int nRow, const char* txt, bool redraw = true);

	int getSelectedItem() const;

	std::string getSelectedItemText() const;

	int getItemsCount() const;

protected:
	virtual EVENT_TYPE eventType() { return KEY_EVENT | MOUSE_EVENT; }

	virtual void onInitEvent() {}

	virtual void onEvent(INPUT_RECORD &input_record);

	void onClickPrePage();

	void onClickNextPage();

	void onClickPreItem();

	void onClickNextItem();

protected:
	//如果redraw == false，可以利用redrawtop或redrawbottom代替redraw
	void refreshPageButtons(bool redraw = true);

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
	ConsoleInputBox(ConsoleUI *consoleUI = NULL);

	virtual ~ConsoleInputBox() { }

public:
	virtual void onInitControl() {}

	virtual bool draw();

public:
	int getContentStartX() const;

	int getContentStartY() const;

	int getContentALineWidth() const;

	int getContentLines() const;

	int getVisibleContentLength() const;

	//设置焦点
	bool setFocus();

	void setContent(const char* content) { m_content = content; }
	std::string getContent() const { return m_content; }

	void setPosition(int x, int y);
	void setWidth(int nWidth);

protected:
	virtual EVENT_TYPE eventType() { return MOUSE_EVENT | KEY_EVENT; }

	virtual void onInitEvent() {}

	//on event
	virtual void onEvent(INPUT_RECORD &input_record);

private:
	std::string m_content;
};

//Console Progress bar
class ConsoleProgressBar
	: public Control
{
public:
	ConsoleProgressBar(ConsoleUI *consoleUI = NULL);

	virtual bool draw();

	void setMaxRange(int nMax) { m_nMax = nMax; }
	int getMaxRange() { return m_nMax; }

	void setCompleted(int n);

	int getCompleted() { return m_nCompleted; }

	void setStep(int n) { m_nStep = n; }
	int getStep() { return m_nStep; }

	bool forward();

	bool backward();

	bool operator++(int i) { return forward(); }

	bool operator--(int i) { return backward(); }

	int operator+(int i);

	bool operator-(int i);

private:
	int m_nCompleted;
	int m_nMax;
	int m_nStep;

	ConsoleColor m_colorRemain;
};

｝NS_END