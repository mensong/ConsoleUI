#include "QuickEditCheck.h"
#include "RegOperator.h"
#include "ConsoleUI.h"
#include "ConsoleComCtrl.h"

using namespace GL;

void StringReplace(std::string &strBase, const std::string& strSrc, const std::string& strDes)
{
	std::string::size_type pos = 0;
	std::string::size_type srcLen = strSrc.size();
	std::string::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != std::string::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
	}
}

Timer timerPaoma;
std::atomic<bool> drawing = false;

class EventSelectMemu
	: public Event
{
public:
	EventSelectMemu(ConsoleUI *consoleUI = NULL)
	{
		setConsoleUI(consoleUI);
		m_nSel = 0;

		m_txtColorNormal = white;
		m_bkColorNormal = gray;

		m_txtColorSel = red;
		m_bkColorSel = blue;
	}

	void addControl(Control* pCtrl)
	{
		m_vctBts.push_back(pCtrl);
	}

	void refreshButtons()
	{
		timerPaoma.Pause();

		while (drawing)
			Sleep(1);

		for (int i = 0; i < m_vctBts.size(); ++i)
		{
			Control* pBt = m_vctBts[i];

			if (i == m_nSel)
			{
				pBt->setTextColor(m_txtColorSel);
				pBt->setBkColor(m_bkColorSel);

				consoleUI()->setCurPosition(pBt->rect().X, pBt->rect().Y + pBt->rect().nHeight - 1);
			}
			else
			{
				pBt->setTextColor(m_txtColorNormal);
				pBt->setBkColor(m_bkColorNormal);
			}

			consoleUI()->redrawControl(pBt);
		}



		timerPaoma.Resume();
	}

protected:
	virtual EVENT_TYPE eventType()
	{
		return KEY_EVENT;
	}

	virtual void onEvent(INPUT_RECORD &input_record)
	{
		if (KEY_EVENT == input_record.EventType)
		{
			if (!input_record.Event.KeyEvent.bKeyDown)
			{
				return;
			}

			WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);
			switch (k)
			{
			case 9://TAB
			case 38://上
			case 37://左
			case 40://下			
			case 39://右
			{
				m_nSel = (m_nSel + 1) % m_vctBts.size();
				refreshButtons();
				break;
			}
			case 13://回车
			{
				if (m_nSel < 0 || m_nSel >= m_vctBts.size())
					break;

				if (m_vctBts[m_nSel]->getID() == "RESTART")
				{
					goto RESTART;
				}
				else if (m_vctBts[m_nSel]->getID() == "IGNORE")
				{
					goto CONTINUE;
				}
				break;
			}
			case 82://R
			{
			RESTART:
				static bool bHasSeted = false;
				if (bHasSeted)
					break;
				bHasSeted = true;

				CRegOperator reg(HKEY_CURRENT_USER);
				std::string sExePath(MAX_PATH, '\0');
				GetModuleFileNameA(NULL, &(sExePath[0]), MAX_PATH);
				StringReplace(sExePath, "\\", "_");
				StringReplace(sExePath, "/", "_");
				sExePath = "Console\\" + sExePath;
				reg.CreateKey(sExePath.c_str());
				reg.Write("QuickEdit", (DWORD)0);
				reg.Close();

				ConsoleLabel* pLable = consoleUI()->getConsoleById("WARN_LABLE")->CAST_TO<ConsoleLabel>();
				pLable->setContent("已设置，请重新启动程序");
				break;
			}
			case 73://I
			{
			CONTINUE:
				//MessageBoxA(NULL, "continue", "msg", 0);
				timerPaoma.StopTimer();
				while (drawing)
					Sleep(1);
				consoleUI()->endLoopEvent();
				break;
			}
			}
		}
	}

private:
	int m_nSel;
	std::vector<Control*> m_vctBts;

	COLOR m_txtColorNormal;
	COLOR m_bkColorNormal;
	COLOR m_txtColorSel;
	COLOR m_bkColorSel;
};

