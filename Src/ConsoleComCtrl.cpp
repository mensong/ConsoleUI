#include "ConsoleComCtrl.h"

NS_START��

GL::ConsolePlane::ConsolePlane(ConsoleUI *consoleUI /*= NULL*/)
	: m_bHasBorder(false)
{
	setConsoleUI(consoleUI);
}


bool ConsolePlane::draw()
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

ConsoleButton::ConsoleButton(ConsoleUI *consoleUI /*= NULL*/)
{
	setConsoleUI(consoleUI);

	m_textColor = white;
	m_bkColor = gray;
	m_Style = style_default;

	m_textClickColor = m_textColor;
	m_bkClickColor = m_bkColor;
	m_ClickStyle = (STYLE)(background_blue);
}


void GL::ConsoleButton::onResize()
{
	if (m_fnOnResize)
		m_fnOnResize();
}

void GL::ConsoleButton::onClicked()
{
	if (m_fnOnClicked)
		m_fnOnClicked();
}

void GL::ConsoleButton::onEvent(INPUT_RECORD &input_record)
{
	if (MOUSE_EVENT == input_record.EventType)
	{
		if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			int nX = input_record.Event.MouseEvent.dwMousePosition.X;
			int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
			//if (parent()->getTopControl(nX, nY) == this)
			{
				//Ϊ��ʵ��jigЧ��
				draw(m_textClickColor, m_bkClickColor, m_ClickStyle);
				Sleep(200);
				draw();

				//���ý��㵽���ؼ�
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

void GL::ConsoleButton::draw(COLOR textColor, COLOR bkColor, STYLE style)
{
	//draw box
	consoleUI()->createBox(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight, textColor, bkColor, style);

	std::string sDrawText = CutTooLongString(m_sCaption, rect().nWidth);

	int leftOffset = 0;
	leftOffset = (rect().nWidth - (int)sDrawText.length()) / 2;
	int topOffset = 0;
	topOffset = rect().nHeight / 2;

	//��������ɵ�����,�����ڵ�ǰ���Ƶ����ݱȾɵ����ݶ�ʱ��������ɵ�����
	char *pSpace = new char[rect().nWidth + 1];
	memset(pSpace, ' ', rect().nWidth);
	pSpace[rect().nWidth] = '\0';
	consoleUI()->drawText(rect().X, rect().Y, pSpace);
	delete[] pSpace;

	//��������
	consoleUI()->drawText(rect().X + leftOffset, rect().Y + topOffset, sDrawText.c_str());

	consoleUI()->createBorder(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight);
}

bool GL::ConsoleButton::draw()
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



ConsoleLabel::ConsoleLabel(ConsoleUI *consoleUI /*= NULL*/)
	: m_bTransparent(true)
	, m_align(left)
	, m_bAutoWidth(false)
{
	setConsoleUI(consoleUI);

	m_bkColor = white;

}

void GL::ConsoleLabel::onEvent(INPUT_RECORD &input_record)
{
	if (MOUSE_EVENT == input_record.EventType)
	{
		if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			int nX = input_record.Event.MouseEvent.dwMousePosition.X;
			int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
			//if (parent()->getTopControl(nX, nY) == this)
			{
				//���ý��㵽���ؼ�
				consoleUI()->setCurPosition(nX, nY);
			}
		}

	}
	//else if (KEY_EVENT == input_record.EventType)
	//{
	//	COORD pt = consoleUI()->getCurPosition();

	//	if (input_record.Event.KeyEvent.bKeyDown)
	//	{
	//		switch (input_record.Event.KeyEvent.wVirtualKeyCode)
	//		{
	//		}
	//	}
	//}
}

int GL::ConsoleLabel::getWidth() const
{
	return rect().nWidth;
}

void GL::ConsoleLabel::setWidth(int nWidth)
{
	//rect().nWidth = nWidth;
	consoleUI()->setControlRect(this, -1, -1, nWidth, -1, false);
}

void GL::ConsoleLabel::setPosition(int x, int y)
{
	consoleUI()->setControlRect(this, x, y, -1, -1, false);
}

bool GL::ConsoleLabel::draw()
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
			consoleUI()->setControlRect(this, -1, -1, (int)m_content.length(), -1, false);

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
			leftOffset = (rect().nWidth - (int)sDrawText.length()) / 2;
		else if (m_align == right)
			leftOffset = rect().nWidth - (int)sDrawText.length();

		//��������ɵ�����,�����ڵ�ǰ���Ƶ����ݱȾɵ����ݶ�ʱ��������ɵ�����
		char *pSpace = new char[rect().nWidth + 1];
		memset(pSpace, ' ', rect().nWidth);
		pSpace[rect().nWidth] = '\0';
		consoleUI()->drawText(rect().X, rect().Y, pSpace);
		delete[] pSpace;

		//��������
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


ConsoleCheckBox::ConsoleCheckBox(ConsoleUI *consoleUI /*= NULL*/)
	: m_bChecked(true)
	, m_bAutoWidth(false)
	, m_bTransparent(false)
{
	setConsoleUI(consoleUI);

	consoleUI->setControlRect(this, -1, -1, 2, 1, false);

	m_textColor = white;
	m_bkColor = gray;
}

void GL::ConsoleCheckBox::onCheck()
{
	if (m_fnOnChecked)
		m_fnOnChecked();
}

void GL::ConsoleCheckBox::onEvent(INPUT_RECORD &input_record)
{
	if (input_record.EventType == KEY_EVENT)
	{
		if (!input_record.Event.KeyEvent.bKeyDown)
		{
			return;
		}

		//WORD k = (input_record.Event.KeyEvent.wVirtualKeyCode);
		//switch (k)
		//{

		//}
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

void GL::ConsoleCheckBox::setWidth(int n)
{
	consoleUI()->setControlRect(this, -1, -1, n, -1, false);
}

void GL::ConsoleCheckBox::setPosition(int x, int y)
{
	consoleUI()->setControlRect(this, x, y, -1, -1, false);
}

bool GL::ConsoleCheckBox::draw()
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
			consoleUI()->setControlRect(this, -1, -1, (int)m_sContent.length() + 2, -1, false);

		//draw box 
		consoleUI()->createBox(rect().X, rect().Y, rect().X + 2, rect().Y + rect().nHeight, textColor, bkColor, style);
		if (m_bChecked)
			consoleUI()->drawText(rect().X, rect().Y, "��");
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


void ConsoleRadioBox::Group::addRadioBox(ConsoleRadioBox* pRadioBox)
{
	pRadioBox->setGroup(this);

	m_vctRadioBoxs.push_back(pRadioBox);

	if (m_vctRadioBoxs.size() == 1)
	{
		setCheck(pRadioBox);
	}
}

void GL::ConsoleRadioBox::Group::setCheck(ConsoleRadioBox* pRadioBox)
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

void GL::ConsoleRadioBox::Group::setCheck(int n)
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

ConsoleRadioBox* GL::ConsoleRadioBox::Group::getCheckedRadioBox()
{
	return m_vctRadioBoxs[getCheckedIdx()];
}

int GL::ConsoleRadioBox::Group::getCheckedIdx()
{
	for (int i = 0; i < m_vctRadioBoxs.size(); ++i)
	{
		if (m_vctRadioBoxs[i]->isChecked())
			return i;
	}

	return -1;
}

void GL::ConsoleRadioBox::Group::removeRadioBox(int n)
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

void GL::ConsoleRadioBox::Group::removeRadioBox(ConsoleRadioBox* pRadioBox)
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

ConsoleRadioBox::ConsoleRadioBox(ConsoleUI *consoleUI /*= NULL*/, Group* pGroup /*= NULL*/)
	: ConsoleCheckBox(consoleUI)
	, m_pGroup(pGroup)
{
	setConsoleUI(consoleUI);
	m_bChecked = false;
}


void GL::ConsoleRadioBox::setCheck(bool b)
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

bool GL::ConsoleRadioBox::draw()
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
			consoleUI()->setControlRect(this, -1, -1, (int)m_sContent.length() + 2, -1, false);

		//draw box 
		consoleUI()->createBox(rect().X, rect().Y, rect().X + 2, rect().Y + rect().nHeight, textColor, bkColor, style);
		if (m_bChecked)
			consoleUI()->drawText(rect().X, rect().Y, "��");
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


ConsoleListBox::ListBoxButton::ListBoxButton(ConsoleUI *consoleUI, ConsoleListBox* pListBox)
	: ConsoleButton(consoleUI)
	, m_pListBox(pListBox)
{

}

void GL::ConsoleListBox::ListBoxButton::onEvent(INPUT_RECORD &input_record)
{
	if (KEY_EVENT == input_record.EventType)
	{
		//�ڰ�ť����������Ҳ�ܰ����¼��л��б�item
		if (m_pListBox)
			m_pListBox->onEvent(input_record);
	}
	else
	{
		ConsoleButton::onEvent(input_record);
	}
}

ConsoleListBox::ConsoleListBox(ConsoleUI *consoleUI /*= NULL*/)
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

	m_brushBroder.setBkColor(gray);
	m_brushBroder.setTextColor(color_default);
	m_brushBroder.setStyle(style_default);

	m_brushSelectedItem.setBkColor(blue);
	m_brushSelectedItem.setTextColor(color_default);
	m_brushSelectedItem.setStyle((STYLE)(common_lvb_grid_horizontal | common_lvb_underscore));
}

void GL::ConsoleListBox::refreshPageButtons(bool redraw /*= true*/)
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
	//parent()->redrawControl(&m_btNextPage);//����redrawtop��call�������䣬����������Բ�����
	if (redraw)
		consoleUI()->redrawControl(this);
}

void GL::ConsoleListBox::onClickNextItem()
{
	if (m_nIdx < m_vctContents.size() - 1)
		++m_nIdx;
	else
		m_nIdx = (int)m_vctContents.size() - 1;

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
}

void GL::ConsoleListBox::onClickPreItem()
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
}

void GL::ConsoleListBox::onClickNextPage()
{
	if (getContentLines() >= m_vctContents.size())
	{
		refreshPageButtons();
		return;
	}

	m_nVisibleStart = m_nVisibleStart + getContentLines();
	if (m_nVisibleStart > (int)m_vctContents.size() - getContentLines())
		m_nVisibleStart = (int)m_vctContents.size() - getContentLines();
	m_nIdx = m_nVisibleStart;

	refreshPageButtons(false);
	consoleUI()->redrawControl(this);
}

void GL::ConsoleListBox::onClickPrePage()
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

void GL::ConsoleListBox::onEvent(INPUT_RECORD &input_record)
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
		case 38://��
		{
			onClickPreItem();
		}
		break;
		case 40://��
		{
			onClickNextItem();
		}
		break;
		case 37://��
		{
			onClickPrePage();
		}
		break;
		case 39://��
		{
			onClickNextPage();
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

				//���ý��㵽���ؼ�
				consoleUI()->setCurPosition(getContentStartX() - 1, getContentStartY() + (m_nIdx - m_nVisibleStart));
			}
		}
	}
}

