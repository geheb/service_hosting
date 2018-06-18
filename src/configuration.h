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
	const std::wstring& executeFile() const { return _executeFile; }

    void throwExceptionIfInvalid()
    {
        if (_serviceName.empty() || _serviceDisplayName.empty() || _executeFile.empty())
        {
            throw std::runtime_error("config file geheb.service_hosting.json has invalid entries");
        }
    }

private:
	std::wstring _serviceName;
	std::wstring _serviceDisplayName;
	std::wstring _serviceDescription;
	std::wstring _executeFile;
	const environment &_env;
};

