#pragma once
#include "targetver.h"
#include <Windows.h>
#include <string>
#include "environment.h"
#include "configuration.h"
#include "handle_deleter.h"

class event_logger final
{
public:
	event_logger(const environment &env, const std::wstring &name);
	void install();
	void uninstall();
	void log_info(const wchar_t *message) const { log(message, EVENTLOG_INFORMATION_TYPE); }
	void log_info(const char *message) const { log(message, EVENTLOG_INFORMATION_TYPE); }
	void log_info(const std::wstring &message) const { log(message.c_str(), EVENTLOG_INFORMATION_TYPE); }
	void log_warn(const wchar_t *message) const { log(message, EVENTLOG_WARNING_TYPE); }
	void log_warn(const char *message) const { log(message, EVENTLOG_WARNING_TYPE); }
	void log_warn(const std::wstring &message) const { log(message.c_str(), EVENTLOG_WARNING_TYPE); }
	void log_error(const wchar_t *message) const { log(message, EVENTLOG_ERROR_TYPE); }
	void log_error(const char *message) const { log(message, EVENTLOG_ERROR_TYPE); }
	void log_error(const std::wstring &message) const { log(message.c_str(), EVENTLOG_ERROR_TYPE); }

private:
	void log(const wchar_t *message, unsigned short type) const;
	void log(const char *message, unsigned short type) const;

	unique_event_source_handle _eventSourceHandle;
	std::wstring _subKey;
	std::wstring _name;
	const environment &_env;
};

