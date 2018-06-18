#pragma once
#include <string>

class winapi_error final
{
public:
	static std::wstring create_message(unsigned long errorCode);
private:
	winapi_error() {}
};

