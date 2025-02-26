#include "ConsoleTabSelector.h"

NS_START｛

GL::ConsoleTabSelector::ConsoleTabSelector(ConsoleUI *consoleUI /*= NULL*/)
	: m_nSelectedLayerId(-1)
	, m_pCurSelCtrl(NULL)
	, m_bHasShowTab(false)
{
	setConsoleUI(consoleUI);

	setGlobalEvent(true);
}

std::vector<ControlSelectable*> GL::ConsoleTabSelector::getSelectableControls()
{
	std::vector<ControlSelectable*> vctRet;
	int nCtrlCount = (int)(consoleUI()->getControls().size());
	for (int i = 0; i < nCtrlCount; ++i)
	{
		ControlSelectable* pCtrl = dynamic_cast<ControlSelectable*>(consoleUI()->getControls()[i]);
		if (pCtrl != NULL && pCtrl->isVisible() && pCtrl->isEnable() && pCtrl->isSelectable())
			vctRet.push_back(pCtrl);
	}
	return vctRet;
}



ControlSelectable* ConsoleTabSelector::getSelectableControl(int nStartLayerId)
{
	std::vector<ControlSelectable*> vctControls = getSelectableControls();
	if (vctControls.size() < 1)
		return NULL;
	return vctControls[nStartLayerId % vctControls.size()];
}

void GL::ConsoleTabSelector::onEvent(INPUT_RECORD &input_record)
{
	if (KEY_EVENT == input_record.EventType)
	{
		if (!input_record.Event.KeyEvent.bKeyDown)
		{
			if (input_record.Event.KeyEvent.wVirtualKeyCode == 18)
			{//放开alt键
				endShowTab();
			}
			return;
		}

		//check cur idx
		Control* pActiveCtrl = consoleUI()->getActiveControl();
		m_nSelectedLayerId = getSelectedId(pActiveCtrl);

		WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);

		if ((input_record.Event.KeyEvent.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
			&& !(input_record.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
			&& !(input_record.Event.KeyEvent.dwControlKeyState & (SHIFT_PRESSED))
			)//alt按下或者alt组合键按下
		{
			if (input_record.Event.KeyEvent.wVirtualKeyCode == 18)
			{//只按下alt键
				beginShowTab();
			}
			else if (k >= '0' && k <= '9')
			{//按下alt组合键
				ControlSelectable* pCtrl = getSelectableControl(k - '0');
				consoleUI()->setCurPosition(pCtrl->rect().X, pCtrl->rect().Y);
				endShowTab();
			}
			else if (k >= 'A' && k <= 'Z')
			{
				ControlSelectable* pCtrl = getSelectableControl(k - 'A' + 10);
				consoleUI()->setCurPosition(pCtrl->rect().X, pCtrl->rect().Y);
				endShowTab();
			}
			return;
		}

		switch (k)
		{
		case 9://tab
		{
			ControlSelectable* pCtrlNext = getSelectableControl(++m_nSelectedLayerId);
			if (NULL != pCtrlNext)
			{
				consoleUI()->setCurPosition(pCtrlNext->rect().X, pCtrlNext->rect().Y);
				pCtrlNext->draw();

				if (NULL != m_pCurSelCtrl)
				{
					//consoleUI()->redrawControl(m_pCurSelCtrl);
					m_pCurSelCtrl->draw();
				}

				m_pCurSelCtrl = pCtrlNext;
			}
			break;
		}
		case 13://回车
		{
			Control* pActiveCtrl = consoleUI()->getActiveControl();
			ConsoleButton* pButton = NULL;
			ConsoleCheckBox* pCheckBox = NULL;
			if (NULL != (pButton = dynamic_cast<ConsoleButton*>(pActiveCtrl)))
			{
				pButton->onClicked();
			}
			else if (NULL != (pCheckBox = dynamic_cast<ConsoleCheckBox*>(pActiveCtrl)))
			{
				pCheckBox->setCheck(!pCheckBox->isChecked());
				pCheckBox->onCheck();
				consoleUI()->redrawControl(pCheckBox);
			}
			break;
		}
		case 27://ESC
		{
			endShowTab();
			break;
		}
		}
	}
}

void GL::ConsoleTabSelector::endShowTab()
{
	if (m_bHasShowTab)
	{
		std::vector<ControlSelectable*> vctControls = getSelectableControls();
		for (int i = 0; i < vctControls.size(); ++i)
		{
			//consoleUI()->redrawControl(vctControls[i]);
			vctControls[i]->draw();
		}
		m_bHasShowTab = false;
	}
}

void GL::ConsoleTabSelector::beginShowTab()
{
	if (!m_bHasShowTab)
	{
		consoleUI()->pushPosition();
		std::vector<ControlSelectable*> vctControls = getSelectableControls();
		for (int i = 0; i < vctControls.size() && i < 36; ++i)
		{
			Rect rect = vctControls[i]->rect();
			consoleUI()->createBox(rect.X, rect.Y, rect.X + 3, rect.Y + 1, white, red);
			int nId = getSelectedId(vctControls[i]);
			if (nId <= 9)
			{
				consoleUI()->drawText(rect.X + 1, rect.Y, std::to_string(nId).c_str());
			}
			else
			{
				char txt[2];
				txt[0] = (char)(('A' - 10) + nId);
				txt[1] = '\0';
				consoleUI()->drawText(rect.X + 1, rect.Y, txt);
			}
			consoleUI()->createBorder(rect.X, rect.Y, rect.X + 3, rect.Y + 1);
		}
		consoleUI()->popPosition();
		m_bHasShowTab = true;
	}
}

int GL::ConsoleTabSelector::getSelectedId(Control* pControl)
{
	ControlSelectable* pCtrlSelect = dynamic_cast<ControlSelectable*>(pControl);
	if (NULL == pCtrlSelect || !pCtrlSelect->isVisible() || !pCtrlSelect->isEnable())
		return m_nSelectedLayerId;

	std::vector<ControlSelectable*> vctControls = getSelectableControls();
	std::vector<ControlSelectable*>::iterator itFinder = std::find(vctControls.begin(), vctControls.end(), pCtrlSelect);
	if (itFinder == vctControls.end())
		return m_nSelectedLayerId;

	return (int)(itFinder - vctControls.begin());
}

｝NS_END

