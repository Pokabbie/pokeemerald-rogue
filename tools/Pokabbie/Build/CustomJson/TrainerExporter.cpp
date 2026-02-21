#include "main.h"

#include <sstream>

struct TrainerDataExport_C
{
	std::stringstream earlyBlock;
	std::stringstream trainerStructsBlock;
};

struct TrainerStrings
{
	std::vector<std::string> text;
	std::unordered_map<std::string, int> textToIndex;
};

static void ExportTrainerGroupData_C(TrainerDataExport_C& exporter, json const& jsonData, std::string trainerGroup);
static void ExportQueryScriptData_C(TrainerDataExport_C& exporter, std::string const& exportName, json const& jsonData);

static TrainerStrings ExtractTrainerStrings(json const& trainers);
static void ExportTrainerStringsData_C(TrainerDataExport_C& exporter, json const& trainers);


static json ExpandTrainersJson(std::string const& sourcePath, json const& rawData)
{
	return ExpandCommonArrayGroup(sourcePath, rawData, "trainers");
}

void ExportTrainerData_C(std::ofstream& fileStream, std::string const& dataPath, json const& rawJsonData)
{
	TrainerDataExport_C exporter;

	json jsonData = ExpandTrainersJson(dataPath, rawJsonData);

	// Recomment to debug
	//std::string expandedFile = jsonData.dump();

	json trainers = jsonData["trainers"];
	ExportTrainerStringsData_C(exporter, trainers);

	for (auto it = trainers.begin(); it != trainers.end(); ++it)
	{
		ExportTrainerGroupData_C(exporter, jsonData, it.key());
	}

	fileStream << exporter.earlyBlock.str() << '\n';

	fileStream << "const struct RogueTrainer gRogueTrainers" << "[] = \n{\n";
	fileStream << c_TabSpacing << "{}, // TRAINER_NONE\n";
	fileStream << exporter.trainerStructsBlock.str() << '\n';
	fileStream << "};\n\n";
	fileStream << "const u16 gRogueTrainerCount = ARRAY_COUNT(gRogueTrainers);\n";
}

