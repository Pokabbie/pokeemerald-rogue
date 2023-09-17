#include "main.h"

#include <sstream>

struct TrainerDataExport_C
{
	std::stringstream earlyBlock;
	std::stringstream trainerStructsBlock;
};

static void ExportTrainerGroupData_C(TrainerDataExport_C& exporter, json const& jsonData, std::string trainerGroup);
static void ExportQueryScriptData_C(TrainerDataExport_C& exporter, std::string const& exportName, json const& jsonData);

void ExportTrainerData_C(std::ofstream& fileStream, json const& jsonData)
{
	TrainerDataExport_C exporter;

	json trainers = jsonData["trainers"];
	for (auto it = trainers.begin(); it != trainers.end(); ++it)
	{
		ExportTrainerGroupData_C(exporter, jsonData, it.key());
	}

	fileStream << exporter.earlyBlock.str() << '\n';

	fileStream << "const struct RogueTrainer gRogueTrainers" << "[] = \n{\n";
	fileStream << exporter.trainerStructsBlock.str() << '\n';
	fileStream << "};\n\n";
	fileStream << "const u16 gRogueTrainerCount = ARRAY_COUNT(gRogueTrainers);\n";
}

static void ExportTrainerGroupData_C(TrainerDataExport_C& exporter, json const& jsonData, std::string trainerGroup)
{
	json trainers = jsonData["trainers"][trainerGroup];
	int i;

	std::string trainerGroupPrefix = "// START " + trainerGroup + "\n";
	std::string trainerGroupSuffix = "// END " + trainerGroup + "\n";

	if (strutil::starts_with(trainerGroup, "#if"))
	{
		trainerGroupPrefix = trainerGroup + "\n";
		trainerGroupSuffix = "#endif\n";

		strutil::replace_all(trainerGroup, "#if", "");
	}

	// Ensure trainerGroup is safe to use as a codeToken
	strutil::replace_all(trainerGroup, " ", "_");
	strutil::replace_all(trainerGroup, "//", "_");

	exporter.earlyBlock << trainerGroupPrefix;
	exporter.trainerStructsBlock << trainerGroupPrefix;

	// Names
	i = 0;
	for (auto trainer : trainers)
	{
		int trainerIdx = i++;
		exporter.earlyBlock << "static const u8 sTrainerName_" << trainerGroup << "_" << trainerIdx << "[] = _(\"" << trainer["name"].get<std::string>() << "\");\n";
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

			exporter.earlyBlock << "\nstatic const struct RogueTeamGeneratorSubset sTrainerTeamSubsets_" << trainerSuffix << "[] =\n{\n";
			for (auto subset : subsets)
			{
				exporter.earlyBlock << c_TabSpacing << "{\n";
				exporter.earlyBlock << c_TabSpacing << ".maxSamples = " << subset["max_samples"].get<int>() << ",\n";

				// included types
				exporter.earlyBlock << c_TabSpacing << ".includedTypeMask = 0";
				if (subset.contains("include_types"))
				{
					for (auto type : subset["include_types"])
						exporter.earlyBlock << " | MON_TYPE_VAL_TO_FLAGS(TYPE_" << type.get<std::string>() << ")";
				}
				exporter.earlyBlock << ",\n";

				// excluded types
				exporter.earlyBlock << c_TabSpacing << ".excludedTypeMask = 0";
				if (subset.contains("exclude_types"))
				{
					for (auto type : subset["exclude_types"])
						exporter.earlyBlock << " | MON_TYPE_VAL_TO_FLAGS(TYPE_" << type.get<std::string>() << ")";
				}
				exporter.earlyBlock << ",\n";

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

		if (trainer.contains("gfx_suffix"))
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".objectEventGfx = OBJ_EVENT_GFX_" << trainer["gfx_suffix"].get<std::string>() << ",\n";
			exporter.trainerStructsBlock << c_TabSpacing << ".trainerPic = TRAINER_PIC_" << trainer["gfx_suffix"].get<std::string>() << ",\n";
		}
		else
		{
			exporter.trainerStructsBlock << c_TabSpacing << ".objectEventGfx = " << trainer["object_event"].get<std::string>() << ",\n";
			exporter.trainerStructsBlock << c_TabSpacing << ".trainerPic = " << trainer["trainer_pic"].get<std::string>() << ",\n";
		}

		// Weather
		if(trainer.contains("weather"))
			exporter.trainerStructsBlock << c_TabSpacing << ".preferredWeather = WEATHER_" << strutil::to_upper(trainer["weather"].get<std::string>()) << ",\n";
		else
			exporter.trainerStructsBlock << c_TabSpacing << ".preferredWeather = WEATHER_DEFAULT,\n";

		// Flags
		exporter.trainerStructsBlock << c_TabSpacing << ".trainerFlags = TRAINER_FLAG_NONE";
		for (auto flag : trainer["trainer_flags"])
		{
			exporter.trainerStructsBlock << " | TRAINER_FLAG_" << strutil::to_upper(flag.get<std::string>());
		}
		exporter.trainerStructsBlock << ",\n";


		// Team generator
		exporter.trainerStructsBlock << c_TabSpacing << ".teamGenerator = \n" << c_TabSpacing << "{\n";
		{
			json generator = trainer["team_generator"];

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

		exporter.earlyBlock << c_TabSpacing << cmd << ",\n";
	}

	exporter.earlyBlock << c_TabSpacing << "QUERY_SCRIPT_END\n};\n";
}