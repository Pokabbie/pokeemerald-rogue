#include "UserData.h"
#include "StringUtils.h"
#include "Log.h"

#include <cstdlib>
#include <filesystem>

#pragma warning( push )
#pragma warning( disable : 4244)

namespace fs = std::filesystem;

static std::wstring GetEnvVar(std::wstring const& var, std::wstring const& defaultVal)
{
	wchar_t* buf = nullptr;
	size_t sz = 0;
	if (_wdupenv_s(&buf, &sz, var.c_str()) == 0 && buf != nullptr)
	{
		std::wstring output = buf;
		free(buf);
		return output;
	}
	return defaultVal;
}

static std::wstring FormatPath(std::wstring const& path)
{
	std::wstring output;

	if (path.find_first_of(':') == std::wstring::npos)
	{
		// Relative path
		fs::path p = GetEnvVar(L"appdata", L"save") + L"/.pokabbie/rogue_assistant/" + path;
		p = std::filesystem::absolute(p);
		output = p;
	}
	else
	{
		// Already absolute path
		output = path;
	}

	strutil::replace_all(output, L"/", L"\\");
	strutil::replace_all(output, L"\\\\", L"\\"); // remove any double slashes
	return output;
}

bool UserData::DoesDirectoryExist(std::wstring const& path)
{
	fs::file_status status = fs::status(FormatPath(path));
	return fs::is_directory(status);
}

bool UserData::DoesFileExist(std::wstring const& path) 
{
	fs::file_status status = fs::status(FormatPath(path));
	return fs::is_regular_file(status);
}

static void EnsureParentDirectoriesExist(std::wstring const& path)
{
	std::wstring directoryPath = FormatPath(path);
	directoryPath = directoryPath.substr(0, directoryPath.find_last_of('\\'));

	if (!UserData::DoesDirectoryExist(directoryPath))
	{
		LOG_INFO("UserData::CreateDir %s", std::string(directoryPath.begin(), directoryPath.end()).c_str());
		fs::create_directories(directoryPath);
	}
}

bool UserData::TryOpenReadFile(std::wstring const& inPath, std::fstream& outStream)
{
	if (DoesFileExist(inPath))
	{
		std::wstring fullPath = FormatPath(inPath);
		EnsureParentDirectoriesExist(fullPath);

		LOG_INFO("UserData::OpenRead %s", std::string(fullPath.begin(), fullPath.end()).c_str());

		outStream.close();
		outStream.open(fullPath.c_str(), std::ios::binary | std::ios::in);
		return outStream.is_open();
	}

	return false;
}

bool UserData::TryOpenWriteFile(std::wstring const& inPath, std::fstream& outStream, bool createIfMissing)
{
	if (createIfMissing || DoesFileExist(inPath))
	{
		std::wstring fullPath = FormatPath(inPath);
		EnsureParentDirectoriesExist(fullPath);

		LOG_INFO("UserData::OpenWrite %s", std::string(fullPath.begin(), fullPath.end()).c_str());

		outStream.close();
		outStream.open(fullPath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
		return outStream.is_open();
	}

	return false;
}

bool UserData::TryOpenAppendFile(std::wstring const& inPath, std::fstream& outStream, bool createIfMissing)
{
	if (createIfMissing || DoesFileExist(inPath))
	{
		std::wstring fullPath = FormatPath(inPath);
		EnsureParentDirectoriesExist(fullPath);

		LOG_INFO("UserData::OpenWrite %s", std::string(fullPath.begin(), fullPath.end()).c_str());

		outStream.close();
		outStream.open(fullPath.c_str(), std::ios::out | std::ios::app);
		return outStream.is_open();
	}

	return false;
}

#pragma warning( pop )