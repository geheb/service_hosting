#include <string>
#include <iostream>
#include <memory>
#include <tchar.h>
#include "service_installer.h"
#include "run_exe_service.h"
#include "configuration.h"
#include "event_logger.h"
#include "environment.h"
#include "winapi_exception.h"
#include "winapi_error.h"

int run(environment &env, event_logger &logger)
{
	configuration config(env);
	config.load();
	config.throwExceptionIfInvalid();
	run_exe_service service(env, config, logger);

	if (env.hasArguments() && env.hasConsoleWindow())
	{
		if (env.hasArgument(L"--install"))
		{
			std::wcout << L"Installing service..." << std::endl;
			service_installer(env).install(service);
			logger.install();
			std::wcout << L"Service installed." << std::endl;
			return 0;
		}
		else if (env.hasArgument(L"--uninstall"))
		{
			std::wcout << L"Uninstalling service..." << std::endl;
			service_installer(env).uninstall(service);
			logger.uninstall();
			std::wcout << L"Service uninstalled." << std::endl;
			return 0;
		}

		std::wcerr << L"Invalid argument. Use --install or --uninstall" << std::endl;
		return -1;
	}

	service_base::run(&service);
	return 0;
}

int _tmain(int argc, wchar_t* argv[])
{
	environment env(argc, argv);
	event_logger logger(env, L"service_hosting");

	try
	{
		return run(env, logger);
	}
	catch (const winapi_exception &ex)
	{
		std::string what(ex.what());
		std::wstring message = std::wstring(what.begin(), what.end()) + L", " + winapi_error::create_message(ex.lastErrorCode());
		if (env.hasConsoleWindow())
		{
			std::wcerr << message << std::endl;
		}
		else
		{
			logger.log_error(message.c_str());
		}
		return -1;
	}
	catch (const std::exception &ex)
	{
		if (env.hasConsoleWindow())
		{
			std::wcerr << ex.what() << std::endl;
		}
		else
		{
			logger.log_error(ex.what());
		}
		return -1;
	}
}