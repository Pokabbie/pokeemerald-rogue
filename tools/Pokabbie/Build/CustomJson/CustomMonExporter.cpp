#include "main.h"

#include <functional>
#include <sstream>
#include <vector>

static std::string FormatName(std::string const& prettyName)
{
	std::string name = strutil::to_upper(prettyName);
	strutil::replace_all(name, " ", "_");
	strutil::replace_all(name, "!", "EMARK");
	strutil::replace_all(name, "?", "QMARK");
	strutil::replace_all(name, ",", "");
	strutil::replace_all(name, ".", "");
	strutil::replace_all(name, "\"", "");
	strutil::replace_all(name, "'", "");
	strutil::replace_all(name, "+", "PLUS");
	strutil::replace_all(name, c_Elipsies, "");
	return name;
}

struct TrainerInfo
{
	std::string name;
	std::string trainerId;
	std::string trainerColour;
	std::string preprocessorCondition;

	inline std::string GetCodeId() const
	{
		return FormatName(name);
	}
};

struct CustomMonInfo
{
	std::string species;
	std::string nickname;
	std::string pokeball;
	std::string shiny;
	std::string heldItem;
	std::string isDefaultSpawn;
	TrainerInfo const* trainer;
	std::vector<std::string> moves;
	std::vector<std::string> abilities;
	std::string preprocessorCondition;

	inline std::string GetCodeId() const
	{
		std::string monName = species.substr(std::string("SPECIES_").size());

		if(trainer != nullptr)
			return FormatName(trainer->name + " " + monName);
		else
			return FormatName("NONE_" + monName);
	}

	inline std::string GetTrainerCodeId() const
	{
		if (trainer != nullptr)
			return trainer->GetCodeId();
		else
			return "NONE";
	}
};

struct CustomMonData
{
	std::vector<TrainerInfo> trainers;
	std::vector<CustomMonInfo> mons;
};

static void GatherCustomMons(std::string const& dataPath, json const& jsonData, CustomMonData& outQuestData);

