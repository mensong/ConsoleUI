#include "ConsoleComEvent.h"
#include "ConsoleComCtrl.h"
#include "QuickEditCheck.h"
#include "ConsoleTabSelector.h"

using namespace GL;

class MyButton
	:public ConsoleButton
{
public:
	MyButton(ConsoleUI *consoleUI)
		: ConsoleButton(consoleUI)
	{

	}

protected:
	virtual void onInitEvent()
	{

	}

	virtual void onClicked()
	{
		ConsoleInputBox* pInputBox = consoleUI()->getConsoleById("MY_INPUT_BOX")->CAST_TO<ConsoleInputBox>();
		ConsoleListBox* pListBox = consoleUI()->getConsoleById("MY_LIST_BOX")->CAST_TO<ConsoleListBox>();
		std::string sTxt = pInputBox->getContent();
		int nSelRow = pListBox->getSelectedItem();
		pListBox->setItemText(nSelRow, sTxt.c_str());
	}

	virtual void onResize()
	{

	}
};

void main(int argc, char** argv)
{
	QuickEditCheck qec;	

	ConsoleUI consoleUI/*(GL::COLOR::dark_cyan)*/;
	consoleUI.setTitle("Console UI 演示");

	EventResizeWindow rw(&consoleUI);
	consoleUI.addEvent(&rw);
	ConsoleTabSelector ts(&consoleUI);
	consoleUI.addEvent(&ts);
#ifdef _DEBUG
	EventDebug debug(&consoleUI);
	debug.setGlobalEvent(true);
	debug.setEvent(KEY_EVENT | MOUSE_EVENT);
	debug.setPosition(65, 13);
	consoleUI.addEvent(&debug);
#endif

	ConsolePlane titlePlane(&consoleUI);
	Rect rectTitle;
	rectTitle.X = 0;
	rectTitle.Y = 0;
	rectTitle.nWidth = consoleUI.getConsoleScreenInfo().dwSize.X;
	rectTitle.nHeight = 2;
	consoleUI.setControlRect(&titlePlane, rectTitle, false);
	titlePlane.setBkColor(dark_white);
	consoleUI.addControl(&titlePlane);
	consoleUI.addEvent(&titlePlane);

	ConsoleLabel titleLabel(&consoleUI);
	titleLabel.setWidth(consoleUI.getConsoleScreenInfo().dwSize.X);
	titleLabel.setAlign(ConsoleLabel::center);
	titleLabel.setPosition(0, 0);
	titleLabel.setTextColor(GL::red);
	titleLabel.setContent("Console UI v0.01 开源库 - Author: Gergul");
	consoleUI.addControl(&titleLabel);

	ConsoleListBox listBox(&consoleUI);
	Rect rectList;
	rectList.X = 2;
	rectList.Y = 3;
	rectList.nWidth = 60;
	rectList.nHeight = 20;
	consoleUI.setControlRect(&listBox, rectList, false);
	listBox.setID("MY_LIST_BOX");
	consoleUI.addControl(&listBox);
	consoleUI.addEvent(&listBox);

	char txt[256];
	for (int i = 0; i < 100; ++i)
	{
		sprintf(txt, "你好%d", i);
		listBox.insertItem(txt, -1, false);
	}
	listBox.draw();

	ConsoleLabel tipLabel(&consoleUI);
	tipLabel.setWidth(8);
	tipLabel.setPosition(65, 4);
	tipLabel.setContent("请输入：");
	consoleUI.addControl(&tipLabel);

	ConsoleInputBox inputBox(&consoleUI);
	inputBox.setPosition(73, 4);
	inputBox.setWidth(15);
	inputBox.setID("MY_INPUT_BOX");
	inputBox.setSelectable(true);
	consoleUI.addEvent(&inputBox);
	consoleUI.addControl(&inputBox);

	MyButton btSetText(&consoleUI);
	btSetText.setCaption("设置文本");
	btSetText.setSelectable(true);
	Rect rect;
	rect.nWidth = 8;
	rect.nHeight = 1;
	rect.X = 90;
	rect.Y = 4;
	consoleUI.setControlRect(&btSetText, rect, false);
	consoleUI.addEvent(&btSetText);
	consoleUI.addControl(&btSetText);

	ConsoleLabel txtLabel(&consoleUI);
	txtLabel.setWidth(23);
	txtLabel.setPosition(65, 6);
	txtLabel.setTextColor(GL::red);
	txtLabel.setContent("");
	txtLabel.setTransparent(false);
	consoleUI.addControl(&txtLabel);

	std::function<void(void)> onGetTextClick = [&](void) -> void {
		txtLabel.setContent(listBox.getSelectedItemText().c_str());
		consoleUI.redrawControl(&txtLabel);
	};
	ConsoleButton btGetText(&consoleUI);
	btGetText.setCaption("获得文本");
	btGetText.setSelectable(true);
	Rect rectGetText;
	rectGetText.nWidth = 8;
	rectGetText.nHeight = 1;
	rectGetText.X = 90;
	rectGetText.Y = 6;
	consoleUI.setControlRect(&btGetText, rectGetText, false);
	btGetText.setClickedEvent(onGetTextClick);
	consoleUI.addEvent(&btGetText);
	consoleUI.addControl(&btGetText);

	ConsoleCheckBox cb(&consoleUI);
	cb.setCheck(false);
	cb.setPosition(65, 8);
	cb.setAutoWidth(true);
	cb.setContent("请选择我吧");
	cb.setSelectable(true);
	consoleUI.addControl(&cb);
	consoleUI.addEvent(&cb);

	ConsoleRadioBox::Group rbg;
	ConsoleRadioBox rb1(&consoleUI);
	rb1.setPosition(65, 10);
	rb1.setAutoWidth(true);
	rb1.setContent("请选择我吧1");
	rb1.setSelectable(true);
	consoleUI.addControl(&rb1);
	consoleUI.addEvent(&rb1);
	rbg.addRadioBox(&rb1);
	ConsoleRadioBox rb2(&consoleUI);
	rb2.setPosition(80, 10);
	rb2.setAutoWidth(true);
	rb2.setContent("请选择我吧2");
	rb2.setSelectable(true);
	consoleUI.addControl(&rb2);
	consoleUI.addEvent(&rb2);
	rbg.addRadioBox(&rb2);
	ConsoleRadioBox rb3(&consoleUI);
	rb3.setPosition(65, 11);
	rb3.setAutoWidth(true);
	rb3.setContent("请选择我吧3");
	rb3.setSelectable(true);
	consoleUI.addControl(&rb3);
	consoleUI.addEvent(&rb3);
	rbg.addRadioBox(&rb3);
	ConsoleRadioBox rb4(&consoleUI);
	rb4.setPosition(80, 11);
	rb4.setAutoWidth(true);
	rb4.setContent("请选择我吧4");
	rb4.setSelectable(true);
	consoleUI.addControl(&rb4);
	consoleUI.addEvent(&rb4);
	rbg.addRadioBox(&rb4);

	std::function<void(void)> onTestClick = [&](void) -> void {
		consoleUI.endLoopEvent();
	};
	ConsoleButton btEnd(&consoleUI);
	btEnd.setCaption("切换");
	btEnd.setSelectable(true);
	Rect rectEnd;
	rectEnd.nWidth = 8;
	rectEnd.nHeight = 1;
	rectEnd.X = 110;
	rectEnd.Y = 3;
	consoleUI.setControlRect(&btEnd, rectEnd, false);
	btEnd.setClickedEvent(onTestClick);
	consoleUI.addEvent(&btEnd);
	consoleUI.addControl(&btEnd);


	ConsoleUI consoleUI2/*(GL::COLOR::dark_cyan)*/;
	consoleUI2.setTitle("Console UI 演示");

	EventMouseFollow mev2(&consoleUI2);
	consoleUI2.addEvent(&mev2);
#ifdef _DEBUG
	EventDebug debug2(&consoleUI2);
	debug2.setGlobalEvent(true);
	debug2.setEvent(KEY_EVENT | MOUSE_EVENT);
	debug2.setPosition(0, 0);
	consoleUI2.addEvent(&debug2);
#endif

	std::function<void(void)> onTestClick2 = [&](void) -> void {
		consoleUI2.endLoopEvent();
	};
	ConsoleButton btEnd2(&consoleUI2);
	btEnd2.setCaption("切换");
	btEnd2.setSelectable(true);
	Rect rectEnd2;
	rectEnd2.nWidth = 8;
	rectEnd2.nHeight = 1;
	rectEnd2.X = 110;
	rectEnd2.Y = 1;
	consoleUI2.setControlRect(&btEnd2, rectEnd2, false);
	btEnd2.setClickedEvent(onTestClick2);
	consoleUI2.addEvent(&btEnd2);
	consoleUI2.addControl(&btEnd2);

	bool b = true;
	for ( ; true; b = !b)
	{
		if (b)
		{
			consoleUI.redraw();
			consoleUI.startLoopEvent();
		}
		else
		{
			consoleUI2.redraw();
			consoleUI2.startLoopEvent();
		}
	}
}
