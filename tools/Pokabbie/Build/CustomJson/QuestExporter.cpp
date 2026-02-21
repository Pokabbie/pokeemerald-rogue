#include "main.h"

#include <functional>
#include <sstream>
#include <vector>

enum class QuestRewardType
{
	Pokemon,
	Item,
	ShopItem,
	Money,
	QuestUnlock,
	Flag,
	HubUpgrade,
	Decor,
	DecorVariant,
	OutfitUnlock
};

struct QuestReward
{
	QuestRewardType type;
	std::string preprocessorCondition;
	std::string visibility;
	std::string requiredDifficulty;
	struct
	{
		bool isValid;
		std::string itemIcon;
		std::string speciesIcon;
		std::string title;
		std::string subtitle;
		std::string soundEffect;
		std::string fanfare;
	} customPopup;
	struct
	{
		std::string species;
		std::string isShiny;
		std::string customMonId;
		std::string nickname;
	} 
	pokemonParams;
	struct
	{
		std::string item;
		std::string count;
	} 
	itemParams;
	struct
	{
		std::string item;
	} 
	shopItemParams;
	struct
	{
		std::string amount;
	} 
	moneyParams;
	struct
	{
		std::string questId;
	}
	questUnlockParams;
	struct
	{
		std::string flag;
	} flagParams;
	struct
	{
		std::string upgradeId;
	} hubUpgrade;
	struct
	{
		std::string decorId;
	} decor;
	struct
	{
		std::string decorVariantId;
	} decorVariant;
	struct
	{
		std::string outfitUnlockId;
	} outfitUnlock;
};

enum class QuestRequirementType
{
	Item,
	Flag,
	ConfigToggle,
	ConfigRange,
};

struct QuestRequirement
{
	QuestRequirementType type;
	std::string preprocessorCondition;
	struct
	{
		std::string item;
		std::string operation;
		std::string count;
	}
	itemParams;
	struct
	{
		std::string flag;
		std::string state;
	}
	flagParams;
	struct
	{
		std::string configToggle;
		std::string state;
	}
	configToggleParams;
	struct
	{
		std::string configRange;
		std::string operation;
		std::string value;
	}
	configRangeParams;
};

struct QuestTrigger
{
	std::string callback;
	std::string passState;
	std::string failState;
	std::vector<std::string> flags;
	std::vector<std::string> params;
};

struct QuestInfo
{
	json groupObj;
	json questObj;
	int importIndex;
	std::string questId;
	std::string preprocessorCondition;
	std::string displayGroup;
	bool isUnlockedViaReward;
	std::vector<std::string> flags;
	std::vector<QuestTrigger> triggers;
	std::vector<std::string> collatedTriggerFlags;
	std::vector<QuestReward> rewards;
	std::vector<QuestRequirement> requirements;

	inline std::string GetUniqueWriteId() const
	{
		return questId + "_" + std::to_string(importIndex);
	}
};

struct DisplayGroup
{
	int m_GroupIndex;
	bool m_SortAlphabetically;
};

struct QuestData
{
	std::vector<QuestInfo> questInfo;
	std::map<std::string, DisplayGroup> displayGroups;
};

static void GatherQuests(std::string const& dataPath, json const& jsonData, QuestData& outQuestData);

static std::string QuestExpandString(QuestInfo const& quest, std::string target)
{
	// Allow templating desc params
	if (quest.questObj.contains("template_params"))
	{
		json templateParams = quest.questObj["template_params"];

		for (auto it = templateParams.begin(); it != templateParams.end(); it++)
		{
			std::string replaceKey = "%" + it.key() + "%";
			std::string replaceValue = it.value().get<std::string>();

			// Pretty print hacks
			if (replaceKey == "%MON_SPECIES%")
			{
				if (strutil::starts_with(replaceValue, "SPECIES_"))
				{
					replaceValue = replaceValue.substr(std::string("SPECIES_").length());
					replaceValue = strutil::capitalize(strutil::to_lower(replaceValue));
				}
			}

			strutil::replace_all(target, replaceKey, replaceValue);
		}
	}

	return target;
}

static std::string GetQuestName(QuestInfo const& quest)
{
	return QuestExpandString(quest, quest.questObj["name"].get<std::string>());
}

static std::string GetQuestDescription(QuestInfo const& quest)
{
	return QuestExpandString(quest, quest.questObj["description"].get<std::string>());
}

static std::string FlagsToString(std::string const& prefix, std::vector<std::string> const flags)
{
	std::string output = "0";

	for (auto const& flag : flags)
		output += " | " + prefix + flag;

	return output;
}

