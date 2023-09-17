#include "GameConnectionRPCs.h"
#include "GameConnection.h"
#include "Log.h"

enum class MethodId
{
    Echo,
    ReadConstant,
    BeginMultiplayerHost,
    BeginMultiplayerClient,
    EndMultiplayer,
    GetSpeciesName,
    GetItemName,
};

RPCQueue::RPCQueue(GameConnection& game)
    : m_Game(game)
    , m_Counter(0)
    , m_SendBufferSize(0)
{
}

void RPCQueue::Update()
{
    //GameStructures::RogueAssistantState const& assistantState = m_Game.GetRogueAssistantState();
    //
    //static bool s_RunTest = false;
    //if (!s_RunTest)
    //{
    //    u8 nameSize = m_Game.GetGameGFHeader().pokemonNameLength1 + 1;
    //    m_Game.ReadData(m_Game.GetGameGFHeader().monSpeciesNames + nameSize * 3, nameSize)
    //        ->Then([&](u8 const* data, size_t size)
    //            {
    //                std::string name = GameHelpers::ParseGameString(data, size);
    //                return;
    //            }
    //    );
    //}

}

void RPCQueue::PushSendBufferData(void const* data, size_t size)
{
    ASSERT_MSG(m_SendBufferSize + size < sizeof(m_SendBuffer), "RPC buffer full");
    memcpy(&m_SendBuffer[m_SendBufferSize], data, size);
    m_SendBufferSize += size;
}

void RPCQueue::ClearSendBuffer()
{
    m_SendBufferSize = 0;
}

void RPCQueue::RPC_GetSpeciesName(u16 species)
{
    //ClearSendBuffer();
    //PushSendData<u16>(m_Counter++);
    //PushSendData<u16>((u16)MethodId::GetSpeciesName);
    //PushSendData<u16>(species);
    //
    //GameStructures::RogueAssistantState const& assistantState = m_Game.GetRogueAssistantState();
    //m_Game.WriteData(assistantState.assistantState + offsetof(GameStructures::RogueAssistantState, inCommBuffer), m_SendBuffer, m_SendBufferSize);
}

//void RPCQueue::GetSpeciesName(u16 species)
//{
//
//}

// Hard coded method IDs synced with the game
//enum class ConstantId
//{
//    GAME_CONSTANT_ASSISTANT_STATE_NUM_ADDRESS,
//    GAME_CONSTANT_ASSISTANT_SUBSTATE_NUM_ADDRESS,
//    GAME_CONSTANT_REQUEST_STATE_NUM_ADDRESS,
//
//    GAME_CONSTANT_SAVE_BLOCK1_PTR_ADDRESS,
//    GAME_CONSTANT_SAVE_BLOCK2_PTR_ADDRESS,
//    GAME_CONSTANT_NET_PLAYER_CAPACITY,
//    GAME_CONSTANT_NET_PLAYER_PROFILE_ADDRESS,
//    GAME_CONSTANT_NET_PLAYER_PROFILE_SIZE,
//    GAME_CONSTANT_NET_PLAYER_STATE_ADDRESS,
//    GAME_CONSTANT_NET_PLAYER_STATE_SIZE,
//
//    GAME_CONSTANT_DEBUG_MAIN_ADDRESS,
//    GAME_CONSTANT_DEBUG_QUERY_UNCOLLAPSE_BUFFER_PTR,
//    GAME_CONSTANT_DEBUG_QUERY_UNCOLLAPSE_CAPACITY,
//    GAME_CONSTANT_DEBUG_QUERY_COLLAPSED_BUFFER_PTR,
//    GAME_CONSTANT_DEBUG_QUERY_COLLAPSED_SIZE_PTR,
//};
