using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game.Commands.Connection
{
	public class VerifyGameCommand : IConnectionCommandBehaviour
	{
		private bool m_HasConnected = false;

		public void Execute(GameConnection conn)
		{
			if(TryCheckInternal(conn, false) || TryCheckInternal(conn, true))
			{
				m_HasConnected = true;
			}
		}

		public bool HasConnected
		{
			get => m_HasConnected;
		}

		private bool TryCheckInternal(GameConnection conn, bool exOffset)
		{
			//uint offsetHeaderAddr = exOffset ? GameConstants.GFHeaderAddress - 4 : GameConstants.GFHeaderAddress;

			uint offsetHeaderAddr = exOffset ? GameConstants.GFHeaderAddress - 4 : GameConstants.GFHeaderAddress;

			GameConnectionHeader header = new GameConnectionHeader();
			header.GameEdition = exOffset ? GameEdition.EX : GameEdition.Vanilla;

			header.GameVersion = conn.ReadU32(GameConstants.GFHeaderAddress + 0);
			header.GameLanguage = conn.ReadU32(GameConstants.GFHeaderAddress + 4);
			header.GameName = conn.ReadAsciiStringFixed(GameConstants.GFHeaderAddress + 8, 32);

			uint handshakeCode = conn.ReadU32(offsetHeaderAddr + 120);
			if (handshakeCode != 20012)
			{
				Console.WriteLine("Failed to parse valid 'rogue header handshake 1'");
				return false;
			}

			header.RogueHeaderAddress = conn.ReadU32(offsetHeaderAddr + 124);

			handshakeCode = conn.ReadU32(offsetHeaderAddr + 128);
			if (handshakeCode != 30035) 
			{
				Console.WriteLine("Failed to parse valid 'rogue header handshake 2'");
				return false;
			}

			header.RogueInputBufferSize = conn.ReadU32(header.RogueHeaderAddress + 0);
			header.RogueOutputBufferSize = conn.ReadU32(header.RogueHeaderAddress + 4);
			header.RogueInputBufferAddress = conn.ReadU32(header.RogueHeaderAddress + 8);
			header.RogueOutputBufferAddress = conn.ReadU32(header.RogueHeaderAddress + 12);

			conn.Header = header;
			return true;
		}
	}
}
