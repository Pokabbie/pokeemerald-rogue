using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Utils
{
	public static class PokemonMoveHelpers
	{
		public static bool IsMoveUnsupported(string moveString)
		{
			return !GameDataHelpers.MoveDefines.ContainsKey("MOVE_" + FormatKeyword(moveString));
		}

		private static string FormatKeyword(string keyword)
		{
			return keyword.Trim()
				.Replace(".", "")
				.Replace("’", "")
				.Replace("'", "")
				.Replace("%", "")
				.Replace(":", "")
				.Replace(" ", "_")
				.Replace("-", "_")
				.Replace("é", "e")
				.ToUpper();
		}
	}
}
