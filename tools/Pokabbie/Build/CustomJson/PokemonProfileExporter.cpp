#include "main.h"

#include <filesystem>
#include <set>
#include <sstream>
#include <unordered_set>

extern std::string g_ExportFilePath;

struct CompetitiveSet
{
	std::vector<std::string> m_Moves;
	std::string m_Ability;
	std::string m_Item;
	std::string m_Nature;
	std::string m_HiddenPower;
	std::string m_TeraType;
	std::vector<std::string> m_SourceTiers;
};

struct Evolution
{
	std::string  m_Method;
	std::string  m_Param;
	std::string  m_Species;
};

struct LevelUpMove
{
	std::string m_Move;
	int m_Level;
};

struct BaseStats
{
	int m_HP;
	int m_Attack;
	int m_Defense;
	int m_Speed;
	int m_SpAttack;
	int m_SpDefense;
};

struct SpeciesProfile
{
	bool m_HasBaseStats = false;
	BaseStats m_BaseStats;
	std::vector<std::string> m_Types;
	std::vector<std::string> m_Abilities;
};

struct PokemonProfile
{
	std::vector<std::string> m_Species;
	std::vector<LevelUpMove> m_LevelUpMoves;
	std::vector<std::string> m_TutorMoves;
	std::vector<CompetitiveSet> m_CompetitiveSets;
	std::vector<Evolution> m_Evolutions;
	std::unordered_map<std::string, SpeciesProfile> m_PerSpeciesProfile;

	bool m_UseFallback_LevelUpMoves = false;
	bool m_UseFallback_TutorMoves = false;
	bool m_UseFallback_CompetitiveSets = false;

	bool HasLevelUpMove(std::string const& move)
	{
		return GetLevelUpMoveIndex(move) != (size_t)-1;
	}

	size_t GetLevelUpMoveIndex(std::string const& move)
	{
		for (size_t i = 0; i < m_LevelUpMoves.size(); ++i)
		{
			if (strutil::compare_ignore_case(m_LevelUpMoves[i].m_Move, move))
			{
				return i;
			}
		}

		return (size_t)-1;
	}

	bool HasTutorMove(std::string const& move)
	{
		return GetTutorMoveIndex(move) != (size_t)-1;
	}

	size_t GetTutorMoveIndex(std::string const& move)
	{
		for (size_t i = 0; i < m_TutorMoves.size(); ++i)
		{
			if (strutil::compare_ignore_case(m_TutorMoves[i], move))
			{
				return i;
			}
		}

		return (size_t)-1;
	}
};

