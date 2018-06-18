#pragma once
#include <string>
#include <memory>
#include <vector>
#include <future>
#include "targetver.h"
#include <Windows.h>
#include "non_copyable.h"
#include "handle_deleter.h"
#include "event_logger.h"

class service_base
{
public:
	virtual ~service_base() {}
	static void run(service_base *instance);

	const std::wstring& serviceName() const { return _serviceName; }
	const std::wstring& serviceDisplayName() const { return _serviceDisplayName; }
	const std::wstring& serviceDescription() const { return _serviceDescription; }

protected:
	service_base(const std::wstring &serviceName, const std::wstring &serviceDisplayName, const std::wstring &serviceDescription,
		const event_logger &logger);
	virtual void on_start(const std::vector<std::wstring> &serviceArguments, HANDLE stopEvent) = 0;
	virtual void on_stop() {}

	const event_logger &_logger;
	std::wstring _serviceName;
private:
	_NON_COPYABLE(service_base);

	static void WINAPI service_main(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors);
	static DWORD WINAPI service_handler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
	bool set_status(unsigned long state);
	void start(const std::vector<std::wstring> &serviceArguments);
	void stop();

	static service_base *_instance;
	std::wstring _serviceDisplayName;
	std::wstring _serviceDescription;
	SERVICE_STATUS _serviceStatus;
	SERVICE_STATUS_HANDLE _serviceStatusHandle;
	unsigned long _checkPoint;
	unique_handle _stopEvent;
	std::vector<std::wstring> _serviceArguments;
	std::future<void> _startFuture;
};

