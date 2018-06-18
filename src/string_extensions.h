#pragma once
#include <string>

template <typename charT>
class string_extensions final
{
public:
	static std::basic_string<charT> ltrim_space(const std::basic_string<charT> &s);
	static std::basic_string<charT> rtrim_space(const std::basic_string<charT> &s);
	static std::basic_string<charT> trim_space(const std::basic_string<charT> &s);

private:
	string_extensions() {}
};

typedef string_extensions<char> cstring_extensions;
typedef string_extensions<wchar_t> wstring_extensions;


