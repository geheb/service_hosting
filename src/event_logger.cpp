#include "winapi_exception.h"
#include "event_logger.h"

event_logger::event_logger(const environment &env, const std::wstring &name)
	: _env(env)
	, _name(name)
	, _eventSourceHandle(::RegisterEventSource(nullptr, name.c_str()))
{
	if (_eventSourceHandle == nullptr)
	{
		throw winapi_exception("register event source failed", ::GetLastError());
	}
	_subKey = L"SYSTEM\\CurrentControlSet\\Services\\EventLog\\";
}

void event_logger::install()
{
	std::wstring appKey = _subKey + L"Application\\" + _name;
	HKEY rootKey = HKEY_LOCAL_MACHINE;
	HKEY key = nullptr;
	LONG status = ::RegOpenKeyEx(rootKey, appKey.c_str(), 0, KEY_READ, &key);
	if (status == ERROR_SUCCESS)
	{
		::RegCloseKey(key);
		return;
	}

	status = ::RegCreateKeyEx(rootKey,
		appKey.c_str(),
		0,
		0,
		REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE,
		0,
		&key,
		NULL);

	if (status != ERROR_SUCCESS)
	{
		throw winapi_exception("create event log key failed", status);
	}

	unique_hkey eventKey(key);

	std::wstring path(L"%SystemRoot%\\System32\\EventCreate.exe");

    status = ::RegSetValueEx(key, L"EventMessageFile", 0, REG_EXPAND_SZ, (LPCBYTE)path.c_str(), (DWORD)(path.size() + 1) * sizeof(wchar_t));
	if (status != ERROR_SUCCESS)
	{
		::RegDeleteKey(rootKey, appKey.c_str());
		throw winapi_exception("create reg value EventMessageFile failed", status);
	}

	DWORD customSource = 1;
	status = ::RegSetValueEx(key, L"CustomSource", 0, REG_DWORD, (LPCBYTE)&customSource, sizeof(customSource));
	if (status != ERROR_SUCCESS)
	{
		::RegDeleteKey(rootKey, appKey.c_str());
		throw winapi_exception("create reg value CustomSource failed", status);
	}

	DWORD typesSupported = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
	status = ::RegSetValueEx(key, L"TypesSupported", 0, REG_DWORD, (LPCBYTE)&typesSupported, sizeof(typesSupported));
	if (status != ERROR_SUCCESS)
	{
		::RegDeleteKey(rootKey, appKey.c_str());
		throw winapi_exception("create reg value TypesSupported failed", status);
	}
}

void event_logger::uninstall()
{
	std::wstring appKey = _subKey + L"Application\\" + _name;
	LONG status = ::RegDeleteKey(HKEY_LOCAL_MACHINE, appKey.c_str());
	if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND)
	{
		throw winapi_exception("delete event log key failed", status);
	}
}

void event_logger::log(const wchar_t *message, unsigned short type) const
{
	const wchar_t* data[1] = { message };
	::ReportEventW(_eventSourceHandle.get(), type, 0, 1, nullptr, 1, 0, data, nullptr);
}

void event_logger::log(const char *message, unsigned short type) const
{
	const char* data[1] = { message };
	::ReportEventA(_eventSourceHandle.get(), type, 0, 1, nullptr, 1, 0, data, nullptr);
}