int GL::ConsoleListBox::getItemsCount() const
{
	return (int)m_vctContents.size();
}

std::string GL::ConsoleListBox::getSelectedItemText() const
{
	int n = getSelectedItem();
	if (n < 0)
		return "";
	return getItemText(n);
}

int GL::ConsoleListBox::getSelectedItem() const
{
	if (m_nIdx < 0 || m_nIdx >= m_vctContents.size())
		return -1;

	return m_nIdx;
}

void GL::ConsoleListBox::setItemText(int nRow, const char* txt, bool redraw /*= true*/)
{
	if (0 > nRow || nRow >= m_vctContents.size())
		return;

	m_vctContents[nRow] = txt;

	refreshPageButtons(redraw);
}

std::string GL::ConsoleListBox::getItemText(int nRow) const
{
	if (0 > nRow || nRow >= m_vctContents.size())
		return "";

	return m_vctContents[nRow];
}

bool GL::ConsoleListBox::removeItem(int nRow, bool redraw /*= true*/)
{
	if (nRow >= m_vctContents.size())
		return false;

	m_vctContents.erase(m_vctContents.begin() + nRow);

	refreshPageButtons(redraw);

	return true;
}

int GL::ConsoleListBox::insertItem(const char* txt, int nRow /*= -1*/, bool redraw /*= true*/)
{
	int nRet = -1;

	std::string sTxt = txt;
	sTxt.resize(sTxt.size() + 1);
	sTxt[sTxt.size() - 1] = '\0';

	if (nRow < 0)
		nRow = (int)m_vctContents.size();

	if (nRow >= (int)m_vctContents.size())
	{
		m_vctContents.push_back(sTxt);
		nRet = (int)m_vctContents.size() - 1;
	}
	else
	{
		m_vctContents.insert(m_vctContents.begin() + nRow, sTxt);
		nRet = nRow;
	}

	refreshPageButtons(redraw);

	return nRet;
}

