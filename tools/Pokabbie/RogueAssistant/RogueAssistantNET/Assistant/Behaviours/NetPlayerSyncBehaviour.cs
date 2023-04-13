using RogueAssistantNET.Game;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant.Behaviours
{
	public class NetPlayerData
	{
		public byte[] ProfileData;
		public byte[] StateData;

		public string PlayerName
		{
			get 
			{ 
				if(ProfileData != null) 
				{
					return GameString.ConvertBytes(ProfileData, 0, GameConstants.PlayerNameLength);
				}

				return "???";
			}
        }

        public int PosX
        {
            get
            {
                if (StateData != null)
                {
                    return BitConverter.ToInt16(StateData.Skip(0).Take(2).ToArray(), 0);
                }

                return -1;
            }
        }

        public int PosY
        {
            get
            {
                if (StateData != null)
                {
                    return BitConverter.ToInt16(StateData.Skip(2).Take(2).ToArray(), 0);
                }

                return -1;
            }
        }
    }

	public class NetPlayerSyncBehaviour : IRogueAssistantBehaviour
	{
		private uint m_MaxPlayerCount;

		private uint m_PlayerProfileAddress;
		private uint m_PlayerProfileSize;
		private uint m_PlayerStateAddress;
		private uint m_PlayerStateSize;

		private NetPlayerData m_LocalPlayerData;
		private List<NetPlayerData> m_OnlinePlayerData = new List<NetPlayerData>();

		public uint MaxPlayerCount
		{
			get => m_MaxPlayerCount;
		}

		public NetPlayerData LocalPlayer
		{
			get => m_LocalPlayerData;
		}

		public IEnumerable<NetPlayerData> OnlinePlayerData
		{
			get => m_OnlinePlayerData;
		}

		public void OnAttach(RogueAssistant assistant)
		{
			var conn = assistant.Connection;
			m_MaxPlayerCount = conn.State.GetConstantValue(GameStateConstant.NetPlayerCapacity);
			m_PlayerProfileAddress = conn.State.GetConstantValue(GameStateConstant.NetPlayerProfileAddress);
			m_PlayerProfileSize = conn.State.GetConstantValue(GameStateConstant.NetPlayerProfileSize);
			m_PlayerStateAddress = conn.State.GetConstantValue(GameStateConstant.NetPlayerStateAddress);
			m_PlayerStateSize = conn.State.GetConstantValue(GameStateConstant.NetPlayerStateSize);

			m_LocalPlayerData = new NetPlayerData();
			RefreshLocalPlayer(conn);
		}

		public void OnDetach(RogueAssistant assistant)
		{
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			var conn = assistant.Connection;

			uint playerId = 0;
			ReadPlayerState(conn, playerId, m_LocalPlayerData);

			++playerId;

			foreach (var playerData in m_OnlinePlayerData)
			{
				WritePlayerProfile(conn, playerId, playerData);
				WritePlayerState(conn, playerId, playerData);

				if (++playerId >= m_MaxPlayerCount)
					break;
			}

			for(; playerId < m_MaxPlayerCount; ++playerId)
			{
				WriteEmptyPlayerProfile(conn, playerId);
			}
		}

		public void RefreshLocalPlayer(GameConnection conn)
		{
			ReadPlayerProfile(conn, 0, m_LocalPlayerData);
			ReadPlayerState(conn, 0, m_LocalPlayerData);
		}

		public void AddOnlinePlayer(NetPlayerData player)
		{
			m_OnlinePlayerData.Add(player);
		}

		public bool RemoveOnlinePlayer(NetPlayerData player)
		{
			return m_OnlinePlayerData.Remove(player);
		}

		private void ReadPlayerProfile(GameConnection conn, uint index, NetPlayerData player)
		{
			player.ProfileData = conn.ReadBytes(m_PlayerProfileAddress + index * m_PlayerProfileSize, m_PlayerProfileSize);
		}

		private void ReadPlayerState(GameConnection conn, uint index, NetPlayerData player)
		{
			player.StateData = conn.ReadBytes(m_PlayerStateAddress + index * m_PlayerStateSize, m_PlayerStateSize);
		}

		private void WritePlayerProfile(GameConnection conn, uint index, NetPlayerData player)
		{
			// TODO - Verify  m_PlayerProfileSize
			conn.WriteBytes(m_PlayerProfileAddress + index * m_PlayerProfileSize, player.ProfileData);
		}

		private void WriteEmptyPlayerProfile(GameConnection conn, uint index)
		{
			conn.WriteBytes(m_PlayerProfileAddress + index * m_PlayerProfileSize, new byte[m_PlayerProfileSize]);
		}

		private void WritePlayerState(GameConnection conn, uint index, NetPlayerData player)
		{
			// TODO - Verify  m_PlayerStateSize
			conn.WriteBytes(m_PlayerStateAddress + index * m_PlayerStateSize, player.StateData);
		}
	}
}
