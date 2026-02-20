#include "main.h"

#include <filesystem>
#include <set>
#include <sstream>
#include <unordered_set>

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

struct LevelUpMove
{
	std::string m_Move;
	int m_Level;
};

struct PokemonProfile
{
	std::vector<std::string> m_Species;
	std::vector<LevelUpMove> m_LevelUpMoves;
	std::vector<std::string> m_TutorMoves;
	std::vector<CompetitiveSet> m_CompetitiveSets;

	bool HasLevelUpMove(std::string const& move)
	{
		for (LevelUpMove const& lvlMove : m_LevelUpMoves)
		{
			if (strutil::compare_ignore_case(lvlMove.m_Move, move))
			{
				return true;
			}
		}

		return false;
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

void ParseProfile(std::string const& filePath, PokemonProfile& outProfile)
{
	json data = ReadJsonFile(filePath);

	for (json species : data["Species"])
	{
		outProfile.m_Species.push_back(species.get<std::string>());
	}

	for (json move : data["LevelUpMoves"])
	{
		outProfile.m_LevelUpMoves.push_back(
			{
				move["Move"].get<std::string>(),
				move["Level"].get<int>()
			}
		);
	}

	for (json move : data["TutorMoves"])
	{
		outProfile.m_TutorMoves.push_back(move.get<std::string>());
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
	std::string const& filePattern = parts[1];

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(dirPath))
	{
		if (dirEntry.is_regular_file() && strutil::ends_with(dirEntry.path().string(), filePattern))
		{
			PokemonProfile profile;
			ParseProfile(dirEntry.path().string(), profile);
			output.push_back(profile);
		}
	}

	std::sort(output.begin(), output.end(), [](PokemonProfile const& lhs, PokemonProfile const& rhs)
		{
			return lhs.m_Species[0] > lhs.m_Species[0];
		});
	return output;
}

void ExportPokemonProfileData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	std::string exportSuffix = strutil::ends_with(g_ExportFilePath, "_revised.h") ? "_Revised" : "";

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
		// Mon flags
		std::set<std::string> sourceTiers;

		for(CompetitiveSet const& compSet : profile.m_CompetitiveSets)
		{
			for(std::string const& tier : compSet.m_SourceTiers)
				sourceTiers.insert(FormatKeyword(tier));
		}

		upperBlock << "#ifdef APPEND_MON_FLAGS_" << profile.m_Species[0] << exportSuffix << "\n";

		upperBlock << "#define MON_FLAGS_" << profile.m_Species[0] << exportSuffix << " (APPEND_MON_FLAGS_" << profile.m_Species[0] << exportSuffix; // allow easily appending flags
		for(std::string const& tier : sourceTiers)
			upperBlock << " | MON_FLAGS_" << tier;
		upperBlock << ")\n";

		upperBlock << "#else\n";

		upperBlock << "#define MON_FLAGS_" << profile.m_Species[0] << exportSuffix << " (0";
		for (std::string const& tier : sourceTiers)
			upperBlock << " | MON_FLAGS_" << tier;
		upperBlock << ")\n";

		upperBlock << "#endif\n\n";

		// Level moves
		upperBlock << "static struct LevelUpMove const sLevelUpMoves_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
		for(LevelUpMove const& move : profile.m_LevelUpMoves)
		{
			upperBlock << "\t{ .move=" << move.m_Move << ", .level=" << move.m_Level << " },\n";
		}
		upperBlock << "\t{ .move=MOVE_NONE, .level=0 },\n";
		upperBlock << "};\n\n";

		// Tutor moves
		upperBlock << "static u16 const sTutorMoves_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
		for(std::string const& move : profile.m_TutorMoves)
		{
			upperBlock << "\t" << move << ",\n";
		}
		upperBlock << "\tMOVE_NONE,\n";
		upperBlock << "};\n\n";

		// Comp sets
		upperBlock << "static struct RoguePokemonCompetitiveSet const sCompetitiveSets_" << profile.m_Species[0] << exportSuffix << "[] = \n{\n";
		for(CompetitiveSet const& compSet : profile.m_CompetitiveSets)
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
			for(std::string const& move : compSet.m_Moves)
			{
				upperBlock << "\t\t\t" << move << ",\n";
			}
			upperBlock << "\t\t},\n";

			upperBlock << "\t},\n";
		}
		upperBlock << "};\n\n";


		// Add to species lookup below
		lowerBlock << "\t[" << profile.m_Species[0] << "] = \n\t{\n";
		lowerBlock << "\t\t.levelUpMoves = sLevelUpMoves_" << profile.m_Species[0] << exportSuffix << ",\n";
		lowerBlock << "\t\t.tutorMoves = sTutorMoves_" << profile.m_Species[0] << exportSuffix << ",\n";
		lowerBlock << "\t\t.competitiveSets = sCompetitiveSets_" << profile.m_Species[0] << exportSuffix << ",\n";
		lowerBlock << "\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_" << profile.m_Species[0] << exportSuffix << "),\n";
		lowerBlock << "\t\t.monFlags = MON_FLAGS_" << profile.m_Species[0] << exportSuffix << ",\n";
		lowerBlock << "\t},\n";


		// Attach redirected species info too
		for (size_t i = 1; i < profile.m_Species.size(); ++i)
		{
			lowerBlock << "\t[" << profile.m_Species[i] << "] = \n\t{\n";
			lowerBlock << "\t\t.levelUpMoves = sLevelUpMoves_" << profile.m_Species[0] << exportSuffix << ",\n";
			lowerBlock << "\t\t.tutorMoves = sTutorMoves_" << profile.m_Species[0] << exportSuffix << ",\n";
			lowerBlock << "\t\t.competitiveSets = sCompetitiveSets_" << profile.m_Species[0] << exportSuffix << ",\n";
			lowerBlock << "\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_" << profile.m_Species[0] << exportSuffix << ",\n";
			lowerBlock << "\t\t.monFlags = MON_FLAGS_" << profile.m_Species[0] << exportSuffix << ",\n";
			lowerBlock << "\t},\n";
		}
	}

	lowerBlock << "};\n";

	fileStream << upperBlock.str() << "\n" << lowerBlock.str();
	return;
}