static void ExportTrainerGroupData_C(TrainerDataExport_C& exporter, json const& jsonData, std::string trainerGroup)
{
	TrainerStrings trainerStrings = ExtractTrainerStrings(jsonData["trainers"]);
	json trainers = jsonData["trainers"][trainerGroup];
	int i;

	std::string trainerGroupPrefix = "// START " + trainerGroup + "\n";
	std::string trainerGroupSuffix = "// END " + trainerGroup + "\n";

	// Remove number prefix e.g. 00000_
	std::string trainerGroupRemovedPrefix = trainerGroup;
	size_t splitIndex = trainerGroup.find('_');
	if (splitIndex != std::string::npos)
	{
		std::string leftHalf = trainerGroup.substr(0, splitIndex);

		// Check the left half of the string only contains numbers (Could just be any old underscore
		bool isValid = true;
		for (char c : leftHalf)
		{
			if (!(c >= '0' && c <= '9'))
			{
				isValid = false;
				break;
			}
		}

		if(isValid)
			trainerGroupRemovedPrefix = trainerGroup.substr(splitIndex + 1);
	}

	if (strutil::starts_with(trainerGroupRemovedPrefix, "#if"))
	{
		trainerGroupPrefix = trainerGroupRemovedPrefix + "\n";
		trainerGroupSuffix = "#endif\n";

		strutil::replace_all(trainerGroup, "#if", "");
	}

	// Ensure trainerGroup is safe to use as a codeToken
	strutil::replace_all(trainerGroup, " ", "_");
	strutil::replace_all(trainerGroup, "/", "_");
	strutil::replace_all(trainerGroup, "\\", "_");
	strutil::replace_all(trainerGroup, "(", "_");
	strutil::replace_all(trainerGroup, ")", "_");
	strutil::replace_all(trainerGroup, "[", "_");
	strutil::replace_all(trainerGroup, "]", "_");
	strutil::replace_all(trainerGroup, ".", "_");
	strutil::replace_all(trainerGroup, "#", "_");

	exporter.earlyBlock << trainerGroupPrefix;
	exporter.trainerStructsBlock << trainerGroupPrefix;

	// Names
	i = 0;
	for (auto trainer : trainers)
	{
		int trainerIdx = i++;
		exporter.earlyBlock << "static const u8 sTrainerName_" << trainerGroup << "_" << trainerIdx << "[] = _(\"" << trainer["name"].get<std::string>() << "\");\n";

		if (trainer.contains("encounter_text"))
		{
			auto encounterTextArray = trainer["encounter_text"];
			exporter.earlyBlock << "static const u8* const sTrainerEncounterText_" << trainerGroup << "_" << trainerIdx << "[TRAINER_STRING_COUNT * " << encounterTextArray.size() << "] = \n{\n";

			int entryIdx = 0;
			for (auto encounterText : encounterTextArray)
			{
				for (auto entryIt = encounterText.begin(); entryIt != encounterText.end(); ++entryIt)
				{
					std::string key = entryIt.key();
					std::string text = entryIt.value().get<std::string>();
					int textIndex = trainerStrings.textToIndex[text];

					exporter.earlyBlock << c_TabSpacing << "[TRAINER_STRING_COUNT * " << entryIdx  << " + TRAINER_STRING_" << key << "] = gTrainerEncounterText_" << textIndex << ",\n";
				}
				++entryIdx;
			}

			exporter.earlyBlock << "};\n";
		}
	}
	exporter.earlyBlock << "\n";

	// Generator scripts
	i = 0;
	for (auto trainer : trainers)
	{
		int trainerIdx = i++;
		json generator = trainer["team_generator"];
		std::string trainerSuffix = trainerGroup + "_" + std::to_string(trainerIdx);

		if (generator.contains("query_script_override"))
		{
			ExportQueryScriptData_C(
				exporter,
				"sTrainerQueryScriptOverride_" + trainerSuffix,
				generator["query_script_override"]
			);
		}

		if (generator.contains("query_script_post"))
		{
			ExportQueryScriptData_C(
				exporter,
				"sTrainerQueryScriptPost_" + trainerSuffix,
				generator["query_script_post"]
			);
		}

		if (generator.contains("weight_script"))
		{
			ExportQueryScriptData_C(
				exporter,
				"sTrainerWeightScript_" + trainerSuffix,
				generator["weight_script"]
			);
		}

		// Subsets
		{
			json subsets = generator["subsets"];
			int counter = 0;

			// Additional Species lists
			for (auto subset : subsets)
			{
				int subsetIndex = counter++;

				if (subset.contains("extra_species"))
				{
					exporter.earlyBlock << "\nstatic u16 const sTrainerTeamSubsetsAdditionalSpecies_" << trainerSuffix << "_" << subsetIndex << "[] =\n{\n";

					for (auto speciesJson : subset["extra_species"])
					{
						std::string species = speciesJson.get<std::string>();

						if(strutil::starts_with(species, "#"))
							exporter.earlyBlock << species << "\n";
						else
							exporter.earlyBlock << "\t" << species << ",\n";
					}

					exporter.earlyBlock << "};\n";
				}
			}

			counter = 0;
			exporter.earlyBlock << "\nstatic const struct RogueTeamGeneratorSubset sTrainerTeamSubsets_" << trainerSuffix << "[] =\n{\n";
			for (auto subset : subsets)
			{
				int subsetIndex = counter++;
				exporter.earlyBlock << c_TabSpacing << "{\n";
				exporter.earlyBlock << c_TabSpacing << ".maxSamples = " << subset["max_samples"].get<int>() << ",\n";

				if (subset.contains("is_diversity_subset"))
					exporter.earlyBlock << c_TabSpacing << ".isDiversitySubset = " << subset["is_diversity_subset"].get<bool>() << ",\n";
				else
					exporter.earlyBlock << c_TabSpacing << ".isDiversitySubset = FALSE,\n";

				if (subset.contains("allow_duplicates"))
					exporter.earlyBlock << c_TabSpacing << ".allowSpeciesDuplicates = " << subset["allow_duplicates"].get<bool>() << ",\n";
				else
					exporter.earlyBlock << c_TabSpacing << ".allowSpeciesDuplicates = FALSE,\n";

				// included types
				exporter.earlyBlock << c_TabSpacing << ".includedTypeMask = 0";
				if (subset.contains("include_types"))
				{
					for (auto type : subset["include_types"])
					{
						if(type.get<std::string>() == "FAIRY")
							exporter.earlyBlock << c_TabSpacing2 << "\n#ifdef ROGUE_EXPANSION";

						exporter.earlyBlock << "\n" << c_TabSpacing2 << "| MON_TYPE_VAL_TO_FLAGS(TYPE_" << type.get<std::string>() << ")";

						if(type.get<std::string>() == "FAIRY")
							exporter.earlyBlock << c_TabSpacing2 << "\n#endif\n";
					}
				}
				exporter.earlyBlock << ",\n";

				// excluded types
				exporter.earlyBlock << c_TabSpacing << ".excludedTypeMask = 0";
				if (subset.contains("exclude_types"))
				{
					for (auto type : subset["exclude_types"])
					{
						if(type.get<std::string>() == "FAIRY")
							exporter.earlyBlock << c_TabSpacing2 << "\n#ifdef ROGUE_EXPANSION";

						exporter.earlyBlock << "\n" << c_TabSpacing2 << "| MON_TYPE_VAL_TO_FLAGS(TYPE_" << type.get<std::string>() << ")";

						if(type.get<std::string>() == "FAIRY")
							exporter.earlyBlock << c_TabSpacing2 << "\n#endif\n";
					}
				}
				exporter.earlyBlock << ",\n";

				// included gens
				exporter.earlyBlock << c_TabSpacing << ".includedGenMask = 0";
				if (subset.contains("include_gens"))
				{
					for (auto gen : subset["include_gens"])
						exporter.earlyBlock << " | MON_GEN_TO_FLAGS(" << gen.get<std::string>() << ")";
				}
				exporter.earlyBlock << ",\n";

				// excluded gens
				exporter.earlyBlock << c_TabSpacing << ".excludedGenMask = 0";
				if (subset.contains("exclude_gens"))
				{
					for (auto gen : subset["exclude_gens"])
						exporter.earlyBlock << " | MON_GEN_TO_FLAGS(" << gen.get<std::string>() << ")";
				}
				exporter.earlyBlock << ",\n";

				// additional species
				if (subset.contains("extra_species"))
				{
					exporter.earlyBlock << c_TabSpacing << ".additionalSpecies = sTrainerTeamSubsetsAdditionalSpecies_" << trainerSuffix << "_" << subsetIndex << ",\n";
					exporter.earlyBlock << c_TabSpacing << ".additionalSpeciesCount = ARRAY_COUNT(sTrainerTeamSubsetsAdditionalSpecies_" << trainerSuffix << "_" << subsetIndex << "),\n";
				}
				else
				{
					exporter.earlyBlock << c_TabSpacing << ".additionalSpecies = NULL,\n";
					exporter.earlyBlock << c_TabSpacing << ".additionalSpeciesCount = 0,\n";
				}

				exporter.earlyBlock << c_TabSpacing << "},\n";

			}
			exporter.earlyBlock << "};\n";
		}
	}
	exporter.earlyBlock << "\n";

	// Generators

	// Trainer data
	i = 0;
	for (auto trainer : trainers)
	{
		int trainerIdx = i++;
		std::string trainerSuffix = trainerGroup + "_" + std::to_string(trainerIdx);

		exporter.trainerStructsBlock << c_TabSpacing << "{\n";

		exporter.trainerStructsBlock << c_TabSpacing << ".trainerName = sTrainerName_" << trainerSuffix << ",\n";
		exporter.trainerStructsBlock << c_TabSpacing << ".trainerClass = " << trainer["trainer_class"].get<std::string>() << ",\n";
		exporter.trainerStructsBlock << c_TabSpacing << ".musicPlayer = BATTLE_MUSIC_" << strutil::to_upper(trainer["music_player"].get<std::string>()) << ",\n";
		exporter.trainerStructsBlock << c_TabSpacing << ".typeAssignment = TYPE_" << strutil::to_upper(trainer["type_assignment"].get<std::string>()) << ",\n";

		if (trainer.contains("level_override"))
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".levelOverride = " << trainer["level_override"].get<int>() << ",\n";
		}
		else
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".levelOverride = 0,\n";
		}
		
		if (trainer.contains("type_group_override"))
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".typeAssignmentGroup = TYPE_" << strutil::to_upper(trainer["type_group_override"].get<std::string>()) << ",\n";
		}
		else
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".typeAssignmentGroup = TYPE_" << strutil::to_upper(trainer["type_assignment"].get<std::string>()) << ",\n";
		}

		// Expect to use gfx_suffix or manual declaration
		//
		if (trainer.contains("object_event"))
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".objectEventGfx = " << trainer["object_event"].get<std::string>() << ",\n";
		}
		else
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".objectEventGfx = OBJ_EVENT_GFX_" << trainer["gfx_suffix"].get<std::string>() << ",\n";
		}

		if (trainer.contains("trainer_pic"))
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".trainerPic = " << trainer["trainer_pic"].get<std::string>() << ",\n";
		}
		else
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".trainerPic = TRAINER_PIC_" << trainer["gfx_suffix"].get<std::string>() << ",\n";
		}


		// Weather
		if(trainer.contains("weather"))
			exporter.trainerStructsBlock << c_TabSpacing << ".preferredWeather = WEATHER_" << strutil::to_upper(trainer["weather"].get<std::string>()) << ",\n";
		else
			exporter.trainerStructsBlock << c_TabSpacing << ".preferredWeather = WEATHER_DEFAULT,\n";

		// Optional Shiny species
		if(trainer.contains("shiny_species"))
		{
			// In vanilla ignore species that don't exists; in EX enforce the species actually exists
			exporter.trainerStructsBlock << "#ifdef ROGUE_EXPANSION\n";
			exporter.trainerStructsBlock << c_TabSpacing << ".potentialShinySpecies = " << strutil::to_upper(trainer["shiny_species"].get<std::string>()) << ",\n";
			exporter.trainerStructsBlock << "#else\n";
			exporter.trainerStructsBlock << c_TabSpacing <<"#ifdef " << strutil::to_upper(trainer["shiny_species"].get<std::string>()) <<"\n";
			exporter.trainerStructsBlock << c_TabSpacing << ".potentialShinySpecies = " << strutil::to_upper(trainer["shiny_species"].get<std::string>()) << ",\n";
			exporter.trainerStructsBlock << c_TabSpacing <<"#endif\n";
			exporter.trainerStructsBlock << "#endif\n";
		}
		else
			exporter.trainerStructsBlock << c_TabSpacing << ".potentialShinySpecies = SPECIES_NONE,\n";

		// Optional Pokeball type
		if(trainer.contains("pokeball"))
			exporter.trainerStructsBlock << c_TabSpacing << ".preferredPokeballItem = ITEM_" << strutil::to_upper(trainer["pokeball"].get<std::string>()) << ",\n";
		else
			exporter.trainerStructsBlock << c_TabSpacing << ".preferredPokeballItem = ITEM_NONE,\n";

		// Flags
		exporter.trainerStructsBlock << c_TabSpacing << ".classFlags = CLASS_FLAG_NONE";
		for (auto flag : trainer["class_flags"])
		{
			exporter.trainerStructsBlock << " | CLASS_FLAG_" << strutil::to_upper(flag.get<std::string>());
		}
		exporter.trainerStructsBlock << ",\n";

		exporter.trainerStructsBlock << c_TabSpacing << ".trainerFlags = TRAINER_FLAG_NONE";
		for (auto flag : trainer["trainer_flags"])
		{
			exporter.trainerStructsBlock << " | TRAINER_FLAG_" << strutil::to_upper(flag.get<std::string>());
		}
		exporter.trainerStructsBlock << ",\n";

		// Strings
		if (trainer.contains("encounter_text"))
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".encounterText = sTrainerEncounterText_" << trainerGroup << "_" << trainerIdx << ",\n";
			exporter.trainerStructsBlock << c_TabSpacing << ".encounterTextCount = ARRAY_COUNT(sTrainerEncounterText_" << trainerGroup << "_" << trainerIdx << ") / TRAINER_STRING_COUNT,\n";
		}
		else
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".encounterText = NULL,\n";
			exporter.trainerStructsBlock << c_TabSpacing << ".encounterTextCount = 0,\n";
		}


		// Team generator
		exporter.trainerStructsBlock << c_TabSpacing << ".teamGenerator = \n" << c_TabSpacing << "{\n";
		{
			json generator = trainer["team_generator"];

			exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".preferredGender = " << trainer["preferred_mon_gender"].get <std::string>() << ",\n";

			if (generator.contains("query_script_override"))
				exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".queryScriptOverride = " << "sTrainerQueryScriptOverride_" << trainerSuffix << ",\n";
			else
				exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".queryScriptOverride = NULL,\n";

			if (generator.contains("query_script_post"))
				exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".queryScriptPost = " << "sTrainerQueryScriptPost_" << trainerSuffix << ",\n";
			else
				exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".queryScriptPost = NULL,\n";

			if (generator.contains("weight_script"))
				exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".weightScript = " << "sTrainerWeightScript_" << trainerSuffix << ",\n";
			else
				exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".weightScript = NULL,\n";

			exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".subsets = sTrainerTeamSubsets_" << trainerSuffix << ", \n";
			exporter.trainerStructsBlock << c_TabSpacing << c_TabSpacing << ".subsetCount = ARRAY_COUNT(sTrainerTeamSubsets_" << trainerSuffix << "), \n";
		}
		exporter.trainerStructsBlock << c_TabSpacing << "},\n";

		exporter.trainerStructsBlock << c_TabSpacing << "},\n";
	}

	exporter.earlyBlock << trainerGroupSuffix;
	exporter.trainerStructsBlock << trainerGroupSuffix;
}

