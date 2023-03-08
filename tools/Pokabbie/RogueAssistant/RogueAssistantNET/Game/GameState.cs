using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game
{
	public enum GameStateConstant
	{
		SaveBlock1Ptr,
		NetPlayerAddress,
		NetPlayerCapacity,
	}

	public struct PlayerPositionData
	{
		public short x;
		public short y;
		public sbyte mapGroup;
		public sbyte mapNum;

		public override string ToString()
		{
			return $"{x}, {y} ({mapGroup}:{mapNum})";
		}
	}

	public class GameState
	{
		private GameConnection m_Connection;

		private ushort m_CommandTokenCounter = 0;
		private Dictionary<GameStateConstant, uint> m_CachedConstants = new Dictionary<GameStateConstant, uint>();

		public PlayerPositionData m_PlayerPosition;

		public GameState(GameConnection connection)
		{
			m_Connection = connection;
			m_CommandTokenCounter = connection.ReadU16(connection.Header.RogueOutputBufferAddress);
		}

		static bool aweae = false;

		public void Update()
		{
			// Update player position
			uint saveBlock1PtrAddress = GetConstantValue(GameStateConstant.SaveBlock1Ptr);
			uint saveBlock1Address = m_Connection.ReadU32(saveBlock1PtrAddress);

			m_PlayerPosition.x = m_Connection.ReadS16(saveBlock1Address + 0);
			m_PlayerPosition.y = m_Connection.ReadS16(saveBlock1Address + 2);
			m_PlayerPosition.mapGroup = m_Connection.ReadS8(saveBlock1Address + 4);
			m_PlayerPosition.mapNum = m_Connection.ReadS8(saveBlock1Address + 5);

			Console.WriteLine(m_PlayerPosition);
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
