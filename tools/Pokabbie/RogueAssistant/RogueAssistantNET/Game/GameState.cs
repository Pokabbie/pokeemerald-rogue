using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game
{
	public enum InternalGameState
	{
		None,
		MultiplayerHost,
		MultiplayerJoin
	}

	public enum GameStateConstant
	{
		GameStateAddress,
		GameSubStateAddress,
		SaveBlock1Ptr,
		SaveBlock2Ptr,
		NetPlayerCapacity,
		NetPlayerProfileAddress,
		NetPlayerProfileSize,
		NetPlayerStateAddress,
		NetPlayerStateSize,

		DebugMainAddress,
	}

	public class GameState
	{
		private GameConnection m_Connection;
		private bool m_FirstUpdate = true;

		private ushort m_CommandTokenCounter = 0;
		private Dictionary<GameStateConstant, uint> m_CachedConstants = new Dictionary<GameStateConstant, uint>();

		private byte[] m_PlayerNameData;
		private byte m_PlayerAvatar;
		private InternalGameState m_InternalGameStateId;

		public GameState(GameConnection connection)
		{
			m_Connection = connection;
			m_CommandTokenCounter = connection.ReadU16(connection.Header.RogueOutputBufferAddress);
		}

		public string PlayerNameStr
		{
			get => m_PlayerNameData != null ? GameString.ConvertBytes(m_PlayerNameData, 0, (uint)m_PlayerNameData.Length) : "";
		}
		public byte[] PlayerNameBytes
		{
			get => m_PlayerNameData;
		}

		public InternalGameState InternalGameState
		{
			get => m_InternalGameStateId;
		}

		public byte PlayerAvatar
		{
			get => m_PlayerAvatar;
		}

		public void Update()
		{
			if(m_FirstUpdate)
			{
				m_FirstUpdate = false;
				RefreshInfrequentData();
			}

			uint gameStateAddr = GetConstantValue(GameStateConstant.GameStateAddress);
			m_InternalGameStateId = (InternalGameState)m_Connection.ReadU16(gameStateAddr);

			// Always do a read to check we're still connection
			m_Connection.ReadU8(GameConstants.GFHeaderAddress);

        }

		public void RefreshInfrequentData()
		{
			uint saveBlock2PtrAddress = GetConstantValue(GameStateConstant.SaveBlock2Ptr);
			uint saveBlock2Address = m_Connection.ReadU32(saveBlock2PtrAddress);

			m_PlayerNameData = m_Connection.ReadBytes(saveBlock2Address + 0, GameConstants.PlayerNameLength + 1);
			m_PlayerAvatar = m_Connection.ReadU8(saveBlock2Address + 8);
		}

		public ushort NextCommandToken()
		{
			++m_CommandTokenCounter;

			// 0 reserved for NULL token
			if (m_CommandTokenCounter == 0)
				m_CommandTokenCounter = 1;

			return m_CommandTokenCounter;
		}

		public uint GetConstantValue(GameStateConstant constant)
		{
			uint value;
			if (m_CachedConstants.TryGetValue(constant, out value))
				return value;

			m_Connection.WriteU16(m_Connection.GameCommandArgAddress, (ushort)constant);
			uint readAddress = m_Connection.SendReliable(GameCommandCode.ReadConstant);

			value = m_Connection.ReadU32(readAddress);
			m_CachedConstants[constant] = value;
			return value;
		}

		public void ClearConstantValueCache(GameStateConstant constant)
		{
			m_CachedConstants.Remove(constant);
		}
	}
}
