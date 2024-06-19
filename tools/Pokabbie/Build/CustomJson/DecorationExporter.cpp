#include "main.h"

#include <functional>
#include <sstream>
#include <vector>

enum class DecorationType
{
	Tile,
	ObjectEvent
};

struct DecorationVariant
{
	DecorationType type;
	std::string name;
	std::string sourceMap;
	std::string layer = "DECOR_LAYER_DEFAULT";
	struct
	{
		int x;
		int y;
		int width = 1;
		int height = 1;
	} tileParams;
	struct
	{
		int localId;
		int editorLocalId = -1;
		int capacityPerArea;
	} objectEventParams;
};

struct Decoration
{
	std::string preprocessorCondition;
	std::string uniqueId;
	std::string name;
	std::string displayGroup;
	std::vector<DecorationVariant> variants;
};

struct DecorationData
{
	std::vector<Decoration> decorations;
	std::vector<std::string> groups;
	std::map<std::string, std::vector<Decoration*>> decorationsByGroup;
	std::vector<std::string> uniqueStrings;
	std::unordered_map<std::string, int> uniqueStringLookup;
};

static void GatherDecorations(std::string const& dataPath, json const& jsonData, DecorationData& outDecorData);
static std::string FormatUniqueId(std::string const& prettyName);

void ExportDecorationData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	DecorationData decorData;
	GatherDecorations(dataPath, jsonData, decorData);

	// Group info
	{
		// Required data
		for (auto it = decorData.decorationsByGroup.begin(); it != decorData.decorationsByGroup.end(); ++it)
		{
			std::string groupName = it->first;
			std::vector<Decoration*> sortedDecorations = it->second;

			std::sort(sortedDecorations.begin(), sortedDecorations.end(), [](Decoration const* a, Decoration const* b)
				{
					return a->uniqueId.compare(b->uniqueId) == -1;
				});

			std::string prettyName = strutil::split(groupName, "#")[0];

			fileStream << "static u8 const sText_GroupName_" << FormatUniqueId(groupName) << "[] = _(\"" + prettyName + "\");\n";
			fileStream << "static u16 const sText_Group_" << FormatUniqueId(groupName) << "[] =\n{\n";

			for (Decoration* decor : sortedDecorations)
			{
				if (!decor->preprocessorCondition.empty())
					fileStream << "#if " << decor->preprocessorCondition << "\n";

				fileStream << c_TabSpacing << "DECOR_ID_" << decor->uniqueId << ",\n";

				if (!decor->preprocessorCondition.empty())
					fileStream << "#endif\n";
			}

			fileStream << "};\n";
			fileStream << "\n";
		}

		// Useable data
		fileStream << "static struct RogueDecorationGroup const sDecorationGroups[DECOR_GROUP_COUNT] =\n{\n";

		for (auto it = decorData.decorationsByGroup.begin(); it != decorData.decorationsByGroup.end(); ++it)
		{
			std::string groupName = it->first;

			fileStream << c_TabSpacing << "[DECOR_GROUP_" + FormatUniqueId(groupName) + "] =\n";
			fileStream << c_TabSpacing << "{\n";
			fileStream << c_TabSpacing2 << ".name = sText_GroupName_" << FormatUniqueId(groupName) << ",\n";
			fileStream << c_TabSpacing2 << ".decorationIds = sText_Group_" << FormatUniqueId(groupName) << ",\n";
			fileStream << c_TabSpacing2 << ".decorationCount = ARRAY_COUNT(sText_Group_" << FormatUniqueId(groupName) << "),\n";
			fileStream << c_TabSpacing << "},\n";
		}

		fileStream << "};\n";
	}
	fileStream << "\n";

	// Strings
	for (int i = 0; i < (int)decorData.uniqueStrings.size(); ++i)
	{
		fileStream << "static u8 const sText_Str_" << i << "[] = _(\"" << decorData.uniqueStrings[i] << "\");\n";
	}
	fileStream << "\n";

	// Decoration Data
	{
		// Variant Data
		fileStream << "static struct RogueDecorationVariant const sDecorationVariants[DECOR_VARIANT_COUNT] =\n{\n";

		for (auto const& decor : decorData.decorations)
		{
			if (!decor.preprocessorCondition.empty())
				fileStream << "#if " << decor.preprocessorCondition << "\n";

			for (auto const& variant : decor.variants)
			{
				fileStream << c_TabSpacing << "[DECOR_VARIANT_" << decor.uniqueId << "_" << FormatUniqueId(variant.name) << "] =\n";
				fileStream << c_TabSpacing << "{\n";
				fileStream << c_TabSpacing2 << ".name = sText_Str_" << decorData.uniqueStringLookup[variant.name] << ",\n";
				fileStream << c_TabSpacing2 << ".srcMapGroup = MAP_GROUP(" << variant.sourceMap << "),\n";
				fileStream << c_TabSpacing2 << ".srcMapNum = MAP_NUM(" << variant.sourceMap << "),\n";
				fileStream << c_TabSpacing2 << ".layer = " << variant.layer << ",\n";

				switch (variant.type)
				{
				case DecorationType::Tile:
					fileStream << c_TabSpacing2 << ".type = DECOR_TYPE_TILE,\n";
					fileStream << c_TabSpacing2 << ".perType = { .tile =\n";
					fileStream << c_TabSpacing2 << "{\n";
					fileStream << c_TabSpacing3 << ".x = " << variant.tileParams.x << ",\n";
					fileStream << c_TabSpacing3 << ".y = " << variant.tileParams.y << ",\n";
					fileStream << c_TabSpacing3 << ".width = " << variant.tileParams.width << ",\n";
					fileStream << c_TabSpacing3 << ".height = " << variant.tileParams.height << ",\n";
					fileStream << c_TabSpacing2 << "} }\n";
					break;

				case DecorationType::ObjectEvent:
					fileStream << c_TabSpacing2 << ".type = DECOR_TYPE_OBJECT_EVENT,\n";
					fileStream << c_TabSpacing2 << ".perType = { .objectEvent =\n";
					fileStream << c_TabSpacing2 << "{\n";
					fileStream << c_TabSpacing3 << ".localId = " << variant.objectEventParams.localId << ",\n";
					fileStream << c_TabSpacing3 << ".editorLocalId = " << (variant.objectEventParams.editorLocalId > 0 ? variant.objectEventParams.editorLocalId : variant.objectEventParams.localId) << ",\n";
					fileStream << c_TabSpacing3 << ".capacityPerArea = " << variant.objectEventParams.capacityPerArea << ",\n";
					fileStream << c_TabSpacing2 << "} }\n";
					break;
				}


				fileStream << c_TabSpacing << "},\n";
			}

			if (!decor.preprocessorCondition.empty())
				fileStream << "#endif\n";
		}

		fileStream << "};\n\n";

		// Main decorations
		fileStream << "static struct RogueDecoration const sDecorations[DECOR_ID_COUNT] =\n{\n";

		for (auto const& decor : decorData.decorations)
		{
			if (!decor.preprocessorCondition.empty())
				fileStream << "#if " << decor.preprocessorCondition << "\n";

			fileStream << c_TabSpacing << "[DECOR_ID_" << decor.uniqueId << "] =\n";
			fileStream << c_TabSpacing << "{\n";
			fileStream << c_TabSpacing2 << ".name = sText_Str_" << decorData.uniqueStringLookup[decor.name] << ",\n";

			DecorationVariant const& firstVariant = decor.variants.front();
			DecorationVariant const& lastVariant = decor.variants.back();

			fileStream << c_TabSpacing2 << ".firstVariantId = DECOR_VARIANT_" << decor.uniqueId << "_" << FormatUniqueId(firstVariant.name) << ",\n";
			fileStream << c_TabSpacing2 << ".lastVariantId = DECOR_VARIANT_" << decor.uniqueId << "_" << FormatUniqueId(lastVariant.name) << ",\n";

			fileStream << c_TabSpacing << "},\n";

			if (!decor.preprocessorCondition.empty())
				fileStream << "#endif\n";
		}

		fileStream << "};\n\n";
	}
}

