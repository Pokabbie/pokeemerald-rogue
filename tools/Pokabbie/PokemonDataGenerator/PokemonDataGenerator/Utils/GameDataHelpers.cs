using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Utils
{
	public class GameDataHelpers
	{
		private static readonly string c_SpeciesDefinesPath = "..\\..\\..\\..\\..\\..\\include\\constants\\species.h";
		private static readonly string c_ItemDefinesPath = "..\\..\\..\\..\\..\\..\\include\\constants\\items.h";
		private static readonly string c_PartyMenuDefinesPath = "..\\..\\..\\..\\..\\..\\include\\constants\\party_menu.h";

		private static Dictionary<string, string> s_SpeciesDefines = null;
		private static Dictionary<string, string> s_ItemDefines = null;
		private static Dictionary<string, string> s_TutorMoveDefines = null;

		public static Dictionary<string, string> SpeciesDefines
		{
			get
			{
				if (s_SpeciesDefines == null)
				{
					s_SpeciesDefines = new Dictionary<string, string>();
					ParseFileDefines("#define SPECIES_", c_SpeciesDefinesPath, s_SpeciesDefines);
				}

				return s_SpeciesDefines;
			}
		}

		public static Dictionary<string, string> ItemDefines
		{
			get 
			{
				if (s_ItemDefines == null)
				{
					s_ItemDefines = new Dictionary<string, string>();
					ParseFileDefines("#define ITEM_", c_ItemDefinesPath, s_ItemDefines);
				}

				return s_ItemDefines;
			}
		}

		public static Dictionary<string, string> TutorMoveDefines
		{
			get
			{
				if (s_TutorMoveDefines == null)
				{
					s_TutorMoveDefines = new Dictionary<string, string>();
					ParseFileDefines("#define TUTOR_MOVE_", c_PartyMenuDefinesPath, s_TutorMoveDefines);
				}

				return s_TutorMoveDefines;
			}
		}

		private static void ParseFileDefines(string prefix, string filePath, Dictionary<string, string> dest)
		{
			using (var stream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
			using (var reader = new StreamReader(stream))
			{
				string line;

				while ((line = reader.ReadLine()) != null)
				{
					if (line.StartsWith(prefix))
					{
						var kvp = ParseDefine(line);
						dest.Add(kvp.Key, kvp.Value);
					}
				}
			}
		}

		private static KeyValuePair<string, string> ParseDefine(string line)
		{
			string key = "";
			string value = "";

			line = line.Substring("#define ".Length);
			int splitIdx = line.IndexOf(' ');

			key = line.Substring(0, splitIdx);
			value = line.Substring(key.Length).Trim();

			return new KeyValuePair<string, string>(key, value);
		}

	}
}
