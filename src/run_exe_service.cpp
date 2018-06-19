#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "targetver.h"
#include <Windows.h>
#include "run_exe_service.h"
#include "handle_deleter.h"
#include "winapi_error.h"

using namespace std::string_literals;

run_exe_service::run_exe_service(const environment &env, const configuration &config, const event_logger &logger)
	: service_base(config.serviceName(), config.serviceDisplayName(), config.serviceDescription(), logger)
	, _env(env)
{
	_commandLine = L"\"" + config.executable() + L"\" " + config.arguments();
	size_t posDirChar = config.executable().find_last_of(L"\\/");
	_workingDirectory = posDirChar != std::wstring::npos ? config.executable().substr(0, posDirChar) : env.currentDirectoryPath();
}

bool terminate_process_via_remote_thread(HANDLE targetProcessHandle, UINT exitCode, DWORD waitMilliseconds)
{
	HMODULE kernel32Handle = ::GetModuleHandle(L"kernel32");
	if (kernel32Handle == nullptr)
	{
		return false;
	}

	auto exitProcessProcAddress = (LPTHREAD_START_ROUTINE)::GetProcAddress(kernel32Handle, "ExitProcess");
	if (exitProcessProcAddress == nullptr)
	{
		return false;
	}

	HANDLE duplicateHandle;
	if (!::DuplicateHandle(::GetCurrentProcess(), targetProcessHandle, ::GetCurrentProcess(),
		&duplicateHandle, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		return false;
	}

	unique_handle processDuplicateHandle(duplicateHandle);

	unique_handle processRemoteThreadHandle(::CreateRemoteThread(duplicateHandle,
		nullptr, 0, exitProcessProcAddress, (LPVOID)(UINT_PTR)exitCode, 0, nullptr));

	if (processRemoteThreadHandle == nullptr)
	{
		return false;
	}

	return ::WaitForSingleObject(targetProcessHandle, waitMilliseconds) == WAIT_OBJECT_0;
}

void run_exe_service::on_start(const std::vector<std::wstring> &serviceArguments, HANDLE stopEvent)
{
	unsigned long startCounter = 0;
	STARTUPINFO startInfo;
	PROCESS_INFORMATION processInfo;
	unique_handle processHandle;

	auto commandLine = unique_wchar_t(_wcsdup(_commandLine.c_str()));

	while (true)
	{
		::ZeroMemory(&startInfo, sizeof(startInfo));
		startInfo.cb = sizeof(startInfo);
		::ZeroMemory(&processInfo, sizeof(processInfo));

		if (!::CreateProcess(nullptr, commandLine.get(), nullptr, nullptr, FALSE,
			CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
			nullptr, _workingDirectory.c_str(), &startInfo, &processInfo))
		{
			_logger.log_error(L"create child process failed, " + winapi_error::create_message(::GetLastError()));
			return;
		}

		startCounter++;

		::CloseHandle(processInfo.hThread);

		if (startCounter > 1)
		{
			_logger.log_info(L"child process \"" 
				+ _serviceName 
				+ L"\" restarted, current restart counter is " 
				+ std::to_wstring(startCounter - 1));
		}

		processHandle.reset(processInfo.hProcess);

		const HANDLE waitHandles[2] = { processInfo.hProcess, stopEvent };

		DWORD waitResult = ::WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
		if (waitResult == WAIT_OBJECT_0) // process exited
		{
			DWORD exitCode;
			std::wstringstream stream;
			stream << L"child process \"" << _serviceName << L"\" stopped, exit code ";
			if (::GetExitCodeProcess(processInfo.hProcess, &exitCode))
			{
				stream << L"0x" << std::hex << exitCode;
			}
			else
			{
				stream << L"unknown";
			}
			stream << L". Restart child process in 10 seconds...";
			_logger.log_warn(stream.str());
			std::this_thread::sleep_for(std::chrono::seconds(10));
		}
		else
		{
			::FreeConsole();
			if (::AttachConsole(processInfo.dwProcessId))
			{
				::SetConsoleCtrlHandler(NULL, TRUE);
				::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
				::FreeConsole();

				if (::WaitForSingleObject(processInfo.hProcess, 20000) == WAIT_OBJECT_0)
				{
					return;
				}
			}

			if (!terminate_process_via_remote_thread(processInfo.hProcess, 1, 10000))
			{
				::TerminateProcess(processInfo.hProcess, 1);
			}

			return;
		}
	}
}
