using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game.Commands.Game
{
	public class GetSpeciesNameCommand : IConnectionCommandBehaviour
	{
		private ushort m_Species;
		private string m_Result;

		public GetSpeciesNameCommand(ushort species)
		{
			m_Species = species;
		}

		public string Result
		{
			get => m_Result;
		}

		public void Execute(GameConnection conn)
		{
			conn.WriteU16(conn.GameCommandArgAddress, m_Species);

			uint readAddress = conn.SendReliable(GameCommandCode.GetSpeciesName);
			m_Result = conn.ReadGameStringFixed(readAddress, GameConstants.PokemonNameLength + 1);
		}
	}
}