static std::string GetAsString(json const& jsonValue)
{
	if (jsonValue.is_null())
		return "";

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

static std::string FormatKeyword(std::string keyword)
{
	strutil::trim(keyword);
	strutil::replace_all(keyword, ".", "");
	strutil::replace_all(keyword, ".", "");
	strutil::replace_all(keyword, "’", "");
	strutil::replace_all(keyword, "'", "");
	strutil::replace_all(keyword, "%", "");
	strutil::replace_all(keyword, ":", "");
	strutil::replace_all(keyword, " ", "_");
	strutil::replace_all(keyword, "-", "_");
	//strutil::replace_all(keyword, "é", "e");
	strutil::capitalize(keyword);
	return keyword;
}

void ParseProfile(std::string const& filePath, PokemonProfile& outProfile, PokemonProfile const* baseProfile)
{
	json data = ReadJsonFile(filePath);

	bool replace_LevelUpMoves = true;
	bool replace_TutorMoves = true;
	bool replace_CompetitiveSets = true;

	if (baseProfile)
	{
		outProfile = *baseProfile;

		if (data.contains("BuildSettings"))
		{
			json buildSettings = data["BuildSettings"];

			replace_LevelUpMoves = strutil::to_upper(GetAsString(buildSettings["LevelUpMoves"])) == "REPLACE";
			replace_TutorMoves = strutil::to_upper(GetAsString(buildSettings["TutorMoves"])) == "REPLACE";
			replace_CompetitiveSets = strutil::to_upper(GetAsString(buildSettings["CompetitiveSets"])) == "REPLACE";
		}
	}

	for (json species : data["Species"])
	{
		outProfile.m_Species.push_back(species.get<std::string>());
	}


	if (baseProfile)
	{
		if (data.contains("LevelUpMoves"))
		{
			if (replace_LevelUpMoves)
			{
				outProfile.m_LevelUpMoves.clear();
			}
		}
		else
		{
			outProfile.m_UseFallback_LevelUpMoves = true;
		}
	}

	for (json move : data["LevelUpMoves"])
	{
		LevelUpMove lvlMove = {
			move["Move"].get<std::string>(),
			move["Level"].get<int>()
		};

		if (outProfile.HasLevelUpMove(lvlMove.m_Move))
		{
			size_t i = outProfile.GetLevelUpMoveIndex(lvlMove.m_Move);
			outProfile.m_LevelUpMoves.erase(outProfile.m_LevelUpMoves.begin() + i);
		}

		outProfile.m_LevelUpMoves.push_back(lvlMove);
	}


	if (baseProfile)
	{
		if (data.contains("TutorMoves"))
		{
			if (replace_TutorMoves)
			{
				outProfile.m_TutorMoves.clear();
			}
		}
		else
		{
			outProfile.m_UseFallback_TutorMoves = true;
		}
	}

	for (json move : data["TutorMoves"])
	{
		if (outProfile.HasTutorMove(move.get<std::string>()))
		{
			size_t i = outProfile.GetTutorMoveIndex(move.get<std::string>());
			outProfile.m_TutorMoves.erase(outProfile.m_TutorMoves.begin() + i);
		}

		outProfile.m_TutorMoves.push_back(move.get<std::string>());
	}


	if (baseProfile)
	{
		if (data.contains("CompetitiveSets"))
		{
			if (replace_CompetitiveSets)
			{
				outProfile.m_CompetitiveSets.clear();
			}
		}
		else
		{
			outProfile.m_UseFallback_CompetitiveSets = true;
		}
	}

	for (json compSet : data["CompetitiveSets"])
	{
		CompetitiveSet outSet;

		for (json move : compSet["Moves"])
		{
			outSet.m_Moves.push_back(move.get<std::string>());
		}
		for (json tier : compSet["SourceTiers"])
		{
			outSet.m_SourceTiers.push_back(tier.get<std::string>());
		}

		outSet.m_Ability = GetAsString(compSet["Ability"]);
		outSet.m_Item = GetAsString(compSet["Item"]);
		outSet.m_Nature = GetAsString(compSet["Nature"]);
		outSet.m_HiddenPower = GetAsString(compSet["HiddenPower"]);
		outSet.m_TeraType = GetAsString(compSet["TeraType"]);

		outProfile.m_CompetitiveSets.push_back(outSet);
	}

	if (data.contains("Evolutions"))
	{
		for (json evoData : data["Evolutions"])
		{
			Evolution evo;
			evo.m_Method = GetAsString(evoData["Method"]);
			evo.m_Param = GetAsString(evoData["Param"]);
			evo.m_Species = GetAsString(evoData["Species"]);

			outProfile.m_Evolutions.push_back(evo);
		}
	}

	if (data.contains("PerSpecies"))
	{
		json perSpeciesData = data["PerSpecies"];

		for (std::string const& species : outProfile.m_Species)
		{
			if (!perSpeciesData.contains(species))
				continue;

			json speciesData = perSpeciesData[species];

			SpeciesProfile speciesProfile;

			if (speciesData.contains("BaseStats"))
			{
				json baseStats = speciesData["BaseStats"];

				speciesProfile.m_HasBaseStats = true;
				speciesProfile.m_BaseStats.m_HP = baseStats["HP"].get<int>();
				speciesProfile.m_BaseStats.m_Attack = baseStats["Attack"].get<int>();
				speciesProfile.m_BaseStats.m_Defense = baseStats["Defense"].get<int>();
				speciesProfile.m_BaseStats.m_Speed = baseStats["Speed"].get<int>();
				speciesProfile.m_BaseStats.m_SpAttack = baseStats["SpAttack"].get<int>();
				speciesProfile.m_BaseStats.m_SpDefense = baseStats["SpDefense"].get<int>();
			}

			if (speciesData.contains("Types"))
			{
				for (json type : speciesData["Types"])
				{
					speciesProfile.m_Types.push_back(GetAsString(type));
				}

				if (speciesProfile.m_Types.size() == 1)
				{
					speciesProfile.m_Types.push_back(speciesProfile.m_Types[0]);
				}
			}

			if (speciesData.contains("Abilities"))
			{
				for (json ability : speciesData["Abilities"])
				{
					speciesProfile.m_Abilities.push_back(GetAsString(ability));
				}
			}

			outProfile.m_PerSpeciesProfile[species] = speciesProfile;
		}
	}

	// Make sure all comp moves are added to tutor moves
	for (CompetitiveSet const& set : outProfile.m_CompetitiveSets)
	{
		for (std::string const& move : set.m_Moves)
		{
			if (!outProfile.HasLevelUpMove(move) && !outProfile.HasTutorMove(move))
			{
				outProfile.m_TutorMoves.push_back(move);
			}
		}
	}

	// Make sure moves are all sorted
	std::sort(outProfile.m_LevelUpMoves.begin(), outProfile.m_LevelUpMoves.end(), [](LevelUpMove const& lhs, LevelUpMove const& rhs)
		{
			return lhs.m_Level < rhs.m_Level;
		});

	std::sort(outProfile.m_TutorMoves.begin(), outProfile.m_TutorMoves.end(), [](std::string const& lhs, std::string const& rhs)
		{
			return lhs < rhs;
		});
}

std::vector<PokemonProfile> GatherProfiles(std::string const& dataPath)
{
	std::vector<PokemonProfile> output;

	std::vector<std::string> parts = strutil::split(dataPath, "*");
	std::string const& dirPath = parts[0];
	std::string filePattern = parts[1];
	std::unordered_set<std::string> speciesSet;

	// Are we attempting to parse revised mode data
	std::string revisedFilePattern = filePattern;
	bool isRevisedExport = false;

	if (strutil::ends_with(filePattern, "_revised.json"))
	{
		filePattern = filePattern.substr(0, filePattern.length() - 13) + ".json";
		isRevisedExport = true;
	}

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(dirPath))
	{
		if (dirEntry.is_regular_file())
		{
			if (strutil::ends_with(dirEntry.path().string(), filePattern))
			{
				std::string baseFilePath = dirEntry.path().string();
				std::string revisedFilePath = baseFilePath.substr(0, baseFilePath.size() - filePattern.size()) + revisedFilePattern;

				PokemonProfile profile;
				ParseProfile(baseFilePath, profile, nullptr);

				if (isRevisedExport && std::filesystem::exists(revisedFilePath))
				{
					PokemonProfile revisedProfile;
					ParseProfile(revisedFilePath, revisedProfile, &profile);

					output.push_back(revisedProfile);
					speciesSet.insert(revisedProfile.m_Species[0]);
				}
				else
				{
					output.push_back(profile);
					speciesSet.insert(profile.m_Species[0]);
				}
			}
		}
	}

	std::sort(output.begin(), output.end(), [](PokemonProfile const& lhs, PokemonProfile const& rhs)
		{
			return lhs.m_Species[0] < lhs.m_Species[0];
		});
	return output;
}

