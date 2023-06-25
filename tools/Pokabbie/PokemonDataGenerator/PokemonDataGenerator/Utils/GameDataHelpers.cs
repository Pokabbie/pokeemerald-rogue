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
		private static readonly string c_MovesDefinesPath = "..\\..\\..\\..\\..\\..\\include\\constants\\moves.h";

		private static Dictionary<string, string> s_SpeciesDefines = null;
		private static Dictionary<string, string> s_ItemDefines = null;
		private static Dictionary<string, string> s_TutorMoveDefines = null;
		private static Dictionary<string, string> s_MoveDefines = null;


		private static Dictionary<string, string> s_MoveToTMHMItem = null;
		private static Dictionary<string, string> s_MoveToTutorMove = null;

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

		public static Dictionary<string, string> MoveDefines
		{
			get
			{
				if (s_MoveDefines == null)
				{
					s_MoveDefines = new Dictionary<string, string>();
					ParseFileDefines("#define MOVE_", c_MovesDefinesPath, s_MoveDefines);
				}

				return s_MoveDefines;
			}
		}

		public static Dictionary<string, string> MoveToTMHMItem
		{
			get
			{
				if (s_MoveToTMHMItem == null)
				{
					s_MoveToTMHMItem = new Dictionary<string, string>();

					foreach (var kvp in ItemDefines)
					{
						if (kvp.Key.StartsWith("ITEM_TM") || kvp.Key.StartsWith("ITEM_HM"))
						{
							// These are the "#define ITEM_TM01 123" lines which we want to skip
							if (kvp.Key.Where(c => c == '_').Count() == 1)
								continue;

							string itemName = kvp.Key;
							string moveName = kvp.Key.Substring("ITEM_TM00_".Length);

							if (itemName == "ITEM_TM_CASE")
								continue;

							s_MoveToTMHMItem.Add(moveName, itemName);
						}
					}
				}

				return s_MoveToTMHMItem;
			}
		}

		public static Dictionary<string, string> MoveToTutorMove
		{
			get
			{
				if (s_MoveToTutorMove == null)
				{
					s_MoveToTutorMove = new Dictionary<string, string>();

					foreach (var kvp in TutorMoveDefines)
					{
						if (kvp.Key.StartsWith("TUTOR_MOVE_"))
						{
							string itemName = kvp.Key;
							string moveName = kvp.Key.Substring("TUTOR_MOVE_".Length);

							if (itemName == "TUTOR_MOVE_COUNT")
								continue;

							s_MoveToTutorMove.Add(moveName, itemName);
						}
					}
				}

				return s_MoveToTutorMove;
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
