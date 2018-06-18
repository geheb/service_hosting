#pragma once
#include <string>
#include "service_base.h"
#include "environment.h"

class service_installer final
{
public:
	service_installer(const environment &env);
	virtual ~service_installer() {}
	void install(const service_base &service);
	void uninstall(const service_base &service);

private:
	void process_wait_for_exit(unsigned long processId);

	const environment &_env;
};

