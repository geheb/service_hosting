#pragma once
#include <string>
#include <vector>

class environment final
{
public:
	environment(int argc, wchar_t* argv[]);
	const std::wstring& currentFilePath() const { return _currentFilePath; }
	const std::wstring& currentDirectoryPath() const { return _currentDirectoryPath; }
	bool hasConsoleWindow() const { return _hasConsoleWindow; }
	const std::vector<std::wstring>& arguments() const { return _arguments; }
	bool hasArgument(const std::wstring &arg) const;
	bool hasArguments() const { return !_arguments.empty(); }

private:
	std::wstring _currentFilePath;
	std::wstring _currentDirectoryPath;
	bool _hasConsoleWindow;
	std::vector<std::wstring> _arguments;
};