void ExportQuestData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	QuestData questData;
	GatherQuests(dataPath, jsonData, questData);

	// Populate lookup
	std::unordered_map<std::string, QuestInfo*> questLookup;

	for (auto& quest : questData.questInfo)
		questLookup[quest.questId] = &quest;

	// Required data
	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << "static u8 const sTitle_" << quest.GetUniqueWriteId() << "[] = _(\"" << GetQuestName(quest) << "\");\n";
		fileStream << "extern const u8 gQuestDescText_" << quest.GetUniqueWriteId() << "[];\n";
		fileStream << "\n";

		// Rewards requirements
		int rewardCounter = 0;
		for (auto const& rewardInfo : quest.rewards)
		{
			int rewardIndex = rewardCounter++;

			if (rewardInfo.customPopup.isValid)
			{
				if (!rewardInfo.preprocessorCondition.empty())
					fileStream << "#if " << rewardInfo.preprocessorCondition << "\n";

				fileStream << "static u8 const sCustomPopupTitle_" << quest.GetUniqueWriteId() << "_" << rewardIndex << "[] = _(\"" << rewardInfo.customPopup.title << "\");\n";
				fileStream << "static u8 const sCustomPopupSubtitle_" << quest.GetUniqueWriteId() << "_" << rewardIndex << "[] = _(\"" << rewardInfo.customPopup.subtitle << "\");\n";
				fileStream << "\n";

				fileStream << "static struct CustomPopup const sCustomPopup_" << quest.GetUniqueWriteId() << "_" << rewardIndex << " = \n";
				fileStream << "{\n";
				fileStream << c_TabSpacing << ".titleStr = sCustomPopupTitle_" << quest.GetUniqueWriteId() << "_" << rewardIndex << ",\n";
				fileStream << c_TabSpacing << ".subtitleStr = sCustomPopupSubtitle_" << quest.GetUniqueWriteId() << "_" << rewardIndex << ",\n";

				if(!rewardInfo.customPopup.itemIcon.empty())
					fileStream << c_TabSpacing << ".itemIcon = " << rewardInfo.customPopup.itemIcon << ",\n";

				if (!rewardInfo.customPopup.speciesIcon.empty())
					fileStream << c_TabSpacing << ".speciesIcon = " << rewardInfo.customPopup.speciesIcon << ",\n";

				fileStream << c_TabSpacing << ".soundEffect = " << rewardInfo.customPopup.soundEffect << ",\n";
				fileStream << c_TabSpacing << ".fanfare = " << rewardInfo.customPopup.fanfare << ",\n";
				fileStream << "};\n";

				if (!rewardInfo.preprocessorCondition.empty())
					fileStream << "#endif\n";

				fileStream << "\n";
			}
		}

		// Rewards
		fileStream << "static struct RogueQuestReward const sRewards_" << quest.GetUniqueWriteId() << "[] = \n";
		fileStream << "{\n";
		rewardCounter = 0;

		for (auto const& rewardInfo : quest.rewards)
		{
			int rewardIndex = rewardCounter++;

			if (!rewardInfo.preprocessorCondition.empty())
				fileStream << "#if " << rewardInfo.preprocessorCondition << "\n";

			fileStream << c_TabSpacing << "{\n";
			fileStream << c_TabSpacing2 << ".visiblity = QUEST_REWARD_VISIBLITY_" << rewardInfo.visibility << ",\n";
			fileStream << c_TabSpacing2 << ".requiredDifficulty = DIFFICULTY_LEVEL_" << rewardInfo.requiredDifficulty << ",\n";

			if (rewardInfo.customPopup.isValid)
				fileStream << c_TabSpacing2 << ".customPopup = &sCustomPopup_" << quest.GetUniqueWriteId() << "_" << rewardIndex << ",\n";
			else
				fileStream << c_TabSpacing2 << ".customPopup = NULL,\n";


			switch (rewardInfo.type)
			{
			case QuestRewardType::Pokemon:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_POKEMON,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".pokemon = {\n";
				fileStream << c_TabSpacing4 << ".species = " << rewardInfo.pokemonParams.species << ",\n";
				fileStream << c_TabSpacing4 << ".isShiny = " << rewardInfo.pokemonParams.isShiny << ",\n";
				fileStream << c_TabSpacing4 << ".customMonId = " << rewardInfo.pokemonParams.customMonId << ",\n";

				if(rewardInfo.pokemonParams.nickname.empty())
					fileStream << c_TabSpacing4 << ".nickname = NULL,\n";
				else
				{
					std::string upperNickname = strutil::to_upper(rewardInfo.pokemonParams.nickname);

					fileStream << "#ifdef ROGUE_EXPANSION\n";
					fileStream << c_TabSpacing4 << ".nickname = COMPOUND_STRING(\"" << rewardInfo.pokemonParams.nickname << "\"),\n";
					fileStream << "#else\n";
					fileStream << c_TabSpacing4 << ".nickname = COMPOUND_STRING(\"" << upperNickname << "\"),\n";
					fileStream << "#endif\n";
				}

				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::Item:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_ITEM,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".item = {\n";
				fileStream << c_TabSpacing4 << ".item = " << rewardInfo.itemParams.item << ",\n";
				fileStream << c_TabSpacing4 << ".count = " << rewardInfo.itemParams.count << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::ShopItem:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_SHOP_ITEM,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".shopItem = {\n";
				fileStream << c_TabSpacing4 << ".item = " << rewardInfo.shopItemParams.item << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::Money:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_MONEY,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".money = {\n";
				fileStream << c_TabSpacing4 << ".amount = " << rewardInfo.moneyParams.amount << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::QuestUnlock:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_QUEST_UNLOCK,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".questUnlock = {\n";
				fileStream << c_TabSpacing4 << ".questId = QUEST_ID_" << rewardInfo.questUnlockParams.questId << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::Flag:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_FLAG,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".flag = {\n";
				fileStream << c_TabSpacing4 << ".flagId = " << rewardInfo.flagParams.flag << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::HubUpgrade:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_HUB_UPGRADE,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".hubUpgrade = {\n";
				fileStream << c_TabSpacing4 << ".upgradeId = " << rewardInfo.hubUpgrade.upgradeId << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;


			case QuestRewardType::Decor:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_DECOR,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".decor = {\n";
				fileStream << c_TabSpacing4 << ".decorId = " << rewardInfo.decor.decorId << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::DecorVariant:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_DECOR_VARIANT,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".decorVariant = {\n";
				fileStream << c_TabSpacing4 << ".decorVariantId = " << rewardInfo.decorVariant.decorVariantId << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::OutfitUnlock:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_OUTFIT_UNLOCK,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".outfitUnlock = {\n";
				fileStream << c_TabSpacing4 << ".outfitUnlockId = " << rewardInfo.outfitUnlock.outfitUnlockId << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			default:
				FATAL_ERROR("Unsupported reward type");
				break;
			}

			fileStream << c_TabSpacing << "},\n";

			if (!rewardInfo.preprocessorCondition.empty())
				fileStream << "#endif\n";

		}

		fileStream << "};\n";

		// Requirements
		//
		if (!quest.requirements.empty())
		{
			fileStream << "static struct RogueQuestRequirement const sRequirements_" << quest.GetUniqueWriteId() << "[] = \n";
			fileStream << "{\n";

			for (auto const& requirementInfo : quest.requirements)
			{
				if (!requirementInfo.preprocessorCondition.empty())
					fileStream << "#if " << requirementInfo.preprocessorCondition << "\n";

				fileStream << c_TabSpacing << "{\n";

				switch (requirementInfo.type)
				{
				case QuestRequirementType::Item:
					fileStream << c_TabSpacing2 << ".type = QUEST_REQUIREMENT_TYPE_ITEM,\n";
					fileStream << c_TabSpacing2 << ".perType = {\n";
					fileStream << c_TabSpacing3 << ".item = {\n";
					fileStream << c_TabSpacing4 << ".itemId = " << requirementInfo.itemParams.item << ",\n";
					fileStream << c_TabSpacing4 << ".operation = QUEST_REQUIREMENT_OPERATION_" << requirementInfo.itemParams.operation << ",\n";
					fileStream << c_TabSpacing4 << ".count = " << requirementInfo.itemParams.count << ",\n";
					fileStream << c_TabSpacing3 << "}\n";
					fileStream << c_TabSpacing2 << "}\n";
					break;

				case QuestRequirementType::Flag:
					fileStream << c_TabSpacing2 << ".type = QUEST_REQUIREMENT_TYPE_FLAG,\n";
					fileStream << c_TabSpacing2 << ".perType = {\n";
					fileStream << c_TabSpacing3 << ".flag = {\n";
					fileStream << c_TabSpacing4 << ".flag = " << requirementInfo.flagParams.flag << ",\n";
					fileStream << c_TabSpacing4 << ".state = " << requirementInfo.flagParams.state << ",\n";
					fileStream << c_TabSpacing3 << "}\n";
					fileStream << c_TabSpacing2 << "}\n";
					break;

				case QuestRequirementType::ConfigToggle:
					fileStream << c_TabSpacing2 << ".type = QUEST_REQUIREMENT_TYPE_CONFIG_TOGGLE,\n";
					fileStream << c_TabSpacing2 << ".perType = {\n";
					fileStream << c_TabSpacing3 << ".configToggle = {\n";
					fileStream << c_TabSpacing4 << ".toggle = " << requirementInfo.configToggleParams.configToggle << ",\n";
					fileStream << c_TabSpacing4 << ".state = " << requirementInfo.configToggleParams.state << ",\n";
					fileStream << c_TabSpacing3 << "}\n";
					fileStream << c_TabSpacing2 << "}\n";
					break;

				case QuestRequirementType::ConfigRange:
					fileStream << c_TabSpacing2 << ".type = QUEST_REQUIREMENT_TYPE_CONFIG_RANGE,\n";
					fileStream << c_TabSpacing2 << ".perType = {\n";
					fileStream << c_TabSpacing3 << ".configRange = {\n";
					fileStream << c_TabSpacing4 << ".range = " << requirementInfo.configRangeParams.configRange << ",\n";
					fileStream << c_TabSpacing4 << ".operation = QUEST_REQUIREMENT_OPERATION_" << requirementInfo.configRangeParams.operation << ",\n";
					fileStream << c_TabSpacing4 << ".value = " << requirementInfo.configRangeParams.value << ",\n";
					fileStream << c_TabSpacing3 << "}\n";
					fileStream << c_TabSpacing2 << "}\n";
					break;

				default:
					FATAL_ERROR("Unsupported reward type");
					break;
				}

				fileStream << c_TabSpacing << "},\n";

				if (!requirementInfo.preprocessorCondition.empty())
					fileStream << "#endif\n";

			}

			fileStream << "};\n";
		}


		// Triggers
		// 
		// Params
		for (size_t i = 0; i < quest.triggers.size(); ++i)
		{
			auto const& trigger = quest.triggers[i];

			fileStream << "static u16 const sTriggerParams_" << quest.GetUniqueWriteId() << "_" << i << "[] = \n";
			fileStream << "{\n";

			for (auto const& triggerParam : trigger.params)
			{
				if(strutil::starts_with(triggerParam, "#"))
					fileStream << c_TabSpacing << triggerParam << "\n";
				else
					fileStream << c_TabSpacing << triggerParam << ",\n";
			}

			fileStream << "};\n";
		}

		// Trigger array
		fileStream << "static struct RogueQuestTrigger const sTriggers_" << quest.GetUniqueWriteId() << "[] = \n";
		fileStream << "{\n";
		for (size_t i = 0; i < quest.triggers.size(); ++i)
		{
			auto const& trigger = quest.triggers[i];

			fileStream << c_TabSpacing << "{\n";

			fileStream << c_TabSpacing2 << ".callback = QuestCondition_" << trigger.callback << ",\n";
			fileStream << c_TabSpacing2 << ".passState = QUEST_STATUS_" << trigger.passState << ",\n";
			fileStream << c_TabSpacing2 << ".failState = QUEST_STATUS_" << trigger.failState << ",\n";
			fileStream << c_TabSpacing2 << ".flags = " << FlagsToString("QUEST_TRIGGER_", trigger.flags) << ",\n";
			fileStream << c_TabSpacing2 << ".params = sTriggerParams_" << quest.GetUniqueWriteId() << "_" << i << ",\n";
			fileStream << c_TabSpacing2 << ".paramCount = ARRAY_COUNT(sTriggerParams_" << quest.GetUniqueWriteId() << "_" << i << "),\n";

			fileStream << c_TabSpacing << "},\n";
		}
		fileStream << "};\n";

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";

		fileStream << "\n";
	}

	// Quest array
	fileStream << "static struct RogueQuestEntry const sQuestEntries[] =\n{\n";
	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "[QUEST_ID_" << quest.questId << "] = \n";
		fileStream << c_TabSpacing << "{\n";


		fileStream << c_TabSpacing2 << ".title = sTitle_" << quest.GetUniqueWriteId() << ",\n";
		fileStream << c_TabSpacing2 << ".desc = gQuestDescText_" << quest.GetUniqueWriteId() << ",\n";
		fileStream << c_TabSpacing2 << ".flags = " << FlagsToString("QUEST_CONST_", quest.flags) << ",\n";

		fileStream << c_TabSpacing2 << ".triggers = sTriggers_" << quest.GetUniqueWriteId() << ",\n";
		fileStream << c_TabSpacing2 << ".triggerCount = ARRAY_COUNT(sTriggers_" << quest.GetUniqueWriteId() << "),\n";
		fileStream << c_TabSpacing2 << ".triggerFlags = " << FlagsToString("QUEST_TRIGGER_", quest.collatedTriggerFlags) << ",\n";

		fileStream << c_TabSpacing2 << ".rewards = sRewards_" << quest.GetUniqueWriteId() << ",\n";
		fileStream << c_TabSpacing2 << ".rewardCount = ARRAY_COUNT(sRewards_" << quest.GetUniqueWriteId() << "),\n";

		if (!quest.requirements.empty())
		{
			fileStream << c_TabSpacing2 << ".requirements = sRequirements_" << quest.GetUniqueWriteId() << ",\n";
			fileStream << c_TabSpacing2 << ".requirementCount = ARRAY_COUNT(sRequirements_" << quest.GetUniqueWriteId() << "),\n";
		}
		else
		{
			fileStream << c_TabSpacing2 << ".requirements = NULL,\n";
			fileStream << c_TabSpacing2 << ".requirementCount = 0,\n";
		}

		fileStream << c_TabSpacing << "},\n";

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "};\n\n";

	// Sorted Quest Order
	//
	// Display order
	std::sort(questData.questInfo.begin(), questData.questInfo.end(),
		[&](QuestInfo const& a, QuestInfo const& b) -> bool
		{
			auto const& groupA = questData.displayGroups[a.displayGroup];
			auto const& groupB = questData.displayGroups[b.displayGroup];

			if (groupA.m_GroupIndex != groupB.m_GroupIndex)
			{
				return groupA.m_GroupIndex < groupB.m_GroupIndex;
			}
			else
			{
				// In same group
				if (groupA.m_SortAlphabetically)
				{
					// Sort alphabetically in same display order index
					int compare = a.questId.compare(b.questId);

					if (compare < 0)
						return true;

					return false;
				}
				else
				{
					return a.importIndex < b.importIndex;
				}
			}
		}
	);


	// Quest UI order
	fileStream << "static u16 const sQuestDisplayOrder[] =\n{\n";
	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "QUEST_ID_" << quest.questId << ",\n";

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "};\n\n";

	// Alphabetical order
	std::sort(questData.questInfo.begin(), questData.questInfo.end(),
		[](QuestInfo const& a, QuestInfo const& b) -> bool
		{
			int compare = a.questId.compare(b.questId);

			if (compare == 0)
				return a.importIndex < b.importIndex;

			if (compare < 0)
				return true;

			return false;
		}
	);

	fileStream << "static u16 const sQuestAlphabeticalOrder[] =\n{\n";
	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "QUEST_ID_" << quest.questId << ",\n";

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "};\n\n";

}

