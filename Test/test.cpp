#include "ConsoleComEvent.h"
#include "ConsoleComCtrl.h"

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

void main()
{
	ConsoleUI consoleUI/*(GL::COLOR::dark_cyan)*/;

	EventMouseFollow mev(&consoleUI);
	consoleUI.addEvent(&mev);
	EventResizeWindow rw(&consoleUI);
	consoleUI.addEvent(&rw);

	ConsolePlane titlePlane(&consoleUI);
	Rect rectTitle;
	rectTitle.X = 0;
	rectTitle.Y = 0;
	rectTitle.nWidth = consoleUI.getConsoleScreenInfo().dwSize.X;
	rectTitle.nHeight = 2;
	titlePlane.setRect(rectTitle);
	titlePlane.setBKColor(dark_white);
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
	listBox.setRect(rectList);
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
	consoleUI.addEvent(&inputBox);
	consoleUI.addControl(&inputBox);

	MyButton btSetText(&consoleUI);
	btSetText.setCaption("设置文本");
	Rect rect;
	rect.nWidth = 8;
	rect.nHeight = 1;
	rect.X = 90;
	rect.Y = 4;
	btSetText.setRect(rect);
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
	Rect rectGetText;
	rectGetText.nWidth = 8;
	rectGetText.nHeight = 1;
	rectGetText.X = 90;
	rectGetText.Y = 6;
	btGetText.setRect(rectGetText);
	btGetText.setClickedEvent(onGetTextClick);
	consoleUI.addEvent(&btGetText);
	consoleUI.addControl(&btGetText);

	consoleUI.loopEvent();
}
