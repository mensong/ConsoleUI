#include "ConsoleComEvent.h"
#include "ConsoleComCtrl.h"
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
	///////////////////��1������̨����////////////////
	ConsoleUI consoleUI(GL::COLOR::dark_cyan);
	consoleUI.setTitle("Console UI ��ʾ");
	consoleUI.setScreenBufferSize(120);

	//ע�����̨�¼�
	//  EventResizeWindow�¼��Ƿ�ֹ����̨Resize��������
	EventResizeWindow rw(&consoleUI);
	consoleUI.addEvent(&rw);
	//  ConsoleTabSelector�ɶԴ���selectable�Ŀؼ����п���
	ConsoleTabSelector ts(&consoleUI);
	consoleUI.addEvent(&ts);
	//  Debug event
#ifdef _DEBUG
	EventDebug debug(&consoleUI);
	debug.setGlobalEvent(true);
	debug.setEvent(KEY_EVENT | MOUSE_EVENT);
	debug.setPosition(65, 13);
	consoleUI.addEvent(&debug);
#endif

	//Planeʾ��
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

	//Labelʾ��
	ConsoleLabel titleLabel(&consoleUI);
	titleLabel.setWidth(consoleUI.getConsoleScreenInfo().dwSize.X);
	titleLabel.setAlign(ConsoleLabel::center);
	titleLabel.setPosition(0, 0);
	titleLabel.setTextColor(GL::red);
	titleLabel.setContent("Console UI v0.01 ��Դ��");
	consoleUI.addControl(&titleLabel);

	//ListBoxʾ��
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
		sprintf_s(txt, "���%d", i);
		listBox.insertItem(txt, -1, false);
	}
	listBox.draw();

	ConsoleLabel tipLabel(&consoleUI);
	tipLabel.setWidth(8);
	tipLabel.setPosition(65, 4);
	tipLabel.setContent("�����룺");
	consoleUI.addControl(&tipLabel);

	//InputBoxʾ��
	ConsoleInputBox inputBox(&consoleUI);
	inputBox.setPosition(73, 4);
	inputBox.setWidth(15);
	inputBox.setID("MY_INPUT_BOX");
	inputBox.setSelectable(true);
	consoleUI.addEvent(&inputBox);
	consoleUI.addControl(&inputBox);

	//����Buttonʾ��
	MyButton btSetText(&consoleUI);
	btSetText.setCaption("�����ı�");
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

	//Buttonʾ��
	std::function<void(void)> onGetTextClick = [&](void) -> void {
		txtLabel.setContent(listBox.getSelectedItemText().c_str());
		consoleUI.redrawControl(&txtLabel);
	};
	ConsoleButton btGetText(&consoleUI);
	btGetText.setCaption("����ı�");
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

	//CheckBoxʾ��
	ConsoleCheckBox cb(&consoleUI);
	cb.setCheck(false);
	cb.setPosition(65, 8);
	cb.setAutoWidth(true);
	cb.setContent("��ѡ���Ұ�");
	cb.setSelectable(true);
	consoleUI.addControl(&cb);
	consoleUI.addEvent(&cb);

	//RadioBoxʾ��
	//  �½�һ��radio��
	ConsoleRadioBox::Group rbg;
	//  �½�radiobox��Ȼ����뵽radio����
	ConsoleRadioBox rb1(&consoleUI);
	rb1.setPosition(65, 10);
	rb1.setAutoWidth(true);
	rb1.setContent("��ѡ���Ұ�1");
	rb1.setTransparent(true);//͸��
	rb1.setSelectable(true);
	consoleUI.addControl(&rb1);
	consoleUI.addEvent(&rb1);
	rbg.addRadioBox(&rb1);
	ConsoleRadioBox rb2(&consoleUI);
	rb2.setPosition(80, 10);
	rb2.setAutoWidth(true);
	rb2.setContent("��ѡ���Ұ�2");
	rb2.setSelectable(true);
	consoleUI.addControl(&rb2);
	consoleUI.addEvent(&rb2);
	rbg.addRadioBox(&rb2);
	ConsoleRadioBox rb3(&consoleUI);
	rb3.setPosition(65, 11);
	rb3.setAutoWidth(true);
	rb3.setContent("��ѡ���Ұ�3");
	rb3.setSelectable(true);
	consoleUI.addControl(&rb3);
	consoleUI.addEvent(&rb3);
	rbg.addRadioBox(&rb3);
	ConsoleRadioBox rb4(&consoleUI);
	rb4.setPosition(80, 11);
	rb4.setAutoWidth(true);
	rb4.setContent("��ѡ���Ұ�4");
	rb4.setSelectable(true);
	consoleUI.addControl(&rb4);
	consoleUI.addEvent(&rb4);
	rbg.addRadioBox(&rb4);

	//�л�����ʾ��
	//  ����endLoopEvent�����������棬
	//    Ȼ����redraw��startLoopEvent�����½���
	std::function<void(void)> onTestClick = [&](void) -> void {
		consoleUI.endLoopEvent();
	};
	ConsoleButton btEnd(&consoleUI);
	btEnd.setCaption("�л�");
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

	ConsoleProgressBar prog(&consoleUI);
	prog.setMaxRange(10);
	prog.setCompleted(0);
	Rect rectProg;
	rectProg.X = 65;
	rectProg.Y = 20;
	rectProg.nWidth = 10;
	rectProg.nHeight = 1;
	consoleUI.setControlRect(&prog, rectProg);
	consoleUI.addControl(&prog);

	std::function<void(void)> onChangeProgClick = [&](void) -> void {
		static bool bP = true;
		if (bP)
		{
			if (!(prog++))
			{
				prog--;
				bP = false;
			}
		}
		else
		{
			if (!(prog--))
			{
				prog++;
				bP = true;
			}
		}
	};
	ConsoleButton btChangeProg(&consoleUI);
	btChangeProg.setCaption("+/-");
	btChangeProg.setSelectable(true);
	Rect rectChangeProg;
	rectChangeProg.nWidth = 5;
	rectChangeProg.nHeight = 1;
	rectChangeProg.X = 76;
	rectChangeProg.Y = 20;
	consoleUI.setControlRect(&btChangeProg, rectChangeProg, false);
	btChangeProg.setClickedEvent(onChangeProgClick);
	consoleUI.addEvent(&btChangeProg);
	consoleUI.addControl(&btChangeProg);


	///////////////////��2������̨����////////////////
	ConsoleUI consoleUI2/*(GL::COLOR::dark_cyan)*/;
	consoleUI2.setTitle("Console UI ��ʾ");

	EventMouseFollow mev2(&consoleUI2);
	consoleUI2.addEvent(&mev2);
	EventResizeWindow rw2(&consoleUI2);
	consoleUI2.addEvent(&rw2);
	ConsoleTabSelector ts2(&consoleUI2);
	consoleUI2.addEvent(&ts2);
