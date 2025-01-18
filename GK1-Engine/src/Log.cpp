#include "Log.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>

bool Log::isInitialized = false;
std::ofstream Log::logFile;

#ifdef _WIN32
HANDLE Log::consoleHandle = nullptr;
FILE* Log::fpstdin = nullptr;
FILE* Log::fpstdout = nullptr;
FILE* Log::fpstderr = nullptr;

std::unordered_map<Log::LogLevel, Log::ConsoleColor> Log::colorMap =
{
	{LogLevel::Info,    {FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, "[INFO]"}},
	{LogLevel::Warning, {FOREGROUND_RED | FOREGROUND_GREEN, "[WARNING]"}},
	{LogLevel::Error,   {FOREGROUND_RED | FOREGROUND_INTENSITY, "[ERROR]"}}
};
#endif


void Log::CreateConsole()
{
#if defined(_DEBUG) && defined(_WIN32)
	if (!AllocConsole()) return;

	freopen_s(&fpstdin, "CONIN$", "r", stdin);
	freopen_s(&fpstdout, "CONOUT$", "w", stdout);
	freopen_s(&fpstderr, "CONOUT$", "w", stderr);

	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitleA("Debug Console");
#endif
}

std::string Log::GetTimestamp()
{
	auto now = std::time(nullptr);
	auto tm = *std::localtime(&now);
	std::stringstream ss;
	ss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S]");
	return ss.str();
}

void Log::WriteToFile(LogLevel level, const std::string& message)
{
	if (logFile.is_open())
	{
		const auto& colorInfo = colorMap[level];
		logFile << GetTimestamp() << " | " << std::setw(9) << std::left << colorInfo.prefix << " | " << message << std::endl;
		logFile.flush();
	}
}

void Log::WriteToConsole(LogLevel level, const std::string& message)
{
#if defined(_DEBUG) && defined(_WIN32)
	if (consoleHandle)
	{
		const auto& colorInfo = colorMap[level];

		SetConsoleTextAttribute(consoleHandle, colorInfo.color);
		std::cout << GetTimestamp() << " | " << std::setw(9) << std::left << colorInfo.prefix << " | " << message << std::endl;
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
#endif
}

void Log::Init(const std::string& filename)
{
	if (!isInitialized)
	{
		std::filesystem::create_directories("logs");

		auto now = std::time(nullptr);
		auto tm = *std::localtime(&now);
		std::stringstream ss;
		ss << "logs/" << std::put_time(&tm, "%Y%m%d_") << filename;

		logFile.open(ss.str(), std::ios::app);

		CreateConsole();

		isInitialized = true;
	}
}

void Log::Info(const std::string& message)
{
	WriteToFile(LogLevel::Info, message);
	WriteToConsole(LogLevel::Info, message);
}

void Log::Warning(const std::string& message)
{
	WriteToFile(LogLevel::Warning, message);
	WriteToConsole(LogLevel::Warning, message);
}

void Log::Error(const std::string& message)
{
	WriteToFile(LogLevel::Error, message);
	WriteToConsole(LogLevel::Error, message);
}

void Log::Shutdown()
{
	if (isInitialized)
	{
		if (logFile.is_open())
		{
			logFile.close();
		}

#if defined(_DEBUG) && defined(_WIN32)
		if (fpstdin) fclose(fpstdin);
		if (fpstdout) fclose(fpstdout);
		if (fpstderr) fclose(fpstderr);
		FreeConsole();
#endif

		isInitialized = false;
	}
}