void ExportCustomMonData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	CustomMonData monData;
	GatherCustomMons(dataPath, jsonData, monData);

	// Trainer params
	for (auto it = monData.trainers.begin(); it != monData.trainers.end(); ++it)
	{
		auto const& trainer = *it;

		if (!trainer.preprocessorCondition.empty())
			fileStream << "#if " << trainer.preprocessorCondition << "\n";

		fileStream << "#define TRAINER_ID_" << trainer.GetCodeId() << " (OTID_FLAG_CUSTOM_MON | " << trainer.trainerId << ")\n";

		fileStream << "#ifdef ROGUE_EXPANSION\n";
		fileStream << "static u8 const sTrainerName_" << trainer.GetCodeId() << "[PLAYER_NAME_LENGTH + 1] = _(\"" << trainer.name << "\");\n";
		fileStream << "#else\n";
		fileStream << "static u8 const sTrainerName_" << trainer.GetCodeId() << "[PLAYER_NAME_LENGTH + 1] = _(\"" << strutil::to_upper(trainer.name) << "\");\n";
		fileStream << "#endif\n";

		if (!trainer.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "\n";

	// Mon params
	for (auto it = monData.mons.begin(); it != monData.mons.end(); ++it)
	{
		auto const& mon = *it;

		if (!mon.preprocessorCondition.empty())
			fileStream << "#if " << mon.preprocessorCondition << "\n";

		if (!mon.nickname.empty())
		{
			fileStream << "#ifdef ROGUE_EXPANSION\n";
			fileStream << "static u8 const sMonNickname_" << mon.GetCodeId() << "[] = _(\"" << mon.nickname << "\");\n";
			fileStream << "#else\n";
			fileStream << "static u8 const sMonNickname_" << mon.GetCodeId() << "[] = _(\"" << strutil::to_upper(mon.nickname) << "\");\n";
			fileStream << "#endif\n";
		}

		if (!mon.moves.empty())
		{
			fileStream << "static u16 const sMonMoves_" << mon.GetCodeId() << "[] = {\n";

			for (auto const& move : mon.moves)
				fileStream << c_TabSpacing << move << ",\n";

			fileStream << "};\n";
		}

		if (!mon.abilities.empty())
		{
			fileStream << "static u16 const sMonAbilities_" << mon.GetCodeId() << "[NUM_ABILITY_SLOTS] = {\n";

			for (auto const& ability : mon.abilities)
				fileStream << c_TabSpacing << ability << ",\n";

			fileStream << "};\n";
		}

		if (!mon.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "\n";
	

	// Trainer data
	fileStream << "static struct CustomTrainerData const sCustomTrainers[] = \n{\n";
	fileStream << c_TabSpacing << "[CUSTOM_TRAINER_NONE] = {},\n";
	for (auto it = monData.trainers.begin(); it != monData.trainers.end(); ++it)
	{
		auto const& trainer = *it;

		if (!trainer.preprocessorCondition.empty())
			fileStream << "#if " << trainer.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "[CUSTOM_TRAINER_" << trainer.GetCodeId() << "]\n";
		fileStream << c_TabSpacing << "{\n";
		fileStream << c_TabSpacing2 << ".name = sTrainerName_" << trainer.GetCodeId() << ",\n";
		fileStream << c_TabSpacing2 << ".trainerId = TRAINER_ID_" << trainer.GetCodeId() << ",\n";
		fileStream << c_TabSpacing2 << ".trainerColour = " << trainer.trainerColour << ",\n";
		fileStream << c_TabSpacing << "},\n";

		if (!trainer.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "};\n\n";

	// Mon data
	fileStream << "static struct CustomMonData const sCustomPokemon[] = \n{\n";
	fileStream << c_TabSpacing << "[CUSTOM_MON_NONE] = {},\n";
	for (auto it = monData.mons.begin(); it != monData.mons.end(); ++it)
	{
		auto const& mon = *it;

		if (!mon.preprocessorCondition.empty())
			fileStream << "#if " << mon.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "[CUSTOM_MON_" << mon.GetCodeId() << "]\n";
		fileStream << c_TabSpacing << "{\n";
		fileStream << c_TabSpacing2 << ".species = " << mon.species << ",\n";
		fileStream << c_TabSpacing2 << ".pokeball = " << mon.pokeball << ",\n";
		fileStream << c_TabSpacing2 << ".isShiny = " << mon.shiny << ",\n";
		fileStream << c_TabSpacing2 << ".heldItem = " << mon.heldItem << ",\n";
		fileStream << c_TabSpacing2 << ".isDefaultSpawn = " << mon.isDefaultSpawn << ",\n";
		fileStream << c_TabSpacing2 << ".customTrainerId = CUSTOM_TRAINER_" << mon.GetTrainerCodeId() << ",\n";
		fileStream << c_TabSpacing2 << ".otId = TRAINER_ID_" << mon.GetTrainerCodeId() << ",\n";

		if (!mon.nickname.empty())
			fileStream << c_TabSpacing2 << ".nickname = sMonNickname_" << mon.GetCodeId() << ",\n";
		else
			fileStream << c_TabSpacing2 << ".nickname = NULL,\n";

		if (!mon.moves.empty())
		{
			fileStream << c_TabSpacing2 << ".moves = sMonMoves_" << mon.GetCodeId() << ",\n";
			fileStream << c_TabSpacing2 << ".movesCount = ARRAY_COUNT(sMonMoves_" << mon.GetCodeId() << "),\n";
		}
		else
		{
			fileStream << c_TabSpacing2 << ".moves = NULL,\n";
			fileStream << c_TabSpacing2 << ".movesCount = 0,\n";
		}

		if (!mon.abilities.empty())
			fileStream << c_TabSpacing2 << ".abilities = sMonAbilities_" << mon.GetCodeId() << ",\n";
		else
			fileStream << c_TabSpacing2 << ".abilities = NULL,\n";

		fileStream << c_TabSpacing << "},\n";

		if (!mon.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "};\n\n";
}

void ExportCustomMonData_H(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	CustomMonData monData;
	GatherCustomMons(dataPath, jsonData, monData);

	// Trainer define
	fileStream << "enum\n{\n";
	fileStream << c_TabSpacing << "CUSTOM_TRAINER_NONE,\n";
	for (auto it = monData.trainers.begin(); it != monData.trainers.end(); ++it)
	{
		auto const& trainer = *it;

		if (!trainer.preprocessorCondition.empty())
			fileStream << "#if " << trainer.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "CUSTOM_TRAINER_" << trainer.GetCodeId() << ",\n";

		if (!trainer.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << c_TabSpacing << "CUSTOM_TRAINER_COUNT,\n";
	fileStream << "};\n\n";

	// Mon define
	fileStream << "enum\n{\n";
	fileStream << c_TabSpacing << "CUSTOM_MON_NONE,\n";
	for (auto it = monData.mons.begin(); it != monData.mons.end(); ++it)
	{
		auto const& mon = *it;

		if (!mon.preprocessorCondition.empty())
			fileStream << "#if " << mon.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "CUSTOM_MON_" << mon.GetCodeId() << ",\n";

		if (!mon.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << c_TabSpacing << "CUSTOM_MON_COUNT,\n";
	fileStream << "};\n\n";
}

static std::string GetAsString(json const& jsonValue)
{
	if (jsonValue.is_boolean())
		return jsonValue.get<bool>() ? "TRUE" : "FALSE";

	if (jsonValue.is_number_integer())
		return std::to_string(jsonValue.get<int>());

	if (jsonValue.is_number_unsigned())
		return std::to_string(jsonValue.get<unsigned int>());

	if (jsonValue.is_number_float())
		return std::to_string(jsonValue.get<float>());

	return jsonValue.get<std::string>();
}

static void GatherCustomMons(std::string const& dataPath, json const& rawJsonData, CustomMonData& outMonData)
{
	json trainerData = ExpandCommonArrayGroup(dataPath, rawJsonData, "trainer_data")["trainer_data"];
	json monData = ExpandCommonArrayGroup(dataPath, rawJsonData, "mon_data")["mon_data"];

	std::hash<std::string> hasher;

	for (auto trainerGroupIt = trainerData.begin(); trainerGroupIt != trainerData.end(); ++trainerGroupIt)
	{
		json trainers = trainerGroupIt.value();

		for (auto trainerIt : trainers)
		{
			TrainerInfo trainerInfo;

			trainerInfo.name = GetAsString(trainerIt["name"]);

			if (trainerIt.contains("#if"))
				trainerInfo.preprocessorCondition = GetAsString(trainerIt["#if"]);
			else
				trainerInfo.preprocessorCondition = "";

			if (trainerIt.contains("trainer_id"))
				trainerInfo.trainerId = GetAsString(trainerIt["trainer_id"]);
			else
			{
				size_t trainerId = hasher(trainerInfo.name) % 65535;
				trainerInfo.trainerId = std::to_string(trainerId);
			}

			if(trainerIt.contains("trainer_colour"))
				trainerInfo.trainerColour = GetAsString(trainerIt["trainer_colour"]);
			else
				trainerInfo.trainerColour = (hasher(trainerInfo.trainerId) % 2) == 0 ? "0" : "1";

			outMonData.trainers.push_back(trainerInfo);
		}
	}

	for (auto monGroupIt = monData.begin(); monGroupIt != monData.end(); ++monGroupIt)
	{
		json mons = monGroupIt.value();

		for (auto monIt : mons)
		{
			CustomMonInfo monInfo;

			monInfo.species = GetAsString(monIt["species"]);

			if (monIt.contains("#if"))
				monInfo.preprocessorCondition = GetAsString(monIt["#if"]);
			else
				monInfo.preprocessorCondition = "";

			if (monIt.contains("nickname"))
				monInfo.nickname = GetAsString(monIt["nickname"]);
			else
				monInfo.nickname = "";

			if (monIt.contains("pokeball"))
				monInfo.pokeball = GetAsString(monIt["pokeball"]);
			else
				monInfo.pokeball = "ITEM_CUSTOM_MON_POKEBALL";

			if (monIt.contains("held_item"))
				monInfo.heldItem = GetAsString(monIt["held_item"]);
			else
				monInfo.heldItem = "ITEM_NONE";

			if (monIt.contains("is_shiny"))
				monInfo.shiny = GetAsString(monIt["is_shiny"]);
			else
				monInfo.shiny = "FALSE";

			if (monIt.contains("is_default_spawn"))
				monInfo.isDefaultSpawn = GetAsString(monIt["is_default_spawn"]);
			else
				monInfo.isDefaultSpawn = "FALSE";

			if (monIt.contains("trainer"))
			{
				std::string trainerName = GetAsString(monIt["trainer"]);

				auto trainerIt = std::find_if(outMonData.trainers.begin(), outMonData.trainers.end(), [trainerName](TrainerInfo const& trainerInfo)
					{
						return strutil::compare_ignore_case(trainerInfo.name, trainerName);
					});

				if(trainerIt == outMonData.trainers.end())
				{
					FATAL_ERROR("Couldn't find trainer '%s' for mon '%s:%s'", trainerName.c_str(), monInfo.nickname.c_str(), monInfo.species.c_str());
				}

				monInfo.trainer = &*trainerIt;
			}

			if (monIt.contains("moves"))
			{
				for (auto move : monIt["moves"])
				{
					monInfo.moves.push_back(GetAsString(move));
				}
			}

			if (monIt.contains("abilities"))
			{
				for (auto ability : monIt["abilities"])
				{
					monInfo.abilities.push_back(GetAsString(ability));
				}
			}

			outMonData.mons.push_back(monInfo);
		}
	}
}