static void ExportQueryScriptData_C(TrainerDataExport_C& exporter, std::string const& exportName, json const& jsonData)
{
	exporter.earlyBlock << "static const u16 " << exportName << "[] =\n{\n";

	for (auto cmdEntry : jsonData)
	{
		std::string cmd = cmdEntry.get<std::string>();

		if (strutil::starts_with(cmd, "TYPE_")
			|| strutil::starts_with(cmd, "STAT_")
			|| strutil::starts_with(cmd, "SPECIES_")
			|| strutil::starts_with(cmd, "ITEM_")
			|| strutil::starts_with(cmd, "#")
			)
		{
			// do nothing, these are allowed constants we can reference
		}
		else if (strutil::starts_with(cmd, "VAR_"))
		{
			cmd = "QUERY_MASK_VAR_LOOKUP | QUERY_" + cmd;
		}
		else if (std::to_string(strutil::parse_string<int>(cmd)) == cmd)
		{
			// do nothing, this is a number
		}
		else
		{
			// Append query script, as this makes the json's slightly cleaner
			cmd = "QUERY_SCRIPT_" + cmd;
		}

		// Support proprecessor directives
		if (strutil::starts_with(cmd, "#"))
		{
			exporter.earlyBlock << cmd << "\n";
		}
		else
		{
			exporter.earlyBlock << c_TabSpacing << cmd << ",\n";
		}
	}

	exporter.earlyBlock << c_TabSpacing << "QUERY_SCRIPT_END\n};\n";
}

