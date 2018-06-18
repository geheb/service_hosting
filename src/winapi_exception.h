#pragma once
#include <stdexcept>

class winapi_exception final : public std::runtime_error
{
public:
	explicit winapi_exception(const char *message, unsigned long lastErrorCode) 
		: std::runtime_error(message)
	{
		_lastErrorCode = lastErrorCode;
	}
	unsigned long lastErrorCode() const { return _lastErrorCode; }
private:
	unsigned long _lastErrorCode;
};

