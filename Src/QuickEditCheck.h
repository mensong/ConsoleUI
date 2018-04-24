#pragma once

class QuickEditCheck
{
public:
	QuickEditCheck(bool requireRestart=false);
	~QuickEditCheck();

protected:
	bool isQuickEditMode();

private:
	//std::string m_commandLine;
};

