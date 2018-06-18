#pragma once
#include <string>
#include "configuration.h"
#include "service_base.h"
#include "non_copyable.h"
#include "environment.h"
#include "event_logger.h"

class run_exe_service final : public service_base
{
public:
	run_exe_service(const environment &env, const configuration &config, const event_logger &logger);

private:
	_NON_COPYABLE(run_exe_service);

	void on_start(const std::vector<std::wstring> &arguments, HANDLE stopEvent) override;

	std::wstring _executeFile;
	const environment &_env;
};