void GL::ConsoleListBox::getPointColorAndStyle(int x, int y, COLOR& bkClolor, COLOR& textColor, STYLE& style) const
{
	Rect rect;
	rect.X = getContentStartX();
	rect.Y = getContentStartY();
	rect.nWidth = getContentEndX() - getContentStartX();
	rect.nHeight = getContentEndY() - getContentStartY();
	if (!rect.isIn(x, y))
	{//border
		textColor = m_brushBroder.getTextColor();
		bkClolor = m_brushBroder.getBkColor();
		style = m_brushBroder.getStyle();
	}
	else
	{//in
		textColor = getTextColor();
		bkClolor = getBkColor();
		style = getStyle();
	}
}

void GL::ConsoleListBox::setSelectedColor(COLOR bkColor, COLOR textColor /*= color_default*/, STYLE style /*= style_default*/)
{
	if (color_default != bkColor)
		m_brushSelectedItem.setBkColor(bkColor);
	if (textColor != color_default)
		m_brushSelectedItem.setTextColor(textColor);
	if (style != style_default)
		m_brushSelectedItem.setStyle(style);
}

void GL::ConsoleListBox::setBorderColor(COLOR bkColor)
{
	m_brushBroder.setBkColor(bkColor);
}

bool GL::ConsoleListBox::draw()
{
	bool bRet = false;
	consoleUI()->pushPosition();
	do {
		if (!Control::draw())
			break;

		//draw border rectange
		consoleUI()->createBox(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight,
			m_brushBroder.getTextColor(), m_brushBroder.getBkColor(), m_brushBroder.getStyle());

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
				m_nIdx = (int)m_vctContents.size() - 1;

			for (int i = 0; i < getContentLines() && (m_nVisibleStart + i) < m_vctContents.size(); ++i)
			{
				consoleUI()->drawText(getContentStartX(), getContentStartY() + i,
					CutTooLongString(m_vctContents[m_nVisibleStart + i], getContentEndX() - getContentStartX()).c_str());

				consoleUI()->fillColorAndStyle(getContentStartX(), getContentStartY() + i, getContentEndX() - getContentStartX(),
					getTextColor(), getBkColor(), (STYLE)(common_lvb_underscore));
			}

			//current select
			consoleUI()->fillColorAndStyle(getContentStartX(), getContentStartY() + (m_nIdx - m_nVisibleStart), getContentEndX() - getContentStartX(),
				m_brushSelectedItem.getTextColor(), m_brushSelectedItem.getBkColor(), m_brushSelectedItem.getStyle());
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

void GL::ConsoleListBox::onInitControl()
{
	int nButtonCount = 4;
	int nButtonWidth = 6;
	int nPageStartX = rect().nWidth - nButtonWidth * nButtonCount - 2 * 2 - nButtonCount;//	nButtonWidth*2��������ť��2*2������߿�ռ2�񣬲�����2������߿�

	{
		m_btPrePage.setBkColor(white);
		m_btPrePage.setTextColor(black);
		m_btPrePage.setCaption("��һҳ");
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
		m_btNextPage.setCaption("��һҳ");
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
		m_btPreItem.setCaption("��һ��");
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
		m_btNextItem.setCaption("��һ��");
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

int GL::ConsoleListBox::getContentLines() const
{
	return getContentEndY() - getContentStartY();
}

int GL::ConsoleListBox::getContentEndY() const
{
	return rect().Y + rect().nHeight - borderWidth;
}

int GL::ConsoleListBox::getContentStartY() const
{
	return rect().Y + borderWidth;
}

int GL::ConsoleListBox::getContentEndX() const
{
	return rect().X + rect().nWidth - borderWidth * 2;
}

int GL::ConsoleListBox::getContentStartX() const
{
	return rect().X + borderWidth * 2;
}


ConsoleInputBox::ConsoleInputBox(ConsoleUI *consoleUI /*= NULL*/)
{
	setConsoleUI(consoleUI);

	m_brushSelected.setTextColor(black);
	m_brushSelected.setBkColor(white);
}

void GL::ConsoleInputBox::onEvent(INPUT_RECORD &input_record)
{
	if (MOUSE_EVENT == input_record.EventType)
	{
		if (input_record.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			//int nX = input_record.Event.MouseEvent.dwMousePosition.X;
			//int nY = input_record.Event.MouseEvent.dwMousePosition.Y;
			////if (parent()->getTopControl(nX, nY) == this)
			//{
			//	//���ý��㵽���ؼ�
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
			case 8://����
			{
				int nCount = (int)ZH_getStringLength(m_content);
				m_content = ZH_subString(m_content, 0, nCount - 1);
				draw();

				setFocus();
				break;
			}
			case 38://��
			{
				COORD curPt = consoleUI()->getCurPosition();
				if (curPt.Y > rect().Y)
					consoleUI()->setCurPosition(curPt.X, curPt.Y - 1);
				break;
			}
			case 40://��
			{
				COORD curPt = consoleUI()->getCurPosition();
				if (curPt.Y < rect().Y + rect().nHeight - 1)
					consoleUI()->setCurPosition(curPt.X, curPt.Y + 1);
				break;
			}
			case 37://��
			{
				COORD curPt = consoleUI()->getCurPosition();
				if (curPt.X > rect().X)
				{
					consoleUI()->setCurPosition(curPt.X - 1, curPt.Y);
				}
				else
				{
					if (curPt.Y > rect().Y)//�ж��Ƿ񻹿����Ƶ���һ��
					{//����һ��β���ƶ�
						consoleUI()->setCurPosition(rect().X + rect().nWidth - 1, curPt.Y - 1);
					}
				}
				break;
			}
			case 39://��
			{
				COORD curPt = consoleUI()->getCurPosition();
				if (curPt.X < rect().X + rect().nWidth - 1)
				{
					consoleUI()->setCurPosition(curPt.X + 1, curPt.Y);
				}
				else
				{
					if (curPt.Y < rect().Y + rect().nHeight - 1)//�ж��Ƿ񻹿����Ƶ���һ��
					{//����һ���ײ��ƶ�
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
			{//�����ַ��������
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
						sprintf_s(c, "%c", input_record.Event.KeyEvent.uChar.AsciiChar);
						//parent()->drawText(pt.X, pt.Y, c);
						draw();

						if (curPt.X > rect().X + rect().nWidth - 2/*�Ƿ񵽴�ת����*/
							&& curPt.Y < rect().Y + rect().nHeight - 1/*�Ƿ����ת*/)
						{
							consoleUI()->setCurPosition(rect().X, curPt.Y + 1);
						}
					}
					//else if (input_record.Event.KeyEvent.uChar.AsciiChar == 13)//�س�
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
					m_content += ss.c_str();

					//parent()->drawTextW(pt.X, pt.Y, c);
					draw();

					if (curPt.X > rect().X + rect().nWidth - 4/*�Ƿ񵽴�ת����*/
						&& curPt.Y < rect().Y + rect().nHeight - 1/*�Ƿ����ת*/)
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

int GL::ConsoleInputBox::getContentStartX() const
{
	return rect().X;
}

int GL::ConsoleInputBox::getContentStartY() const
{
	return rect().Y;
}

int GL::ConsoleInputBox::getContentALineWidth() const
{
	return rect().nWidth - 1;
}

int GL::ConsoleInputBox::getContentLines() const
{
	return rect().nHeight;
}

int GL::ConsoleInputBox::getVisibleContentLength() const
{
	return getContentALineWidth() * getContentLines();
}

void GL::ConsoleInputBox::setWidth(int nWidth)
{
	consoleUI()->setControlRect(this, -1, -1, nWidth, -1, false);
}

void GL::ConsoleInputBox::setPosition(int x, int y)
{
	consoleUI()->setControlRect(this, x, y, -1, -1, false);
}

bool GL::ConsoleInputBox::setFocus()
{
	////�������󽹵�����������ˣ��Ͱѽ������û��������
	//COORD curPt = consoleUI()->getCurPosition();
	//if (!rect().isIn(curPt.X, curPt.Y))
	//{
	//	consoleUI()->setCurPosition(rect().X + rect().nWidth - 1, curPt.Y);
	//	return true;
	//}

	int nSelX = (int)m_content.length();
	if (nSelX > getContentALineWidth() - 1)
		nSelX = getContentALineWidth() - 1;
	consoleUI()->setCurPosition(rect().X + nSelX, rect().Y);

	return false;
}

bool GL::ConsoleInputBox::draw()
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
		int nLinesNeed = (int)m_content.size() / getContentALineWidth();
		if (m_content.size() % getContentALineWidth() != 0)
			++nLinesNeed;//ʣ����ַ�������Ҫ����һ��
		std::string sDrawText;
		if (nLinesNeed > getContentLines())
		{//���ݶ��ڿ���ʾ��area�����ȡ�����Ա���ʾ
			size_t startZh = ZH_origIdxToZhIdx(m_content, m_content.size() - getVisibleContentLength());
			sDrawText = ZH_subString(m_content, startZh + 1);
			nLinesNeed = getContentLines();
		}
		else
		{//��ʾȫ������
			sDrawText = m_content;
		}
		//չʾ����
		int nLineY = rect().Y;
		for (int i = 0; i < nLinesNeed; ++i)
		{
			if (i == nLinesNeed - 1)
			{//���һ��
				consoleUI()->drawText(rect().X, nLineY, sDrawText.c_str());
			}
			else
			{
				size_t zhIdx = ZH_origIdxToZhIdx(sDrawText, getContentALineWidth());
				consoleUI()->drawText(rect().X, nLineY, ZH_subString(sDrawText, 0, (int)zhIdx).c_str());

				sDrawText = ZH_subString(sDrawText, zhIdx);
			}
			++nLineY;
		}

		//�߿�
		consoleUI()->createBorder(rect().X, rect().Y, rect().X + rect().nWidth - 1, rect().Y + rect().nHeight);

		bRet = true;
	} while (0);
	consoleUI()->popPosition();

	return bRet;
}


ConsoleProgressBar::ConsoleProgressBar(ConsoleUI *consoleUI /*= NULL*/)
	: m_nCompleted(0)
	, m_nMax(0)
	, m_nStep(1)
{
	setConsoleUI(consoleUI);

	m_textColor = yellow;
	m_bkColor = red;
}

bool GL::ConsoleProgressBar::operator-(int i)
{
	setCompleted(getCompleted() - 1);
	return getCompleted();
}

int GL::ConsoleProgressBar::operator+(int i)
{
	setCompleted(getCompleted() + 1);
	return getCompleted();
}

bool GL::ConsoleProgressBar::backward()
{
	bool bRet = false;
	if (m_nCompleted > 0)
	{
		--m_nCompleted;
		bRet = true;
	}

	draw();

	return bRet;
}

bool GL::ConsoleProgressBar::forward()
{
	bool bRet = false;
	if (m_nCompleted < m_nMax)
	{
		++m_nCompleted;
		bRet = true;
	}

	draw();

	return bRet;
}

void GL::ConsoleProgressBar::setCompleted(int n)
{
	if (n > m_nMax)
		n = m_nMax;
	else if (n < 0)
		n = 0;
	m_nCompleted = n;
}

bool GL::ConsoleProgressBar::draw()
{
	bool bRet = false;
	consoleUI()->pushPosition();
	do {
		if (!Control::draw())
			break;

		COLOR textColor = getTextColor();
		COLOR bkColor = getBkColor();
		STYLE style = getStyle();

		//draw context
		int nCompletedX = (m_nCompleted * rect().nWidth) / (m_nMax == 0 ? rect().nWidth : m_nMax);
		consoleUI()->createBox(rect().X, rect().Y, rect().X + nCompletedX, rect().Y + rect().nHeight, textColor, bkColor, style);
		consoleUI()->createBox(rect().X + nCompletedX, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight,
			m_brushRemain.getTextColor(), m_brushRemain.getBkColor(), m_brushRemain.getStyle());
		//draw border
		consoleUI()->createBorder(rect().X, rect().Y, rect().X + rect().nWidth, rect().Y + rect().nHeight);

		bRet = true;
	} while (0);
	consoleUI()->popPosition();

	return bRet;
}


��NS_END



