#include <fstream>
#include <codecvt>
#include "nlohmann/json.hpp"
#include "configuration.h"

configuration::configuration(const environment &env)
	: _env(env)
{
}

void configuration::load()
{
	std::ifstream inputStream(_env.currentDirectoryPath() + L"\\service_hosting.json");
	if (!inputStream.is_open())
	{
		throw std::runtime_error("config file service_hosting.json not found");
	}

	nlohmann::json json;
	inputStream >> json;
	inputStream.close();

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf8converter;

	_serviceName = utf8converter.from_bytes(json["service_name"]);
	_serviceDisplayName = utf8converter.from_bytes(json["service_display_name"]);
	_serviceDescription = utf8converter.from_bytes(json["service_description"]);
	_executable = utf8converter.from_bytes(json["executable"]);
	_arguments = utf8converter.from_bytes(json["arguments"]);
}
