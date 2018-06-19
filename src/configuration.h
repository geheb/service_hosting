#pragma once
#include <string>
#include "environment.h"

class configuration final
{
public:
	configuration(const environment &env);
	void load();

	const std::wstring& serviceName() const { return _serviceName; }
	const std::wstring& serviceDisplayName() const { return _serviceDisplayName; }
	const std::wstring& serviceDescription() const { return _serviceDescription; }
	const std::wstring& executable() const { return _executable; }
	const std::wstring& arguments() const { return _arguments; }

	void throwExceptionIfInvalid()
	{
		if (_serviceName.empty() || _serviceDisplayName.empty() || _executable.empty())
		{
			throw std::runtime_error("config file service_hosting.json has invalid entries");
		}
	}

private:
	std::wstring _serviceName;
	std::wstring _serviceDisplayName;
	std::wstring _serviceDescription;
	std::wstring _executable;
	std::wstring _arguments;
	const environment &_env;
};

