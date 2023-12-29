#include "main.h"

#include <fstream>
#include <sstream>
#include <set>

#define POKEMON_NAME_LENGTH 10

static bool ParseToGbaLegal(std::wstring const& str, std::string& outStr)
{
	outStr = "";

	for (int i = 0; i < static_cast<int>(str.size()); ++i)
	{
		wchar_t c = str[i];

		outStr += static_cast<char>(c);

		//if (c >= L'a' && c <= L'z')
		//	continue;
		//
		//if (c >= L'A' && c <= L'Z')
		//	continue;
		//
		//if (c >= L'0' && c <= L'9')
		//	continue;
		//
		//if (c == L' ' || c == L'-' || c == L'_' || c == L'"' || c == L'\'' || c == L'=' || c == L',')
		//	continue;
		//
		//return false;
	}

	return true;
}

static bool isGbaLegal(std::string const& str)
{
	for (int i = 0; i < static_cast<int>(str.size()); ++i)
	{
		char c = str[i];

		if (c >= 'a' && c <= 'z')
			continue;

		if (c >= 'A' && c <= 'Z')
			continue;

		if (c >= '0' && c <= '9')
			continue;

		if (c == ' ')
			continue;

		return false;
	}

	return true;
}

void ExportNicknameData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	std::wifstream dataStream(dataPath);

	std::set<std::string> completeNicknameSet;
	std::map<std::string, std::set<std::string>> nicknameTable;

	std::string currentKey = "none";
	std::string line;
	std::wstring rawLine;

	while (std::getline(dataStream, rawLine))
	{
		//std::string prevLine = line;

		if (!ParseToGbaLegal(rawLine, line))
			continue;

		strutil::trim(line);
		//fileStream << "// consider " << line << "\n";

		// Reading new table
		if (strutil::starts_with(line, "=="))
		{
			line = line.substr(2);
			strutil::trim_left(line);

			currentKey = line;
		}
		else
		{
			strutil::replace_all(line, "'", "");
			strutil::replace_all(line, "\"", "");
			strutil::replace_all(line, "-", " ");
			strutil::replace_all(line, ",", "");

			strutil::replace_all(line, "\t", " ");
			strutil::replace_all(line, "  ", " ");
			strutil::replace_all(line, "  ", " ");
			strutil::replace_all(line, "  ", " ");

			bool capitalizeNext = true;
			for (int i = 0; i < static_cast<int>(line.size()); ++i)
			{
				char c = line[i];

				if (c == ' ')
				{
					capitalizeNext = true;
				}
				else if (capitalizeNext)
				{
					line[i] = (char)toupper(c);
					capitalizeNext = false;
				}
			}

			if (line.size() > POKEMON_NAME_LENGTH)
			{
				// Attempt to remove white spaces
				strutil::replace_all(line, " ", "");
			}

			// Add to the set if it still fits
			if (line.size() != 0 && line.size() <= POKEMON_NAME_LENGTH && isGbaLegal(line))
			{
				nicknameTable[currentKey].insert(line);
				completeNicknameSet.insert(line);
			}
		}
	}

	for (auto it = completeNicknameSet.begin(); it != completeNicknameSet.end(); ++it)
	{
		std::string key = *it;
		strutil::replace_all(key, " ", "_");

		fileStream << "static const u8 sTextNickname_" << key << "[POKEMON_NAME_LENGTH + 1] = _(\"" << *it << "\");\n";
	}

	fileStream << "\n";

	{
		std::string tableKey = "Global";
		strutil::replace_all(tableKey, " ", "_");

		fileStream << "static const u8* const sNicknameTable_" << tableKey << "[] =\n{\n";

		for (auto it = completeNicknameSet.begin(); it != completeNicknameSet.end(); ++it)
		{
			std::string key = *it;
			strutil::replace_all(key, " ", "_");

			fileStream << "\tsTextNickname_" << key << ",\n";
		}
	}

	fileStream << "};\n";

	fileStream << "\n";
	//for (auto tableIt = nicknameTable.begin(); tableIt != nicknameTable.end(); ++tableIt)
	//{
	//	std::string tableKey = tableIt->first;
	//	strutil::replace_all(tableKey, " ", "_");
	//
	//	fileStream << "static const u8* const sNicknameTable_" << tableKey << "[] =\n{\n";
	//
	//	for (auto it = tableIt->second.begin(); it != tableIt->second.end(); ++it)
	//	{
	//		std::string key = *it;
	//		strutil::replace_all(key, " ", "_");
	//
	//		fileStream << "\tsTextNickname_" << key << ",\n";
	//	}
	//
	//	fileStream << "};\n";
	//}
}