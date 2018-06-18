#include "targetver.h"
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include "string_extensions.h"
#include "winapi_error.h"
#include "handle_deleter.h"

std::wstring winapi_error::create_message(unsigned long errorCode)
{
	LPWSTR formatMessage = nullptr;
	std::wstringstream stream;

	DWORD length = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&formatMessage,
		0,
		NULL);

	stream << L"error code 0x" << std::setfill(L'0') << std::setw(6) << std::hex << errorCode;

	if (length > 0)
	{
		unique_hlocal localMessage(formatMessage);
		stream << L", " << wstring_extensions::trim_space(formatMessage);
	}

	return stream.str();
}
