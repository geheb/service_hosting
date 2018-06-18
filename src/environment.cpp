#include "targetver.h"
#include <Windows.h>
#include "environment.h"
#include "winapi_exception.h"

environment::environment(int argc, wchar_t* argv[])
{
	if (argc > 1)
	{
		_arguments.insert(_arguments.end(), argv + 1, argv + argc);
	}

	_hasConsoleWindow = ::GetConsoleWindow() != nullptr;

	DWORD size = MAX_PATH;
	LPWSTR path = new wchar_t[size + 1];
	std::wmemset(path, 0, size + 1);

	::GetModuleFileName(NULL, path, size);
	DWORD lastError = ::GetLastError();
	while(lastError == ERROR_INSUFFICIENT_BUFFER)
	{
		delete[] path;
		size *= 2;
		path = new wchar_t[size + 1];
		std::wmemset(path, 0, size + 1);
		::GetModuleFileName(NULL, path, size);
		lastError = ::GetLastError();
	}

	if (lastError != ERROR_SUCCESS)
	{
		delete[] path;
		throw winapi_exception("get module file name failed", lastError);
	}

	_currentFilePath = path;
	delete[] path;

	std::wstring::size_type pos = _currentFilePath.find_last_of(L"\\/");
	_currentDirectoryPath = _currentFilePath.substr(0, pos);
}

bool environment::hasArgument(const std::wstring & arg) const
{
	return std::find(_arguments.begin(), _arguments.end(), arg) != _arguments.end();
}