void ExportDecorationData_H(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	DecorationData decorData;
	GatherDecorations(dataPath, jsonData, decorData);

	// Grooup Enum define
	fileStream << "enum\n{\n";
	for (auto it = decorData.groups.begin(); it != decorData.groups.end(); ++it)
	{
		fileStream << c_TabSpacing << "DECOR_GROUP_" << FormatUniqueId(*it) << ",\n";
	}
	fileStream << c_TabSpacing << "DECOR_GROUP_COUNT,\n";
	fileStream << "};\n\n";

	// Decoration Enum define
	fileStream << "enum\n{\n";
	for (auto it = decorData.decorations.begin(); it != decorData.decorations.end(); ++it)
	{
		auto const& decor = *it;

		if (!decor.preprocessorCondition.empty())
			fileStream << "#if " << decor.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "DECOR_ID_" << decor.uniqueId << ",\n";

		if (!decor.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << c_TabSpacing << "DECOR_ID_COUNT,\n";
	fileStream << "};\n\n";

	// Variant Enum define
	fileStream << "enum\n{\n";
	for (auto it = decorData.decorations.begin(); it != decorData.decorations.end(); ++it)
	{
		auto const& decor = *it;

		if (!decor.preprocessorCondition.empty())
			fileStream << "#if " << decor.preprocessorCondition << "\n";

		for (auto const& variant : decor.variants)
		{
			fileStream << c_TabSpacing << "DECOR_VARIANT_" << decor.uniqueId << "_" << FormatUniqueId(variant.name) << ",\n";
		}

		if (!decor.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << c_TabSpacing << "DECOR_VARIANT_COUNT,\n";
	fileStream << "};\n\n";
}

//static std::string GetAsString(json const& jsonValue)
//{
//	if (jsonValue.is_boolean())
//		return jsonValue.get<bool>() ? "TRUE" : "FALSE";
//
//	if (jsonValue.is_number_integer())
//		return std::to_string(jsonValue.get<int>());
//
//	if (jsonValue.is_number_unsigned())
//		return std::to_string(jsonValue.get<unsigned int>());
//
//	if (jsonValue.is_number_float())
//		return std::to_string(jsonValue.get<float>());
//
//	return jsonValue.get<std::string>();
//}

static DecorationVariant ParseDecorationVariant(json const& jsonData, DecorationVariant const& defaultValues)
{
	DecorationVariant outVariant = defaultValues;

	outVariant.name = jsonData["name"].get<std::string>();

	if(jsonData.contains("source_map"))
		outVariant.sourceMap = jsonData["source_map"].get<std::string>();

	if (jsonData.contains("layer"))
		outVariant.layer = jsonData["layer"].get<std::string>();

	if (jsonData.contains("source_tiles"))
	{
		json sourceTilesJson = jsonData["source_tiles"];

		outVariant.type = DecorationType::Tile;

		if (sourceTilesJson.contains("x"))
			outVariant.tileParams.x = sourceTilesJson["x"].get<int>();

		if (sourceTilesJson.contains("y"))
			outVariant.tileParams.y = sourceTilesJson["y"].get<int>();

		if (sourceTilesJson.contains("width"))
			outVariant.tileParams.width = sourceTilesJson["width"].get<int>();

		if (sourceTilesJson.contains("height"))
			outVariant.tileParams.height = sourceTilesJson["height"].get<int>();

		return outVariant;
	}

	if (jsonData.contains("source_object"))
	{
		json sourceObjectJson = jsonData["source_object"];

		outVariant.type = DecorationType::ObjectEvent;

		outVariant.objectEventParams.localId = sourceObjectJson["local_id"].get<int>();

		if (sourceObjectJson.contains("editor_local_id"))
			outVariant.objectEventParams.editorLocalId = sourceObjectJson["editor_local_id"].get<int>();

		if (sourceObjectJson.contains("capacity_per_area"))
			outVariant.objectEventParams.capacityPerArea = sourceObjectJson["capacity_per_area"].get<int>();

		return outVariant;
	}


	FATAL_ERROR("Unrecognised variant object:\n'%s'", jsonData.dump().c_str());
	return outVariant;
}

static Decoration ParseDecoration(std::string const& idPrefix, json const& jsonData)
{
	Decoration outDecor;
	DecorationVariant defaultVariant = ParseDecorationVariant(jsonData, DecorationVariant());
	defaultVariant.name = "Unnamed";

	outDecor.name = jsonData["name"].get<std::string>();
	outDecor.displayGroup = jsonData["display_group"].get<std::string>();

	if (jsonData.contains("#if"))
	{
		outDecor.preprocessorCondition = jsonData["#if"].get<std::string>();
	}

	if (jsonData.contains("variants"))
	{
		json variantsGroup = jsonData["variants"];

		for (auto groupIt = variantsGroup.begin(); groupIt != variantsGroup.end(); ++groupIt)
		{
			json variantGroup = groupIt.value();
			DecorationVariant variant = ParseDecorationVariant(variantGroup, defaultVariant);
			outDecor.variants.push_back(variant);
		}
	}
	else
	{
		defaultVariant.name = "Default";
		outDecor.variants.push_back(defaultVariant);
	}

	return outDecor;
}

static void ProcessUniqueString(DecorationData& outDecorData, std::string const& str)
{
	if (outDecorData.uniqueStringLookup.find(str) == outDecorData.uniqueStringLookup.end())
	{
		outDecorData.uniqueStringLookup[str] = (int)outDecorData.uniqueStrings.size();
		outDecorData.uniqueStrings.push_back(str);
	}
}

static std::string FormatUniqueId(std::string const& prettyName)
{
	std::string uniqueId = strutil::to_upper(prettyName);
	strutil::replace_all(uniqueId, " ", "_");
	strutil::replace_all(uniqueId, "-", "_");
	strutil::replace_all(uniqueId, "é", "E"); // code for �
	strutil::replace_all(uniqueId, "!", "EMARK");
	strutil::replace_all(uniqueId, "?", "QMARK");
	strutil::replace_all(uniqueId, ",", "");
	strutil::replace_all(uniqueId, ".", "");
	strutil::replace_all(uniqueId, "\"", "");
	strutil::replace_all(uniqueId, "\\", "");
	strutil::replace_all(uniqueId, "/", "");
	strutil::replace_all(uniqueId, "[", "");
	strutil::replace_all(uniqueId, "]", "");
	strutil::replace_all(uniqueId, "{", "");
	strutil::replace_all(uniqueId, "}", "");
	strutil::replace_all(uniqueId, "(", "");
	strutil::replace_all(uniqueId, ")", "");
	strutil::replace_all(uniqueId, "'", "");
	strutil::replace_all(uniqueId, "+", "PLUS");
	strutil::replace_all(uniqueId, "#", "_");
	strutil::replace_all(uniqueId, c_Elipsies, "");
	return uniqueId;
}

static void GatherDecorations(std::string const& dataPath, json const& rawJsonData, DecorationData& outDecorData)
{
	json jsonData = ExpandCommonArrayGroup(dataPath, rawJsonData, "decorations");
	json decorationGroups = jsonData["decorations"];

	std::unordered_map<std::string, int> nameCount;

	for (auto groupIt = decorationGroups.begin(); groupIt != decorationGroups.end(); ++groupIt)
	{
		std::string prefix = groupIt.key();
		json decorGroup = groupIt.value();

		for (auto decorIt : decorGroup)
		{
			Decoration decor = ParseDecoration(prefix, decorIt);
			decor.uniqueId = FormatUniqueId(decor.name);

			if (nameCount.find(decor.uniqueId) == nameCount.end())
			{
				nameCount[decor.name] = 1;
			}
			else
			{
				int suffix = nameCount[decor.uniqueId]++;
				decor.uniqueId += "_" + std::to_string(suffix);
			}

			outDecorData.decorations.push_back(std::move(decor));
		}
	}

	// Register all the unique strings
	for (auto const& decor : outDecorData.decorations)
	{
		ProcessUniqueString(outDecorData, decor.name);

		for (auto const& variant : decor.variants)
		{
			ProcessUniqueString(outDecorData, variant.name);
		}
	}

	// Populate decorations by group
	for (auto& decor : outDecorData.decorations)
	{
		if (outDecorData.decorationsByGroup[decor.displayGroup].empty())
			outDecorData.groups.push_back(decor.displayGroup);

		outDecorData.decorationsByGroup[decor.displayGroup].push_back(&decor);
	}

	return;
}