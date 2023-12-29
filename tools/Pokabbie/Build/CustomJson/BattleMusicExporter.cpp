#include "main.h"

#include <sstream>

void ExportBattleMusicData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	json musicPlayers = jsonData["music_players"];

	// Enum define
	//int counter = 0;
	fileStream << "enum\n{\n";
	fileStream << c_TabSpacing << "_BATTLE_MUSIC_UNKNOWN_START = BATTLE_MUSIC_UNKNOWN_START - 1,\n";
	for (auto it = musicPlayers.begin(); it != musicPlayers.end(); ++it)
	{
		fileStream << "#ifndef BATTLE_MUSIC_" << strutil::to_upper(it.key()) << "\n";
		fileStream << c_TabSpacing << "BATTLE_MUSIC_" << strutil::to_upper(it.key()) << ",\n";
		fileStream << "#endif\n";
	}
	fileStream << "};\n\n";

	// Redirects
	int redirectCounter = 0;
	for (auto it = musicPlayers.begin(); it != musicPlayers.end(); ++it)
	{
		auto value = it.value();

		if (value.contains("conditional_redirects"))
		{
			int redirectIndex = redirectCounter++;
			fileStream << "static const struct RogueBattleMusicRedirect gRogueTrainerMusicRedirect_" << redirectIndex << "[] =\n{\n";

			for (auto redirect : value["conditional_redirects"])
			{
				if(redirect.contains("base_species"))
					fileStream << "#ifdef SPECIES_" << redirect["base_species"].get<std::string>() << "\n";

				fileStream << c_TabSpacing << "{\n";

				if (redirect.contains("trainer_class"))
				{
					fileStream << c_TabSpacing << ".redirectType = REDIRECT_PARAM_TRAINER_CLASS,\n";
					fileStream << c_TabSpacing << ".redirectParam = TRAINER_CLASS_" << redirect["trainer_class"].get<std::string>() << ",\n";
				}
				else if(redirect.contains("base_species"))
				{
					fileStream << c_TabSpacing << ".redirectType = REDIRECT_PARAM_SPECIES,\n";
					fileStream << c_TabSpacing << ".redirectParam = SPECIES_" << redirect["base_species"].get<std::string>() << ",\n";
				}
				else
				{
					fileStream << c_TabSpacing << ".redirectType = REDIRECT_PARAM_NONE,\n";
					fileStream << c_TabSpacing << ".redirectParam = 0,\n";
				}

				fileStream << c_TabSpacing << ".musicPlayer = BATTLE_MUSIC_" << strutil::to_upper(redirect["music_player"].get<std::string>()) << ",\n";
				fileStream << c_TabSpacing << "},\n";

				if(redirect.contains("base_species"))
					fileStream << "#endif\n";
			}
			fileStream << "};\n\n";
		}
	}
	fileStream << "\n";

	// Music table
	redirectCounter = 0;
	fileStream << "const struct RogueBattleMusic gRogueTrainerMusic[] =\n{\n";
	for (auto it = musicPlayers.begin(); it != musicPlayers.end(); ++it)
	{
		auto value = it.value();

		fileStream << c_TabSpacing << "[BATTLE_MUSIC_" << strutil::to_upper(it.key()) << "]\n{\n";

		if(value.contains("encounter"))
			fileStream << c_TabSpacing << ".encounterMusic = " << value["encounter"].get<std::string>() << ",\n";
		else
			fileStream << c_TabSpacing << ".encounterMusic = MUS_NONE,\n";

		if (value.contains("battle"))
			fileStream << c_TabSpacing << ".battleMusic = " << value["battle"].get<std::string>() << ",\n";
		else
			fileStream << c_TabSpacing << ".battleMusic = MUS_NONE,\n";

		if (value.contains("victory"))
			fileStream << c_TabSpacing << ".victoryMusic = " << value["victory"].get<std::string>() << ",\n";
		else
			fileStream << c_TabSpacing << ".victoryMusic = MUS_NONE,\n";

		if (value.contains("conditional_redirects"))
		{
			int redirectIndex = redirectCounter++;

			fileStream << c_TabSpacing << ".redirects = gRogueTrainerMusicRedirect_" << redirectIndex << ",\n";
			fileStream << c_TabSpacing << ".redirectCount = ARRAY_COUNT(gRogueTrainerMusicRedirect_" << redirectIndex << "),\n";
		}

		fileStream << c_TabSpacing << "},\n";
	}
	fileStream << "};\n\n";

	//fileStream << "#undef RESOLVE\n";
}