#pragma once
#include "ConsoleUI.h"
#include "ConsoleComCtrl.h"

NS_START｛

class ConsoleTabSelector
	: public Event
{
public:
	ConsoleTabSelector(ConsoleUI *consoleUI = NULL);

	std::vector<ControlSelectable*> getSelectableControls();

	//获得可选择控件，如果nStartLayerId不是ControlSelectable，则向下找，如果到尾则回到首部开始向下找
	ControlSelectable* getSelectableControl(int nStartLayerId);

	int getSelectedId(Control* pControl);

	void beginShowTab();

	void endShowTab();

protected:
	virtual EVENT_TYPE eventType() { return KEY_EVENT; }

	virtual void onEvent(INPUT_RECORD &input_record);

private:
	int m_nSelectedLayerId;
	ControlSelectable* m_pCurSelCtrl;
	bool m_bHasShowTab;
};


｝NS_END