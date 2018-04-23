#pragma once
#include <string>
#include <vector>

namespace GL
{
//判断字符是否为中文的开始字符
inline bool ZH_isZHChar(char c);

//取得字符串字符个数,不限中英文
size_t ZH_getStringLength(const std::string &s);

//取得以0为起始索引的指定位置字符
std::string ZH_getStringAt(std::string &s, size_t index);

//sub string
std::string ZH_subString(const std::string& s, size_t start, int count = -1);

//取字符串中的整型数，支持负数。string输入，vector输出
void GetIntegerInString(const std::string& s, std::vector<int>& vi);

//查找字符串
int ZH_findString(const std::string& s, const std::string& sFind, size_t start=0);

//把中文索引转换到原始索引
size_t ZH_ZhIdxToOrigIdx(const std::string& s, size_t ZhIdx);

//把原始索引转换到中文索引
size_t ZH_origIdxToZhIdx(const std::string& s, size_t OrigIdx);

}

