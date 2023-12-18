using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game.Commands.Game
{
	public class GetItemNameCommand : IConnectionCommandBehaviour
	{
		private ushort m_Item;
		private string m_Result;

		public GetItemNameCommand(ushort item)
		{
			m_Item = item;
		}

		public string Result
		{
			get => m_Result;
		}

		public void Execute(GameConnection conn)
		{
			conn.WriteU16(conn.GameCommandArgAddress, m_Item);

			uint readAddress = conn.SendReliable(GameCommandCode.GetItemName);
			m_Result = conn.ReadGameStringFixed(readAddress, GameConstants.ItemNameLength);
		}
	}
}