QuickEditCheck::QuickEditCheck(bool requireRestart/* = false*/)
{
	if (isQuickEditMode())
	{
		ConsoleUI consoleUI;
		consoleUI.setTitle("注意：“快速编辑模式”已开启,会导致鼠标点击不可用");
		//consoleUI.setScreenBufferSize(120);

		EventSelectMemu sel(&consoleUI);
		sel.setGlobalEvent(true);
		consoleUI.addEvent(&sel);

		ConsolePlane warnPlan(&consoleUI);
		warnPlan.setBkColor(dark_cyan);
		warnPlan.setStyle(background_intensity);
		Rect rectPlan;
		rectPlan.X = 2;
		rectPlan.Y = 1;
		rectPlan.nWidth = consoleUI.getConsoleScreenInfo().dwSize.X - rectPlan.X * 2 + 1;
		rectPlan.nHeight = consoleUI.getConsoleScreenInfo().srWindow.Bottom - rectPlan.Y - 5;
		consoleUI.setControlRect(&warnPlan, rectPlan, false);
		consoleUI.addControl(&warnPlan);

		const char* szWarnTxt = "\"快速编辑模式\"已开启,会导致鼠标点击不可用,是否关闭 ?";
		ConsoleLabel warnLable(&consoleUI);
		warnLable.setID("WARN_LABLE");
		warnLable.setTextColor(red);
		warnLable.setPosition(rectPlan.X + 2, rectPlan.Y);
		warnLable.setContent(szWarnTxt);
		warnLable.setAutoWidth(true);
		consoleUI.addControl(&warnLable);

		ConsoleButton restart(&consoleUI);
		restart.setID("RESTART");
		restart.setCaption("关闭快速编辑模式(R)");
		Rect rectButton;
		rectButton.X = 0;
		rectButton.Y = rectPlan.Y + rectPlan.nHeight + 1;
		rectButton.nWidth = 23;
		rectButton.nHeight = 3;
		consoleUI.setControlRect(&restart, rectButton, false);
		consoleUI.addControl(&restart);
		consoleUI.addEvent(&restart);

		ConsoleButton ignore(&consoleUI);
		ignore.setID("IGNORE");
		ignore.setCaption("忽略，请继续(I)");
		rectButton.X += rectButton.nWidth + 2;
		consoleUI.setControlRect(&ignore, rectButton, false);
		consoleUI.addControl(&ignore);
		consoleUI.addEvent(&ignore);

		int moveX = (consoleUI.getConsoleScreenInfo().dwSize.X - (rectButton.X + rectButton.nWidth)) / 2;
		Rect rect = restart.getRect();
		rect.X += moveX;
		consoleUI.setControlRect(&restart, rect, true);
		rect = ignore.getRect();
		rect.X += moveX;
		consoleUI.setControlRect(&ignore, rect, true);

		sel.addControl(&restart);
		sel.addControl(&ignore);

		sel.refreshButtons();

		srand((unsigned)time(NULL));
		timerPaoma.StartTimer(1000, [&](void)->void {
			drawing = true;

			warnPlan.draw();
			int nStartX = rectPlan.X + (rand() % (rectPlan.nWidth - warnLable.getRect().nWidth));
			int nStartY = rectPlan.Y + (rand() % (rectPlan.nHeight));
			warnLable.setPosition(nStartX, nStartY);
			warnLable.draw();

			drawing = false;
		});

		consoleUI.startLoopEvent();
	}
}


QuickEditCheck::~QuickEditCheck()
{
}

bool QuickEditCheck::isQuickEditMode()
{
	std::string sExePath(MAX_PATH, '\0');
	GetModuleFileNameA(NULL, &(sExePath[0]), MAX_PATH);
	StringReplace(sExePath, "\\", "_");
	StringReplace(sExePath, "/", "_");

	sExePath = "Console\\" + sExePath;

	CRegOperator reg;
	reg.SetHKEY("HKEY_CURRENT_USER");
	if (!reg.OpenKey(sExePath.c_str()))
	{
		return true;
	}

	DWORD dwIsQuickEdit = 1;
	reg.Read("QuickEdit", dwIsQuickEdit);
	reg.Close();

	return (dwIsQuickEdit == 1);
}