void ExportPokemonProfileData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	bool isRevisedDataset = strutil::ends_with(g_ExportFilePath, "_revised.h");
	std::string exportSuffix = isRevisedDataset ? "_Revised" : "";

	std::vector<PokemonProfile> profiles = GatherProfiles(dataPath);

	std::stringstream upperBlock;
	std::stringstream lowerBlock;

	// Gather some usage info to slap at the top
	//
	{
		PokemonProfile* mostLevelMoves = nullptr;
		PokemonProfile* mostTutorMoves = nullptr;
		std::set<std::string> competitiveFormats;

		for(auto it = profiles.begin(); it != profiles.end(); ++it)
		{
			PokemonProfile const& profile = *it;

			if (mostLevelMoves == nullptr || profile.m_LevelUpMoves.size() > mostLevelMoves->m_LevelUpMoves.size())
				mostLevelMoves = &*it;

			if (mostTutorMoves == nullptr || profile.m_TutorMoves.size() > mostTutorMoves->m_TutorMoves.size())
				mostTutorMoves = &*it;

			for(CompetitiveSet compSet : profile.m_CompetitiveSets)
			{
				for(std::string const& tier : compSet.m_SourceTiers)
					competitiveFormats.insert(tier);
			}
		}

		upperBlock << "// == INFO ==\n";
		upperBlock << "//\n";
		upperBlock << "// Highest Move Count\n";
		upperBlock << "// Level Up: " << mostLevelMoves->m_Species[0] << "(" << mostLevelMoves->m_LevelUpMoves.size() << ")\n";
		upperBlock << "// Tutor: " << mostTutorMoves->m_Species[0] << "(" << mostTutorMoves->m_TutorMoves.size() << ")\n";

		upperBlock << "//\n";
		upperBlock << "// Source Tiers:\n";
		for(std::string const& tier : competitiveFormats)
		{
			upperBlock << "// " << tier << "\n";
		}
		upperBlock << "//\n";
	}

	// Move/Item usages
	//
	std::map<std::string, int> moveCount;
	std::map<std::string, int> specialMoveCount; // i.e. moves found not in the level up moveset
	std::map<std::string, int> heldItemCount;

	for(PokemonProfile profile : profiles)
	{
		std::unordered_set<std::string> uniqueItems;
		std::unordered_set<std::string> uniqueMoves;

		for (CompetitiveSet const& compSet : profile.m_CompetitiveSets)
		{
			for(std::string const& move : compSet.m_Moves)
				uniqueMoves.insert(move);

			if (!compSet.m_Item.empty())
				uniqueItems.insert(compSet.m_Item);
		}

		for(std::string const& move : uniqueMoves)
		{
			if (moveCount.find(move) != moveCount.end())
				++moveCount[move];
			else
				moveCount[move] = 1;

			if (!profile.HasLevelUpMove(move))
			{
				if (specialMoveCount.find(move) != specialMoveCount.end())
					++specialMoveCount[move];
				else
					specialMoveCount[move] = 1;
			}
		}

		for(std::string const& item : uniqueItems)
		{
			if (heldItemCount.find(item) != heldItemCount.end())
				++heldItemCount[item];
			else
				heldItemCount[item] = 1;
		}
	}

	upperBlock << "u16 const gRoguePokemonMoveUsages" << exportSuffix << "[MOVES_COUNT] = \n{\n";

	for (auto it : moveCount)
		upperBlock << "\t[" << it.first << "] = " << it.second << ",\n";

	upperBlock << "};\n\n";

	upperBlock << "u16 const gRoguePokemonSpecialMoveUsages" << exportSuffix << "[MOVES_COUNT] = \n{\n";

	for (auto it : specialMoveCount)
		upperBlock << "\t[" << it.first << "] = " << it.second << ",\n";

	upperBlock << "};\n\n";

	upperBlock << "u16 const gRoguePokemonHeldItemUsages" << exportSuffix << "[ITEMS_COUNT] = \n{\n";

	for (auto it : heldItemCount)
		upperBlock << "\t[" << it.first << "] = " << it.second << ",\n";

	upperBlock << "};\n\n";

	// Pokemon Profiles
	//
	lowerBlock << "struct RoguePokemonProfile const gRoguePokemonProfiles" << exportSuffix << "[NUM_SPECIES] = \n{\n";

	for(PokemonProfile const& profile : profiles)
	{
		if (!profile.m_UseFallback_LevelUpMoves)
		{
			// Level moves
			upperBlock << "static struct LevelUpMove const sLevelUpMoves_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
			for (LevelUpMove const& move : profile.m_LevelUpMoves)
			{
				upperBlock << "\t{ .move=" << move.m_Move << ", .level=" << move.m_Level << " },\n";
			}
			upperBlock << "\t{ .move=MOVE_NONE, .level=0 },\n";
			upperBlock << "};\n\n";
		}

		if (!profile.m_UseFallback_TutorMoves)
		{
			// Tutor moves
			upperBlock << "static u16 const sTutorMoves_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
			for (std::string const& move : profile.m_TutorMoves)
			{
				upperBlock << "\t" << move << ",\n";
			}
			upperBlock << "\tMOVE_NONE,\n";
			upperBlock << "};\n\n";
		}

		{
			// Evolutions
			if (!profile.m_Evolutions.empty())
			{
				upperBlock << "static struct Evolution const sEvolutions_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
				for (Evolution const& evo : profile.m_Evolutions)
				{
					upperBlock << "\t{ .method=" << evo.m_Method << ", .param=" << evo.m_Param << ", .targetSpecies=" << evo.m_Species << " },\n";
				}
				upperBlock << "};\n\n";
			}
		}

		if (!profile.m_UseFallback_CompetitiveSets)
		{
			// Mon flags
			std::set<std::string> sourceTiers;

			for (CompetitiveSet const& compSet : profile.m_CompetitiveSets)
			{
				for (std::string const& tier : compSet.m_SourceTiers)
					sourceTiers.insert(FormatKeyword(tier));
			}

			upperBlock << "#ifdef APPEND_MON_FLAGS_" << profile.m_Species[0] << exportSuffix << "\n";

			upperBlock << "#define MON_FLAGS_" << profile.m_Species[0] << exportSuffix << " (APPEND_MON_FLAGS_" << profile.m_Species[0] << exportSuffix; // allow easily appending flags
			for (std::string const& tier : sourceTiers)
				upperBlock << " | MON_FLAGS_" << tier;
			upperBlock << ")\n";

			upperBlock << "#else\n";

			upperBlock << "#define MON_FLAGS_" << profile.m_Species[0] << exportSuffix << " (0";
			for (std::string const& tier : sourceTiers)
				upperBlock << " | MON_FLAGS_" << tier;
			upperBlock << ")\n";

			upperBlock << "#endif\n\n";


			// Comp sets
			if (!profile.m_CompetitiveSets.empty())
			{
				upperBlock << "static struct RoguePokemonCompetitiveSet const sCompetitiveSets_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
				for (CompetitiveSet const& compSet : profile.m_CompetitiveSets)
				{
					upperBlock << "\t{\n";

					upperBlock << "\t\t.flags= (0";
					for (std::string const& tier : compSet.m_SourceTiers)
						upperBlock << " | MON_FLAGS_" << tier;
					upperBlock << "),\n";

					if (!compSet.m_Item.empty())
						upperBlock << "\t\t.heldItem=" << compSet.m_Item << ",\n";

					if (!compSet.m_Ability.empty())
						upperBlock << "\t\t.ability=" << compSet.m_Ability << ",\n";

					if (!compSet.m_HiddenPower.empty())
						upperBlock << "\t\t.hiddenPowerType=" << compSet.m_HiddenPower << ",\n";
					else
						upperBlock << "\t\t.hiddenPowerType=TYPE_NONE,\n";

					if (!compSet.m_TeraType.empty())
						upperBlock << "\t\t.teraType=" << compSet.m_TeraType << ",\n";
					else
						upperBlock << "\t\t.teraType=TYPE_NONE,\n";

					if (!compSet.m_Nature.empty())
						upperBlock << "\t\t.nature=" << compSet.m_Nature << ",\n";

					upperBlock << "\t\t.moves=\n\t\t{\n";
					for (std::string const& move : compSet.m_Moves)
					{
						upperBlock << "\t\t\t" << move << ",\n";
					}
					upperBlock << "\t\t},\n";

					upperBlock << "\t},\n";
				}
				upperBlock << "};\n\n";
			}
		}

		// No need to duplicate, can just use non revised version
		std::string lvlUpMovesSuffix = profile.m_UseFallback_LevelUpMoves ? "" : exportSuffix;
		std::string tutorMovesSuffix = profile.m_UseFallback_TutorMoves ? "" : exportSuffix;
		std::string competitiveSetsSuffix = profile.m_UseFallback_CompetitiveSets ? "" : exportSuffix;

		// Add to species lookup below
		lowerBlock << "\t[" << profile.m_Species[0] << "] = \n\t{\n";
		lowerBlock << "\t\t.levelUpMoves = sLevelUpMoves_" << profile.m_Species[0] << lvlUpMovesSuffix << ",\n";
		lowerBlock << "\t\t.tutorMoves = sTutorMoves_" << profile.m_Species[0] << tutorMovesSuffix << ",\n";
		if (!profile.m_CompetitiveSets.empty())
		{
			lowerBlock << "\t\t.competitiveSets = sCompetitiveSets_" << profile.m_Species[0] << competitiveSetsSuffix << ",\n";
			lowerBlock << "\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_" << profile.m_Species[0] << competitiveSetsSuffix << "),\n";
		}
		else
		{
			lowerBlock << "\t\t.competitiveSets = NULL,\n";
			lowerBlock << "\t\t.competitiveSetCount = 0,\n";
		}
		if (!profile.m_Evolutions.empty())
		{
			lowerBlock << "\t\t.evolutions = sEvolutions_" << profile.m_Species[0] << exportSuffix << ",\n";
			lowerBlock << "\t\t.evolutionCount = ARRAY_COUNT(sEvolutions_" << profile.m_Species[0] << exportSuffix << "),\n";
		}
		else
		{
			lowerBlock << "\t\t.evolutions = NULL,\n";
			lowerBlock << "\t\t.evolutionCount = 0,\n";
		}
		lowerBlock << "\t\t.monFlags = MON_FLAGS_" << profile.m_Species[0] << competitiveSetsSuffix << ",\n";

		lowerBlock << "\t\t.baseStats = \n\t\t{\n";

		SpeciesProfile speciesProfile = { 0 };
		{
			auto findIt = profile.m_PerSpeciesProfile.find(profile.m_Species[0]);
			if (findIt != profile.m_PerSpeciesProfile.end())
			{
				speciesProfile = findIt->second;
			}
		}

		if (speciesProfile.m_Types.empty())
		{
			lowerBlock << "\t\t\t.types = { TYPE_NONE },\n";
		}
		else
		{
			lowerBlock << "\t\t\t.types = { ";
			for (std::string const& type : speciesProfile.m_Types)
				lowerBlock << type << ", ";
			lowerBlock << "},\n";
		}

		if (speciesProfile.m_Abilities.empty())
		{
			lowerBlock << "\t\t\t.abilities = { ABILITY_NONE },\n";
		}
		else
		{
			lowerBlock << "\t\t\t.abilities = { ";
			for (std::string const& ability : speciesProfile.m_Abilities)
				lowerBlock << ability << ", ";
			lowerBlock << "},\n";
		}

		if (speciesProfile.m_HasBaseStats)
		{
			lowerBlock << "\t\t\t.baseHP = " << speciesProfile.m_BaseStats.m_HP << ",\n";
			lowerBlock << "\t\t\t.baseAttack = " << speciesProfile.m_BaseStats.m_Attack << ",\n";
			lowerBlock << "\t\t\t.baseDefense = " << speciesProfile.m_BaseStats.m_Defense << ",\n";
			lowerBlock << "\t\t\t.baseSpeed = " << speciesProfile.m_BaseStats.m_Speed << ",\n";
			lowerBlock << "\t\t\t.baseSpAttack = " << speciesProfile.m_BaseStats.m_SpAttack << ",\n";
			lowerBlock << "\t\t\t.baseSpDefense = " << speciesProfile.m_BaseStats.m_SpDefense << ",\n";
		}

		lowerBlock << "\t\t},\n";

		lowerBlock << "\t},\n";


		// Attach redirected species info too
		for (size_t i = 1; i < profile.m_Species.size(); ++i)
		{
			speciesProfile = { 0 };
			{
				auto findIt = profile.m_PerSpeciesProfile.find(profile.m_Species[0]);
				if (findIt != profile.m_PerSpeciesProfile.end())
				{
					speciesProfile = findIt->second;
				}
			}

			lowerBlock << "\t[" << profile.m_Species[i] << "] = \n\t{\n";
			lowerBlock << "\t\t.levelUpMoves = sLevelUpMoves_" << profile.m_Species[0] << lvlUpMovesSuffix << ",\n";
			lowerBlock << "\t\t.tutorMoves = sTutorMoves_" << profile.m_Species[0] << tutorMovesSuffix << ",\n";
			if (!profile.m_CompetitiveSets.empty())
			{
				lowerBlock << "\t\t.competitiveSets = sCompetitiveSets_" << profile.m_Species[0] << competitiveSetsSuffix << ",\n";
				lowerBlock << "\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_" << profile.m_Species[0] << competitiveSetsSuffix << "),\n";
			}
			else
			{
				lowerBlock << "\t\t.competitiveSets = NULL,\n";
				lowerBlock << "\t\t.competitiveSetCount = 0,\n";
			}
			if (!profile.m_Evolutions.empty())
			{
				lowerBlock << "\t\t.evolutions = sEvolutions_" << profile.m_Species[0] << exportSuffix << ",\n";
				lowerBlock << "\t\t.evolutionCount = ARRAY_COUNT(sEvolutions_" << profile.m_Species[0] << exportSuffix << "),\n";
			}
			else
			{
				lowerBlock << "\t\t.evolutions = NULL,\n";
				lowerBlock << "\t\t.evolutionCount = 0,\n";
			}
			lowerBlock << "\t\t.monFlags = MON_FLAGS_" << profile.m_Species[0] << competitiveSetsSuffix << ",\n";

			lowerBlock << "\t\t.baseStats = \n\t\t{\n";

			if (speciesProfile.m_Types.empty())
			{
				lowerBlock << "\t\t\t.types = { TYPE_NONE },\n";
			}
			else
			{
				lowerBlock << "\t\t\t.types = { ";
				for (std::string const& type : speciesProfile.m_Types)
					lowerBlock << type << ", ";
				lowerBlock << "},\n";
			}

			if (speciesProfile.m_Abilities.empty())
			{
				lowerBlock << "\t\t\t.abilities = { ABILITY_NONE },\n";
			}
			else
			{
				lowerBlock << "\t\t\t.abilities = { ";
				for (std::string const& ability : speciesProfile.m_Abilities)
					lowerBlock << ability << ", ";
				lowerBlock << "},\n";
			}

			if (speciesProfile.m_HasBaseStats)
			{
				lowerBlock << "\t\t\t.baseHP = " << speciesProfile.m_BaseStats.m_HP << ",\n";
				lowerBlock << "\t\t\t.baseAttack = " << speciesProfile.m_BaseStats.m_Attack << ",\n";
				lowerBlock << "\t\t\t.baseDefense = " << speciesProfile.m_BaseStats.m_Defense << ",\n";
				lowerBlock << "\t\t\t.baseSpeed = " << speciesProfile.m_BaseStats.m_Speed << ",\n";
				lowerBlock << "\t\t\t.baseSpAttack = " << speciesProfile.m_BaseStats.m_SpAttack << ",\n";
				lowerBlock << "\t\t\t.baseSpDefense = " << speciesProfile.m_BaseStats.m_SpDefense << ",\n";
			}

			lowerBlock << "\t\t},\n";

			lowerBlock << "\t},\n";
		}
	}

	lowerBlock << "};\n";

	fileStream << upperBlock.str() << "\n" << lowerBlock.str();
	return;
}