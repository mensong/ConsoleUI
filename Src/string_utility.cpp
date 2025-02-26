#include <Windows.h>
#include "string_utility.h"

namespace GL
{

	bool ZH_isZHChar(char c)
	{
		return (c & 0x1000);
	}

	//ȡ���ַ����ַ�����,������Ӣ��
	size_t ZH_getStringLength(const std::string &s)
	{
		char ch;
		size_t cnt = s.size();
		for (size_t i = 0; i < s.size(); ++i)
		{
			ch = s.at(i);
			if (ZH_isZHChar(ch))
			{
				++i;
				--cnt;
			}
		}
		return cnt;
	}

	//ȡ����0Ϊ��ʼ������ָ��λ���ַ�
	std::string ZH_getStringAt(const std::string &s, size_t index)
	{
		char ch;
		std::string res;
		size_t cnt = index;
		size_t i = 0;
		for (; i < s.size() && i < cnt; ++i)
		{
			ch = s.at(i);
			if (ZH_isZHChar(ch))
			{
				++i;
				++cnt;
			}
		}
		ch = s.at(i);
		if (ZH_isZHChar(ch))
			res = s.substr(i, 2);
		else
			res = s.substr(i, 1);

		return res;
	}

	std::string ZH_subString(const std::string& s, size_t start, int count/* = -1*/)
	{
		if (0 == count)
			return "";

		if (0 > count)
			count = (int)s.size();

		char ch;
		size_t i = 0;
		for (; i < s.size() && start >0; ++i)
		{
			ch = s.at(i);
			if (ZH_isZHChar(ch))
			{
				++i;
			}
			--start;
		}

		std::string sRet;
		for (size_t j = i; j < s.size() && count > 0; ++j)
		{
			ch = s.at(j);
			sRet += ch;

			if (ZH_isZHChar(ch))
			{
				++j;
				sRet += s.at(j);
			}

			--count;
		}

		return sRet;
	}

	//ȡ�ַ����е���������֧�ָ�����string���룬vector���
	void GetIntegerInString(const std::string& s, std::vector<int>& vi)
	{
		std::string::size_type size = s.size();
		int element = 0;
		bool now_digit = false;
		bool negative = false;
		for (std::string::size_type i = 0; i < size; ++i)
		{
			if (s.at(i) <= '9' && s.at(i) >= '0')
			{
				now_digit = true;
				element = element * 10 + s.at(i) - '0';
			}
			else
			{
				if (s.at(i) == '-')
					negative = true;
				else
				{
					if (now_digit)
					{
						if (negative)
							element = -element;
						vi.push_back(element);
						element = 0;
						now_digit = false;
						negative = false;
					}
				}
			}
		}
		if (now_digit)
		{
			if (negative)
				element = -element;
			vi.push_back(element);
		}
	}

	int ZH_findString(const std::string & s, const std::string & sFind, size_t start/*=0*/)
	{
		size_t startOrig = ZH_ZhIdxToOrigIdx(s, start);
		if (startOrig == std::string::npos)
			return -1;

		size_t nFind = s.find(sFind, startOrig);
		if (nFind == std::string::npos)
			return -1;

		nFind = ZH_origIdxToZhIdx(s, nFind);
		if (nFind < start)
			return -1;

		return (int)nFind;
	}

	size_t ZH_ZhIdxToOrigIdx(const std::string & s, size_t ZhIdx)
	{
		if (ZH_getStringLength(s) <= ZhIdx)
			return std::string::npos;
		return ZH_subString(s, 0, (int)ZhIdx).length();
	}

	size_t ZH_origIdxToZhIdx(const std::string & s, size_t OrigIdx)
	{
		if (s.length() <= OrigIdx)
			return std::string::npos;

		if (ZH_isZHChar(s[OrigIdx]))
			++OrigIdx;

		return ZH_getStringLength(s.substr(0, OrigIdx + 1)) - 1;
	}

	std::string CutTooLongString(const std::string& s, int maxWidth)
	{
		//Ϊʲô����ֱ�ӵ��ַ�����ȡ�أ�����Ϊ���к��ֵ�ʱ�򣬺����п��ܱ��ض�
		std::string sValidText;
		if (s.length() > maxWidth)
		{//���ݶ��ڿ���ʾ��area�����ȡ�����Ա���ʾ
			size_t startZh = ZH_origIdxToZhIdx(s, maxWidth);
			sValidText = ZH_subString(s, 0, (int)startZh);
			if (sValidText.length() > maxWidth && GL::ZH_isZHChar(sValidText[sValidText.length() - 1]))
				sValidText[sValidText.length() - 2] = '\0';//��ֹ���ֵİ��ɳ�
		}
		else
		{//��ʾȫ������
			sValidText = s;
		}

		return sValidText;
	}

	bool WideByte2Ansi(std::string &out, const wchar_t* wstrcode)
	{
		int ansiSize = WideCharToMultiByte(CP_OEMCP, 0, wstrcode, -1, NULL, 0, NULL, NULL);
		if (ansiSize == ERROR_NO_UNICODE_TRANSLATION)
		{
			return false;
		}
		if (ansiSize == 0)
		{
			return false;
		}

		out.resize(ansiSize);
		int convresult = WideCharToMultiByte(CP_OEMCP, 0, wstrcode, -1, &(out[0]), ansiSize, NULL, NULL);
		if (convresult != ansiSize)
		{
			return false;
		}

		return true;
	}

}