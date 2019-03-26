#include <Windows.h>
#include "string_utility.h"

namespace GL
{

	bool ZH_isZHChar(char c)
	{
		return (c & 0x1000);
	}

	//取得字符串字符个数,不限中英文
	size_t ZH_getStringLength(const std::string &s)
	{
		short ch;
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

	//取得以0为起始索引的指定位置字符
	std::string ZH_getStringAt(const std::string &s, size_t index)
	{
		short ch;
		std::string res;
		unsigned cnt = index;
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
			count = s.size();

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
		for (int j = i; j < s.size() && count > 0; ++j)
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

	//取字符串中的整型数，支持负数。string输入，vector输出
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

		return nFind;
	}

	size_t ZH_ZhIdxToOrigIdx(const std::string & s, size_t ZhIdx)
	{
		if (ZH_getStringLength(s) <= ZhIdx)
			return std::string::npos;
		return ZH_subString(s, 0, ZhIdx).length();
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
		//为什么不用直接的字符串截取呢？是因为怕有汉字的时候，汉字有可能被截断
		std::string sValidText;
		if (s.length() > maxWidth)
		{//内容多于可显示的area，则截取内容以便显示
			size_t startZh = ZH_origIdxToZhIdx(s, maxWidth);
			sValidText = ZH_subString(s, 0, startZh);
			if (sValidText.length() > maxWidth && GL::ZH_isZHChar(sValidText[sValidText.length() - 1]))
				sValidText[sValidText.length() - 2] = '\0';//防止汉字的半块飞出
		}
		else
		{//显示全部内容
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