#pragma once
#include "Defines.h"

#include <fstream>
#include <string>

class UserData
{
public:
	static bool DoesDirectoryExist(std::wstring const& path);
	static bool DoesFileExist(std::wstring const& path);

	static bool TryOpenReadFile(std::wstring const& path, std::fstream& outStream);
	static bool TryOpenWriteFile(std::wstring const& path, std::fstream& outStream, bool createIfMissing = true);
	static bool TryOpenAppendFile(std::wstring const& path, std::fstream& outStream, bool createIfMissing = true);

private:
};
