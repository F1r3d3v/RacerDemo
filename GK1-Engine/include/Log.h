#pragma once
#include <string>
#include <fstream>
#include <unordered_map>

#ifdef _WIN32
#include <Windows.h>
#endif

class Log
{
private:
	static bool isInitialized;
	static std::ofstream logFile;
	enum class LogLevel
	{
		Info,
		Warning,
		Error
	};

#ifdef _WIN32
	static HANDLE consoleHandle;
	static FILE* fpstdin;
	static FILE* fpstdout;
	static FILE* fpstderr;

	struct ConsoleColor
	{
		WORD color;
		std::string prefix;
	};

	static std::unordered_map<LogLevel, ConsoleColor> colorMap;
#endif

	Log() = default;

	static void CreateConsole();
	static std::string GetTimestamp();
	static void WriteToFile(LogLevel level, const std::string& message);
	static void WriteToConsole(LogLevel level, const std::string& message);

public:
	static void Init(const std::string& filename = "app.log");
	static void Shutdown();

	static void Info(const std::string& message);
	static void Warning(const std::string& message);
	static void Error(const std::string& message);
};