#ifdef _DEBUG
	EventDebug debug2(&consoleUI2);
	debug2.setGlobalEvent(true);
	debug2.setEvent(KEY_EVENT | MOUSE_EVENT);
	debug2.setPosition(0, 0);
	consoleUI2.addEvent(&debug2);
#endif

	ConsolePlane aPlaneTip2(&consoleUI2);
	aPlaneTip2.setHasBorder(true);
	aPlaneTip2.setTextColor(red);
	aPlaneTip2.setBkColor(yellow);
	CONSOLE_SCREEN_BUFFER_INFO csbi = consoleUI2.getConsoleScreenInfo();
	Rect rectAPlaneTip2;
	rectAPlaneTip2.X = 2;
	rectAPlaneTip2.Y = 1;
	rectAPlaneTip2.nWidth = csbi.dwSize.X - rectAPlaneTip2.X * 2;
	rectAPlaneTip2.nHeight = csbi.srWindow.Bottom - csbi.srWindow.Top - rectAPlaneTip2.Y * 2;
	consoleUI2.setControlRect(&aPlaneTip2, rectAPlaneTip2);
	consoleUI2.addControl(&aPlaneTip2, false);

	std::function<void(void)> onTestClick2 = [&](void) -> void {
		consoleUI2.endLoopEvent();
	};
	ConsoleButton btEnd2(&consoleUI2);
	btEnd2.setCaption("�л�");
	btEnd2.setSelectable(true);
	btEnd2.setBkColor(green);
	btEnd2.setTextColor(red);
	Rect rectEnd2;
	rectEnd2.nWidth = 8;
	rectEnd2.nHeight = 1;
	rectEnd2.X = 110;
	rectEnd2.Y = 1;
	consoleUI2.setControlRect(&btEnd2, rectEnd2, false);
	btEnd2.setClickedEvent(onTestClick2);
	consoleUI2.addEvent(&btEnd2);
	consoleUI2.addControl(&btEnd2);

	//�رհ�ť
	bool bExit = false;
	ConsoleButton btClost(&consoleUI);
	btClost.setCaption("��");
	btClost.setSelectable(true);
	btClost.setBkColor(green);
	btClost.setTextColor(red);
	Rect rectClose;
	rectClose.nWidth = 4;
	rectClose.nHeight = 1;
	rectClose.X = consoleUI.getConsoleScreenInfo().dwSize.X - rectClose.nWidth - 1;
	rectClose.Y = 0;
	consoleUI.setControlRect(&btClost, rectClose, false);
	btClost.setClickedEvent([&](void)->void {
		consoleUI.endLoopEvent();
		consoleUI.endLoopEvent();
		bExit = true;
	});
	consoleUI.addEvent(&btClost);
	consoleUI.addControl(&btClost);


	///////////////////////Loop event////////////////////
	bool b = true;
	for (; !bExit; b = !b)
	{
		//��redraw��startLoopEvent�����½���
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
