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
};

struct QuestReward
{
	QuestRewardType type;
	std::string preprocessorCondition;
	struct
	{
		std::string species;
		bool isShiny;
	} 
	pokemonParams;
	struct
	{
		std::string item;
		int count;
	} 
	itemParams;
	struct
	{
		std::string item;
	} 
	shopItemParams;
	struct
	{
		int amount;
	} 
	moneyParams;
	struct
	{
		std::string questId;
	}
	questUnlockParams;
};

struct QuestInfo
{
	json groupObj;
	json questObj;
	std::string questId;
	std::string preprocessorCondition;
	int sortOrder;
	bool isUnlockedViaReward;
	std::vector<std::string> flags;
	std::string triggerFunc;
	std::vector<std::string> triggerFlags;
	std::vector<std::string> triggerParams;
	std::vector<QuestReward> rewards;
};

static void GatherQuests(json const& jsonData, std::vector<QuestInfo>& outQuestInfo);

static std::string FlagsToString(std::string const& prefix, std::vector<std::string> const flags)
{
	std::string output = "0";

	for (auto const& flag : flags)
		output += " | " + prefix + flag;

	return output;
}

void ExportQuestData_C(std::ofstream& fileStream, json const& jsonData)
{
	std::vector<QuestInfo> questInfo;
	GatherQuests(jsonData, questInfo);

	// Populate lookup
	std::unordered_map<std::string, QuestInfo*> questLookup;

	for (auto& quest : questInfo)
		questLookup[quest.questId] = &quest;

	// Required data
	for (auto it = questInfo.begin(); it != questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << "static u8 const sTitle_" << quest.questId << "[] = _(\"" << quest.questObj["name"].get<std::string>() << "\");\n";
		fileStream << "extern const u8 gQuestDescText_" << quest.questId << "[];\n";

		// Triggers
		fileStream << "static u16 const sTriggerParams_" << quest.questId << "[] = \n";
		fileStream << "{\n";

		for(auto const& triggerParam : quest.triggerParams)
			fileStream << c_TabSpacing << triggerParam << ",\n";

		fileStream << "};\n";

		// Rewards
		fileStream << "static struct RogueQuestRewardNEW const sRewards_" << quest.questId << "[] = \n";
		fileStream << "{\n";

		for (auto const& rewardInfo : quest.rewards)
		{
			if (!rewardInfo.preprocessorCondition.empty())
				fileStream << "#if " << rewardInfo.preprocessorCondition << "\n";

			fileStream << c_TabSpacing << "{\n";

			switch (rewardInfo.type)
			{
			case QuestRewardType::Pokemon:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_POKEMON,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".pokemon = {\n";
				fileStream << c_TabSpacing3 << ".species = " << rewardInfo.pokemonParams.species << ",\n";
				fileStream << c_TabSpacing3 << ".isShiny = " << (rewardInfo.pokemonParams.isShiny ? "TRUE" : "FALSE") << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::Item:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_ITEM,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".item = {\n";
				fileStream << c_TabSpacing3 << ".item = " << rewardInfo.itemParams.item << ",\n";
				fileStream << c_TabSpacing3 << ".count = " << rewardInfo.itemParams.count << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::ShopItem:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_SHOP_ITEM,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".shopItem = {\n";
				fileStream << c_TabSpacing3 << ".item = " << rewardInfo.shopItemParams.item << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::Money:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_MONEY,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".money = {\n";
				fileStream << c_TabSpacing3 << ".amount = " << rewardInfo.moneyParams.amount << ",\n";
				fileStream << c_TabSpacing3 << "}\n";
				fileStream << c_TabSpacing2 << "}\n";
				break;

			case QuestRewardType::QuestUnlock:
				fileStream << c_TabSpacing2 << ".type = QUEST_REWARD_QUEST_UNLOCK,\n";
				fileStream << c_TabSpacing2 << ".perType = {\n";
				fileStream << c_TabSpacing3 << ".questUnlock = {\n";
				fileStream << c_TabSpacing3 << ".questId = QUEST_ID_" << rewardInfo.questUnlockParams.questId << ",\n";
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

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";

		fileStream << "\n";
	}

	// Quest array
	fileStream << "static struct RogueQuestEntry const sQuestEntries[] =\n{\n";
	for (auto it = questInfo.begin(); it != questInfo.end(); ++it)
	{
		auto const& quest = *it;

		if (!quest.preprocessorCondition.empty())
			fileStream << "#if " << quest.preprocessorCondition << "\n";

		fileStream << c_TabSpacing << "[QUEST_ID_" << quest.questId << "] = \n";
		fileStream << c_TabSpacing << "{\n";


		fileStream << c_TabSpacing2 << ".title = sTitle_" << quest.questId << ",\n";
		fileStream << c_TabSpacing2 << ".desc = gQuestDescText_" << quest.questId << ",\n";
		fileStream << c_TabSpacing2 << ".flags = " << FlagsToString("QUEST_CONST_", quest.flags) << ",\n";

		fileStream << c_TabSpacing2 << ".triggerFunc = " << quest.triggerFunc << ",\n";
		fileStream << c_TabSpacing2 << ".triggerFlags = " << FlagsToString("QUEST_TRIGGER_", quest.triggerFlags) << ",\n";
		fileStream << c_TabSpacing2 << ".triggerParams = " << "sTriggerParams_" << quest.questId << ",\n";
		fileStream << c_TabSpacing2 << ".triggerParamCount = " << "ARRAY_COUNT(sTriggerParams_" << quest.questId << "),\n";

		fileStream << c_TabSpacing << "},\n";

		if (!quest.preprocessorCondition.empty())
			fileStream << "#endif\n";
	}
	fileStream << "};\n\n";
}

void ExportQuestData_H(std::ofstream& fileStream, json const& jsonData)
{
	std::vector<QuestInfo> questInfo;
	GatherQuests(jsonData, questInfo);

	// Enum define
	fileStream << "enum\n{\n";
	for (auto it = questInfo.begin(); it != questInfo.end(); ++it)
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

void ExportQuestData_Pory(std::ofstream& fileStream, json const& jsonData)
{
	std::vector<QuestInfo> questInfo;
	GatherQuests(jsonData, questInfo);

	for (auto it = questInfo.begin(); it != questInfo.end(); ++it)
	{
		auto const& quest = *it;

		fileStream << "text gQuestDescText_" << quest.questId << "\n{\n";
		fileStream << c_TabSpacing << "format(\"" << quest.questObj["description"].get<std::string>() << "\")\n";
		fileStream << "}\n\n";
	}
}

static std::string FormatQuestId(std::string const& prettyName)
{
	std::string questId = strutil::to_upper(prettyName);
	strutil::replace_all(questId, " ", "_");
	strutil::replace_all(questId, "!", "");
	strutil::replace_all(questId, "?", "");
	strutil::replace_all(questId, ",", "");
	strutil::replace_all(questId, ".", "");
	return questId;
}

static QuestReward ParseQuestReward(json const& jsonData)
{
	QuestReward reward;

	// Preprocessor condition
	if (jsonData.contains("#if"))
	{
		reward.preprocessorCondition = jsonData["#if"].get<std::string>();
	}

	// Per type
	if (jsonData.contains("species"))
	{
		reward.type = QuestRewardType::Pokemon;

		reward.pokemonParams.species = jsonData["species"].get<std::string>();

		if (jsonData.contains("shiny"))
			reward.pokemonParams.isShiny = jsonData["shiny"].get<bool>();
		else
			reward.pokemonParams.isShiny = false;

		return reward;
	}

	if (jsonData.contains("item"))
	{
		reward.type = QuestRewardType::Item;

		reward.itemParams.item = jsonData["item"].get<std::string>();

		if (jsonData.contains("count"))
			reward.itemParams.count = jsonData["count"].get<int>();
		else
			reward.itemParams.count = 1;

		return reward;
	}

	if (jsonData.contains("shop_item"))
	{
		reward.type = QuestRewardType::ShopItem;

		reward.itemParams.item = jsonData["shop_item"].get<std::string>();

		return reward;
	}

	if (jsonData.contains("money"))
	{
		reward.type = QuestRewardType::Money;

		reward.moneyParams.amount = jsonData["money"].get<int>();

		return reward;
	}

	if (jsonData.contains("quest"))
	{
		reward.type = QuestRewardType::QuestUnlock;

		reward.questUnlockParams.questId = FormatQuestId(jsonData["quest"].get<std::string>());

		return reward;
	}
	
	FATAL_ERROR("Unrecognised reward object:\n'%s'", jsonData.dump().c_str());
	return reward;
}

static void GatherQuests(json const& jsonData, std::vector<QuestInfo>& outQuestInfo)
{
	json questGroups = jsonData["quest_groups"];

	for (auto groupIt = questGroups.begin(); groupIt != questGroups.end(); ++groupIt)
	{
		json quests = groupIt.value()["quests"];

		for (auto questIt : quests)
		{
			QuestInfo quest = { groupIt.value(), questIt };

			// Quest ID
			quest.questId = FormatQuestId(quest.questObj["name"].get<std::string>());

			// Sort order
			if (quest.groupObj.contains("sort_order"))
			{
				quest.sortOrder += quest.groupObj["sort_order"].get<int>() * 10000;
			}

			if (quest.questObj.contains("sort_order"))
			{
				quest.sortOrder += quest.questObj["sort_order"].get<int>();
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
			if (quest.groupObj.contains("group_flags"))
			{
				for (auto flagObj : quest.groupObj["group_flags"])
				{
					quest.flags.push_back(flagObj.get<std::string>());
				}
			}

			if (quest.questObj.contains("group_flags"))
			{
				for (auto flagObj : quest.questObj["group_flags"])
				{
					quest.flags.push_back(flagObj.get<std::string>());
				}
			}

			// Triggers
			if (quest.questObj.contains("callback"))
			{
				auto const& callbackInfo = quest.questObj["callback"];

				quest.triggerFunc = callbackInfo["method"].get<std::string>();

				for (auto triggerFlag : callbackInfo["triggers"])
				{
					quest.triggerFlags.push_back(triggerFlag.get<std::string>());
				}

				if (callbackInfo.contains("params"))
				{
					for (auto param : callbackInfo["params"])
					{
						if (param.is_number_integer())
							quest.triggerParams.push_back(std::to_string(param.get<int>()));
						else if (param.is_number_float())
							quest.triggerParams.push_back(std::to_string(param.get<double>()));
						else
							quest.triggerParams.push_back(param.get<std::string>());
					}
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

			outQuestInfo.push_back(quest);
		}
	}

	// Sort quests
	std::sort(outQuestInfo.begin(), outQuestInfo.end(), 
		[](QuestInfo const& a, QuestInfo const& b) -> bool
		{
			if (a.sortOrder == b.sortOrder)
			{
				if (a.questId.compare(b.questId) < 0)
					return true;
			}
			else if (a.sortOrder < b.sortOrder)
				return true;

			return false;
		}
	);

	// Populate prerequisite quests based on rewards
	std::unordered_map<std::string, QuestInfo*> questLookup;

	for (auto& quest : outQuestInfo)
	{
		questLookup[quest.questId] = &quest;
	}

	for (auto& quest : outQuestInfo)
	{
		for (auto& reward : quest.rewards)
		{
			if (reward.type == QuestRewardType::QuestUnlock)
			{
				auto& unlockedQuest = *questLookup[reward.questUnlockParams.questId];
				unlockedQuest.isUnlockedViaReward = true;
			}
		}
	}

	for (auto& quest : outQuestInfo)
	{
		if (!quest.isUnlockedViaReward)
		{
			quest.flags.push_back("UNLOCKED_BY_DEFAULT");
		}
	}
}