void ExportQuestData_H(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	QuestData questData;
	GatherQuests(dataPath, jsonData, questData);

	// Enum define
	fileStream << "enum\n{\n";
	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "QUEST_ID_" << quest.questId << ",\n";

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << c_TabSpacing << "QUEST_ID_COUNT,\n";
	fileStream << "};\n\n";
}

void ExportQuestData_ConstsH(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	QuestData questData;
	GatherQuests(dataPath, jsonData, questData);

	// Save count define
	int counter = 0;

	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;
		int index = counter++;

		if (!quest.preprocessorCondition.empty())
		{
			fileStream << "#if " << quest.preprocessorCondition << "\n";
			fileStream << "\t#define __QUEST_COUNT_" << quest.questId << "_" << index << " 1\n";
			fileStream << "#else\n";
			fileStream << "\t#define __QUEST_COUNT_" << quest.questId << "_" << index << " 0\n";
			fileStream << "#endif\n";
		}
		else
			fileStream << "#define __QUEST_COUNT_" << quest.questId << "_" << index << " 1\n";
	}

	counter = 0;

	fileStream << "\n\n";
	fileStream << "#define QUEST_SAVE_COUNT (0 \\\n";
	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		int index = counter++;

		auto const& quest = *it;
		fileStream << "\t+ __QUEST_COUNT_" << quest.questId << "_" << index << " \\\n";
	}
	fileStream << ")\n\n";
}