void ExportTrainerData_Pory(std::ofstream& fileStream, std::string const& dataPath, json const& rawJsonData)
{
	json jsonData = ExpandTrainersJson(dataPath, rawJsonData);

	TrainerStrings trainerStrings = ExtractTrainerStrings(jsonData["trainers"]);

	for (size_t i = 0; i < trainerStrings.text.size(); ++i)
	{
		fileStream << "text gTrainerEncounterText_" << i << "\n{\n";
		fileStream << c_TabSpacing << "format(\"" << trainerStrings.text[i] << "\")\n";
		fileStream << "}\n\n";
	}
}

static TrainerStrings ExtractTrainerStrings(json const& trainers)
{
	TrainerStrings trainerStrings;

	for (auto groupIt = trainers.begin(); groupIt != trainers.end(); ++groupIt)
	{
		for (auto trainer : groupIt.value())
		{
			if (trainer.contains("encounter_text"))
			{
				for (auto encounterText : trainer["encounter_text"])
				{
					for (auto entryIt = encounterText.begin(); entryIt != encounterText.end(); ++entryIt)
					{
						std::string text = entryIt.value().get<std::string>();
						strutil::replace_all(text, "...", c_Elipsies);

						auto findIt = trainerStrings.textToIndex.find(text);

						if (findIt == trainerStrings.textToIndex.end())
						{
							trainerStrings.textToIndex[text] = trainerStrings.text.size();
							trainerStrings.text.push_back(text);
						}
					}
				}
			}
		}
	}

	return trainerStrings;
}

static void ExportTrainerStringsData_C(TrainerDataExport_C& exporter, json const& trainers)
{
	TrainerStrings trainerStrings = ExtractTrainerStrings(trainers);

	for (size_t i = 0; i < trainerStrings.text.size(); ++i)
	{
		exporter.earlyBlock << "extern const u8 gTrainerEncounterText_" << i << "[];\n";
	}
	exporter.earlyBlock << "\n";
}