using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game.Commands.Game
{
	public class EchoGameCommand : IConnectionCommandBehaviour
	{
		public void Execute(GameConnection conn)
		{
			ushort value = (ushort)conn.RNG.Next(ushort.MinValue, ushort.MaxValue);

			conn.WriteU16(conn.GameCommandArgAddress, value);

			uint readAddress = conn.SendReliable(GameCommandCode.Echo);
			ushort readValue = conn.ReadU16(readAddress);

			Console.WriteLine($"Echo '{value}' -> '{readValue}'");
		}
	}
}