void ExportQuestData_Pory(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData)
{
	QuestData questData;
	GatherQuests(dataPath, jsonData, questData);

	for (auto it = questData.questInfo.begin(); it != questData.questInfo.end(); ++it)
	{
		auto const& quest = *it;

		fileStream << "text gQuestDescText_" << quest.GetUniqueWriteId() << "\n{\n";
		fileStream << c_TabSpacing << "format(\"" << GetQuestDescription(quest) << "\")\n";
		fileStream << "}\n\n";
	}
}

static std::string FormatQuestId(std::string const& prettyName)
{
	std::string questId = strutil::to_upper(prettyName);
	strutil::replace_all(questId, " ", "_");
	strutil::replace_all(questId, "-", "_");
	strutil::replace_all(questId, "!", "EMARK");
	strutil::replace_all(questId, "?", "QMARK");
	strutil::replace_all(questId, "Ã©", "E"); // code for é
	strutil::replace_all(questId, "é", "E");
	strutil::replace_all(questId, ",", "");
	strutil::replace_all(questId, ".", "");
	strutil::replace_all(questId, "\"", "");
	strutil::replace_all(questId, "'", "");
	strutil::replace_all(questId, "(", "");
	strutil::replace_all(questId, ")", "");
	strutil::replace_all(questId, "[", "");
	strutil::replace_all(questId, "]", "");
	strutil::replace_all(questId, "{", "");
	strutil::replace_all(questId, "}", "");
	strutil::replace_all(questId, "+", "PLUS");
	strutil::replace_all(questId, c_Elipsies, "");
	return questId;
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

static QuestReward ParseQuestReward(json const& jsonData)
{
	QuestReward reward;

	// Preprocessor condition
	if (jsonData.contains("#if"))
	{
		reward.preprocessorCondition = jsonData["#if"].get<std::string>();
	}

	if (jsonData.contains("visibility"))
		reward.visibility = GetAsString(jsonData["visibility"]);
	else
		reward.visibility = "DEFAULT";

	if (jsonData.contains("difficulty"))
		reward.requiredDifficulty = GetAsString(jsonData["difficulty"]);
	else
		reward.requiredDifficulty = "EASY";

	// Custom popup
	if (jsonData.contains("custom_popup"))
	{
		auto const& customPopup = jsonData["custom_popup"];

		reward.customPopup.isValid = true;

		if (customPopup.contains("item_icon"))
			reward.customPopup.itemIcon = customPopup["item_icon"].get<std::string>();

		if (customPopup.contains("species_icon"))
			reward.customPopup.speciesIcon = customPopup["species_icon"].get<std::string>();

		reward.customPopup.title = customPopup["title"].get<std::string>();
		reward.customPopup.subtitle = customPopup["subtitle"].get<std::string>();

		if (customPopup.contains("sound_effect"))
			reward.customPopup.soundEffect = customPopup["sound_effect"].get<std::string>();
		else
			reward.customPopup.soundEffect = "0";

		if (customPopup.contains("fanfare"))
			reward.customPopup.fanfare = customPopup["fanfare"].get<std::string>();
		else
			reward.customPopup.fanfare = "0";
	}
	else
	{
		reward.customPopup.isValid = false;
	}

	// Per type
	if (jsonData.contains("species"))
	{
		reward.type = QuestRewardType::Pokemon;

		reward.pokemonParams.species = jsonData["species"].get<std::string>();

		if (jsonData.contains("shiny"))
			reward.pokemonParams.isShiny = GetAsString(jsonData["shiny"]);
		else
			reward.pokemonParams.isShiny = "FALSE";

		if (jsonData.contains("nickname"))
			reward.pokemonParams.nickname = GetAsString(jsonData["nickname"]);
		else
			reward.pokemonParams.nickname = "";

		if (jsonData.contains("custom_mon_id"))
			reward.pokemonParams.customMonId = GetAsString(jsonData["custom_mon_id"]);
		else
			reward.pokemonParams.customMonId = "CUSTOM_MON_NONE";

		return reward;
	}

	if (jsonData.contains("item"))
	{
		reward.type = QuestRewardType::Item;

		reward.itemParams.item = jsonData["item"].get<std::string>();

		if (jsonData.contains("count"))
			reward.itemParams.count = GetAsString(jsonData["count"]);
		else
			reward.itemParams.count = "1";

		return reward;
	}

	if (jsonData.contains("shop_item"))
	{
		reward.type = QuestRewardType::ShopItem;

		reward.shopItemParams.item = jsonData["shop_item"].get<std::string>();

		return reward;
	}

	if (jsonData.contains("money"))
	{
		reward.type = QuestRewardType::Money;

		reward.moneyParams.amount = GetAsString(jsonData["money"]);

		return reward;
	}

	if (jsonData.contains("quest"))
	{
		reward.type = QuestRewardType::QuestUnlock;

		reward.questUnlockParams.questId = FormatQuestId(jsonData["quest"].get<std::string>());

		return reward;
	}

	if (jsonData.contains("hub_upgrade"))
	{
		reward.type = QuestRewardType::HubUpgrade;

		reward.hubUpgrade.upgradeId = jsonData["hub_upgrade"].get<std::string>();

		return reward;
	}

	if (jsonData.contains("flag"))
	{
		reward.type = QuestRewardType::Flag;

		reward.flagParams.flag = FormatQuestId(jsonData["flag"].get<std::string>());

		return reward;
	}

	if (jsonData.contains("decor"))
	{
		reward.type = QuestRewardType::Decor;

		reward.decor.decorId = jsonData["decor"].get<std::string>();

		return reward;
	}

	if (jsonData.contains("decor_variant"))
	{
		reward.type = QuestRewardType::DecorVariant;

		reward.decorVariant.decorVariantId = jsonData["decor_variant"].get<std::string>();

		return reward;
	}

	if (jsonData.contains("outfit_unlock"))
	{
		reward.type = QuestRewardType::OutfitUnlock;

		reward.outfitUnlock.outfitUnlockId = jsonData["outfit_unlock"].get<std::string>();

		return reward;
	}
	
	FATAL_ERROR("Unrecognised reward object:\n'%s'", jsonData.dump().c_str());
	return reward;
}

static QuestRequirement ParseQuestRequirement(json const& jsonData)
{
	QuestRequirement requirement;

	// Preprocessor condition
	if (jsonData.contains("#if"))
	{
		requirement.preprocessorCondition = jsonData["#if"].get<std::string>();
	}


	// Per type
	if (jsonData.contains("item"))
	{
		requirement.type = QuestRequirementType::Item;

		requirement.itemParams.item = jsonData["item"].get<std::string>();

		if (jsonData.contains("count"))
			requirement.itemParams.count = GetAsString(jsonData["count"]);
		else
			requirement.itemParams.count = "1";

		if (jsonData.contains("operation"))
			requirement.itemParams.operation = GetAsString(jsonData["operation"]);
		else
			requirement.itemParams.operation = "GREATER_THAN_EQUAL";

		return requirement;
	}

	if (jsonData.contains("flag"))
	{
		requirement.type = QuestRequirementType::Flag;

		requirement.flagParams.flag = jsonData["flag"].get<std::string>();

		if (jsonData.contains("state"))
			requirement.flagParams.state = GetAsString(jsonData["state"]);
		else
			requirement.flagParams.state = "TRUE";

		return requirement;
	}

	if (jsonData.contains("config_toggle"))
	{
		requirement.type = QuestRequirementType::ConfigToggle;

		requirement.configToggleParams.configToggle = jsonData["config_toggle"].get<std::string>();

		if (jsonData.contains("state"))
			requirement.configToggleParams.state = GetAsString(jsonData["state"]);
		else
			requirement.configToggleParams.state = "TRUE";

		return requirement;
	}

	if (jsonData.contains("config_range"))
	{
		requirement.type = QuestRequirementType::ConfigRange;

		requirement.configRangeParams.configRange = jsonData["config_range"].get<std::string>();
		requirement.configRangeParams.value = GetAsString(jsonData["value"]);

		if (jsonData.contains("operation"))
			requirement.configRangeParams.operation = GetAsString(jsonData["operation"]);
		else
			requirement.configRangeParams.operation = "EQUAL";

		return requirement;
	}


	FATAL_ERROR("Unrecognised requirement object:\n'%s'", jsonData.dump().c_str());
	return requirement;
}

static void GatherQuests(std::string const& dataPath, json const& rawJsonData, QuestData& outQuestData)
{
	json jsonData = ExpandCommonArrayGroup(dataPath, rawJsonData, "quest_groups");
	json questGroups = jsonData["quest_groups"];
	int counter = 0;

	for (auto groupIt = questGroups.begin(); groupIt != questGroups.end(); ++groupIt)
	{
		json quests = groupIt.value();

		for (auto questIt : quests)
		{
			QuestInfo quest = { groupIt.value(), questIt };

			// Quest ID
			quest.questId = FormatQuestId(GetQuestName(quest));
			quest.importIndex = counter++;

			// Display Group Name
			if (quest.questObj.contains("display_group"))
			{
				quest.displayGroup = quest.questObj["display_group"].get<std::string>();
			}

			// Preprocessor condition
			if (quest.groupObj.contains("#if"))
			{
				quest.preprocessorCondition = quest.groupObj["#if"].get<std::string>();
			}

			if (quest.questObj.contains("#if"))
			{
				if (quest.preprocessorCondition.empty())
					quest.preprocessorCondition = quest.questObj["#if"].get<std::string>();
				else
					quest.preprocessorCondition += " && " + quest.questObj["#if"].get<std::string>();
			}

			// Quest flags
			if (quest.questObj.contains("flags"))
			{
				for (auto flagObj : quest.questObj["flags"])
				{
					quest.flags.push_back(flagObj.get<std::string>());
				}
			}

			// Triggers
			// (Json params are made to be more UX friendly)
			if (quest.questObj.contains("triggers"))
			{
				for (auto triggerInfo : quest.questObj["triggers"])
				{
					QuestTrigger trigger;

					trigger.callback = triggerInfo["condition"].get<std::string>();

					if (triggerInfo.contains("pass"))
						trigger.passState = triggerInfo["pass"].get<std::string>();
					else
						trigger.passState = "PENDING";

					if (triggerInfo.contains("fail"))
						trigger.failState = triggerInfo["fail"].get<std::string>();
					else
						trigger.failState = "PENDING";

					for (auto triggerFlag : triggerInfo["on"])
					{
						std::string flag = triggerFlag.get<std::string>();
						trigger.flags.push_back(flag);

						if(std::find(quest.collatedTriggerFlags.begin(), quest.collatedTriggerFlags.end(), flag) == quest.collatedTriggerFlags.end())
							quest.collatedTriggerFlags.push_back(flag);
					}

					if (triggerInfo.contains("params"))
					{
						for (auto param : triggerInfo["params"])
						{
							if (param.is_number_integer())
								trigger.params.push_back(std::to_string(param.get<int>()));
							else if (param.is_number_float())
								trigger.params.push_back(std::to_string(param.get<double>()));
							else
								trigger.params.push_back(param.get<std::string>());
						}
					}

					// Allow redirecting so can prevent duplicate source data
					if (triggerInfo.contains("template_params"))
					{
						json templateParams = quest.questObj["template_params"];

						for (auto sourceParam : triggerInfo["template_params"])
						{
							json actualParam = templateParams[sourceParam.get<std::string>()];

							if (actualParam.is_number_integer())
								trigger.params.push_back(std::to_string(actualParam.get<int>()));
							else if (actualParam.is_number_float())
								trigger.params.push_back(std::to_string(actualParam.get<double>()));
							else
								trigger.params.push_back(actualParam.get<std::string>());
						}
					}

					quest.triggers.push_back(std::move(trigger));
				}
			}

			// Rewards
			if (quest.questObj.contains("rewards"))
			{
				for (json rewardObj : quest.questObj["rewards"])
				{
					QuestReward reward = ParseQuestReward(rewardObj);
					quest.rewards.push_back(std::move(reward));
				}
			}

			// Requirements
			if (quest.questObj.contains("requirements"))
			{
				for (json requirementObj : quest.questObj["requirements"])
				{
					QuestRequirement requirement = ParseQuestRequirement(requirementObj);
					quest.requirements.push_back(std::move(requirement));
				}
			}

			outQuestData.questInfo.push_back(quest);
		}
	}

	// Figure out the display order here based on the groups
	json displayGroups = rawJsonData["display_groups"];
	counter = 0;

	for (auto displayIt = displayGroups.begin(); displayIt != displayGroups.end(); ++displayIt)
	{
		json groupObj = *displayIt;
		DisplayGroup group;

		std::string id = groupObj["id"].get<std::string>();
		group.m_GroupIndex = counter++;
		group.m_SortAlphabetically = false;

		if (groupObj.contains("sort_alphabetically"))
			group.m_SortAlphabetically = groupObj["sort_alphabetically"].get<bool>();

		outQuestData.displayGroups[id] = group;
	}

	// Populate prerequisite quests based on rewards
	// (We can point to multiple quests if we have Vanilla/EX versions of the same quest)
	std::unordered_map<std::string, std::vector<QuestInfo*>> questLookup;

	for (auto& quest : outQuestData.questInfo)
	{
		questLookup[quest.questId].push_back(&quest);
	}

	for (auto& quest : outQuestData.questInfo)
	{
		for (auto& reward : quest.rewards)
		{
			if (reward.type == QuestRewardType::QuestUnlock)
			{
				for (auto* otherQuest : questLookup[reward.questUnlockParams.questId])
				{
					otherQuest->isUnlockedViaReward = true;
				}
			}
		}
	}

	for (auto& quest : outQuestData.questInfo)
	{
		if (!quest.isUnlockedViaReward)
		{
			quest.flags.push_back("UNLOCKED_BY_DEFAULT");
		}
	}
}