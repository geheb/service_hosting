#include <algorithm> 
#include <cctype>
#include <locale>
#include "string_extensions.h"

template class string_extensions<char>;
template class string_extensions<wchar_t>;

template <typename charT>
std::basic_string<charT> string_extensions<charT>::ltrim_space(const std::basic_string<charT> &s)
{
	std::basic_string<charT> result(s);
	result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch)
	{
		return !std::isspace(ch);
	}));
	return result;
}

template <typename charT>
std::basic_string<charT> string_extensions<charT>::rtrim_space(const std::basic_string<charT> &s)
{
	std::basic_string<charT> result(s);
	result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch)
	{
		return !std::isspace(ch);
	}).base(), result.end());
	return result;
}

template <typename charT>
std::basic_string<charT> string_extensions<charT>::trim_space(const std::basic_string<charT> &s)
{
	return rtrim_space(ltrim_space(s));
}
