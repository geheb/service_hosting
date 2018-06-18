#include "winapi_exception.h"
#include "service_base.h"

service_base *service_base::_instance;

service_base::service_base(
	const std::wstring &serviceName 
	, const std::wstring &serviceDisplayName 
	, const std::wstring &serviceDescription
	, const event_logger &logger
)
	: _serviceName(serviceName)
	, _serviceDisplayName(serviceDisplayName)
	, _serviceDescription(serviceDescription)
	, _logger(logger)
	, _serviceStatusHandle(nullptr)
	, _checkPoint(1)
{
	_stopEvent.reset(::CreateEvent(nullptr, TRUE, TRUE, nullptr));
	if (_stopEvent == nullptr)
	{
		throw winapi_exception("create event failed", ::GetLastError());
	}

	::ZeroMemory(&_serviceStatus, sizeof(_serviceStatus));
	_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	_serviceStatus.dwCurrentState = SERVICE_STOPPED;
	_serviceStatus.dwWin32ExitCode = NO_ERROR;
	_serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
	_serviceStatus.dwCheckPoint = 0;
	_serviceStatus.dwWaitHint = 0;
}

void service_base::run(service_base *instance)
{
	_instance = instance;

	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ const_cast<LPWSTR>(_instance->_serviceName.c_str()), service_main },
		{ nullptr, nullptr }
	};

	if (!::StartServiceCtrlDispatcher(serviceTable))
	{
		throw winapi_exception("start service dispatcher failed", ::GetLastError());
	}
}

void WINAPI service_base::service_main(DWORD dwNumServicesArgs, LPWSTR *lpServiceArgVectors)
{
	_instance->_serviceStatusHandle = ::RegisterServiceCtrlHandlerEx(_instance->_serviceName.c_str(), service_handler, nullptr);
	if (_instance->_serviceStatusHandle == nullptr)
	{
		throw winapi_exception("register control handler failed", ::GetLastError());
	}

	std::vector<std::wstring> serviceArguments;
	if (dwNumServicesArgs > 1)
	{
		serviceArguments.insert(serviceArguments.end(), lpServiceArgVectors + 1, lpServiceArgVectors + dwNumServicesArgs);
	}
	_instance->start(serviceArguments);
}

DWORD WINAPI service_base::service_handler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP: 
	case SERVICE_CONTROL_SHUTDOWN: 
		_instance->stop(); 
		return NO_ERROR;
	case SERVICE_CONTROL_INTERROGATE:
		return NO_ERROR;
	default:
		return ERROR_CALL_NOT_IMPLEMENTED;
	}
}

bool service_base::set_status(unsigned long state) 
{
	_serviceStatus.dwCurrentState = state;
	_serviceStatus.dwWin32ExitCode = NO_ERROR;
	_serviceStatus.dwWaitHint = 0;

	if (state == SERVICE_RUNNING || state == SERVICE_STOPPED)
		_checkPoint = 0;
	else 
		_checkPoint++;

	_serviceStatus.dwCheckPoint = _checkPoint;

	return ::SetServiceStatus(_serviceStatusHandle, &_serviceStatus) == TRUE;
}

void service_base::start(const std::vector<std::wstring> &serviceArguments)
{
	_serviceArguments = serviceArguments;
	_logger.log_info(L"starting service " + _serviceName + L"...");

	if (!set_status(SERVICE_START_PENDING))
	{
		DWORD lastError = ::GetLastError();
		stop();
		throw winapi_exception("set start pending status failed", lastError);
	}

	if (!::ResetEvent(_stopEvent.get()))
	{
		DWORD lastError = ::GetLastError();
		stop();
		throw winapi_exception("reset stop event failed", lastError);
	}

	 _startFuture = std::async(std::launch::async, &service_base::on_start, this, _serviceArguments, _stopEvent.get());

	if (!set_status(SERVICE_RUNNING))
	{
		DWORD lastError = ::GetLastError();
		stop();
		throw winapi_exception("set runnning status failed", lastError);
	}

	_logger.log_info(L"service " + _serviceName + L" started");
}

void service_base::stop()
{
	_logger.log_info(L"stopping service " + _serviceName + L"...");
	set_status(SERVICE_STOP_PENDING);
	::SetEvent(_stopEvent.get());
	on_stop();
	if (_startFuture.valid())
	{
		_startFuture.wait();
	}
	set_status(SERVICE_STOPPED);
	_logger.log_info(L"service " + _serviceName + L" stopped");
}
