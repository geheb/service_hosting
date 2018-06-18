#include "service_installer.h"
#include "handle_deleter.h"
#include "winapi_exception.h"

service_installer::service_installer(const environment &env)
	: _env(env)
{
}

void service_installer::install(const service_base &service)
{
	unique_sc_handle serviceControlManagerHandle(::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE));

	if (serviceControlManagerHandle == nullptr)
	{
		throw winapi_exception("open service manager failed", ::GetLastError());
	}

	std::wstring quotedPath(L"\"" + _env.currentFilePath() + L"\"");

	unique_sc_handle serviceHandle(::CreateService(serviceControlManagerHandle.get(),
		service.serviceName().c_str(),
		service.serviceDisplayName().c_str(),
		SERVICE_ALL_ACCESS, 
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, 
		SERVICE_ERROR_NORMAL, 
		quotedPath.c_str(),
		nullptr, 
		nullptr,
		nullptr,
		nullptr,
		nullptr));

	if (serviceHandle == nullptr)
	{
		throw winapi_exception("create service failed", ::GetLastError());
	}

    if (!service.serviceDescription().empty())
    {
        SERVICE_DESCRIPTION description;
        ::ZeroMemory(&description, sizeof(description));
        description.lpDescription = const_cast<LPWSTR>(service.serviceDescription().c_str());

        if (!::ChangeServiceConfig2(serviceHandle.get(), SERVICE_CONFIG_DESCRIPTION, &description))
        {
            throw winapi_exception("change service description failed", ::GetLastError());
        }
    }

	SERVICE_FAILURE_ACTIONS failureActions;
	::ZeroMemory(&failureActions, sizeof(failureActions));
	SC_ACTION failActions[3];
	::ZeroMemory(&failActions, sizeof(failActions));
	failActions[0].Type = SC_ACTION_RESTART;
	failActions[0].Delay = 60000;
	failActions[1].Type = SC_ACTION_RESTART;
	failActions[1].Delay = 60000;
	failActions[2].Type = SC_ACTION_RESTART;
	failActions[2].Delay = 60000;
	failureActions.dwResetPeriod = 0;
	failureActions.cActions = 3;
	failureActions.lpsaActions = failActions;

	if (!::ChangeServiceConfig2(serviceHandle.get(), SERVICE_CONFIG_FAILURE_ACTIONS, &failureActions))
	{
		throw winapi_exception("change service config failure actions failed", ::GetLastError());
	}

	SERVICE_DELAYED_AUTO_START_INFO delayedAutoStart;
	::ZeroMemory(&delayedAutoStart, sizeof(delayedAutoStart));
	delayedAutoStart.fDelayedAutostart = TRUE;

	if (!::ChangeServiceConfig2(serviceHandle.get(), SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &delayedAutoStart))
	{
		throw winapi_exception("change service config delayed auto start failed", ::GetLastError());
	}
}

void service_installer::process_wait_for_exit(unsigned long processId)
{
	unique_handle processHandle(::OpenProcess(SYNCHRONIZE, FALSE, processId));
	if (processHandle == nullptr)
	{
		return;
	}
	if (::WaitForSingleObject(processHandle.get(), INFINITE) == WAIT_FAILED)
	{
		throw winapi_exception("process wait for exit failed", ::GetLastError());
	}
}

void service_installer::uninstall(const service_base &service)
{
	unique_sc_handle serviceControlManagerHandle(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
	if (serviceControlManagerHandle == nullptr)
	{
		throw winapi_exception("open service manager failed", ::GetLastError());
	}

	unique_sc_handle serviceHandle(::OpenService(serviceControlManagerHandle.get(), service.serviceName().c_str(),
		SERVICE_ALL_ACCESS));

	if (serviceHandle == nullptr)
	{
		throw winapi_exception("open service failed", ::GetLastError());
	}

	SERVICE_STATUS_PROCESS serviceStatusProcess;
	::ZeroMemory(&serviceStatusProcess, sizeof(serviceStatusProcess));
	DWORD bytesNeeded = 0;

	if (!::QueryServiceStatusEx(serviceHandle.get(), SC_STATUS_PROCESS_INFO,
		(LPBYTE)&serviceStatusProcess, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded))
	{
		throw winapi_exception("query service status failed", ::GetLastError());
	}

	if (serviceStatusProcess.dwCurrentState == SERVICE_STOP_PENDING)
	{
		process_wait_for_exit(serviceStatusProcess.dwProcessId);
	}
	else if (serviceStatusProcess.dwCurrentState != SERVICE_STOPPED)
	{
		SERVICE_STATUS serviceStatus;
		::ZeroMemory(&serviceStatus, sizeof(serviceStatus));

		if (!::ControlService(serviceHandle.get(), SERVICE_CONTROL_STOP, &serviceStatus))
		{
			DWORD lastError = ::GetLastError();
			if (lastError != ERROR_SERVICE_REQUEST_TIMEOUT)
			{
				throw winapi_exception("stop service failed", lastError);
			}
		}
		process_wait_for_exit(serviceStatusProcess.dwProcessId);
	}

	if (!::DeleteService(serviceHandle.get()))
	{
		throw winapi_exception("delete service failed", ::GetLastError());
	}
}
