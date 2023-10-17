using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Utils
{
	public static class GameDataHelpers
	{
		private static string s_RootDirectory = Path.GetFullPath(@"..\\..\\..\\..\\..\\..\\");

		private static Dictionary<string, string> s_SpeciesDefines = null;
		private static Dictionary<string, string> s_ItemDefines = null;
		private static Dictionary<string, string> s_TutorMoveDefines = null;
		private static Dictionary<string, string> s_MoveDefines = null;
		private static Dictionary<string, string> s_AbilitiesDefines = null;
		private static Dictionary<string, string> s_NaturesDefines = null;
		private static Dictionary<string, string> s_TypesDefines = null;


		private static Dictionary<string, string> s_MoveToTMHMItem = null;
		private static Dictionary<string, string> s_MoveToTutorMove = null;

		public static string RootDirectory
		{
			get => s_RootDirectory;
		}

		private static string PokemonDefinesPath
		{
			get => Path.Combine(RootDirectory, "include\\constants\\pokemon.h");
		}

		private static string SpeciesDefinesPath
		{
			get => Path.Combine(RootDirectory, "include\\constants\\species.h");
		}

		private static string ItemDefinesPath
		{
			get => Path.Combine(RootDirectory, "include\\constants\\items.h");
		}

		private static string PartyMenuDefinesPath
		{
			get => Path.Combine(RootDirectory, "include\\constants\\party_menu.h");
		}

		private static string MovesDefinesPath
		{
			get => Path.Combine(RootDirectory, "include\\constants\\moves.h");
		}

		private static string AbilitiesDefinesPath
		{
			get => Path.Combine(RootDirectory, "include\\constants\\abilities.h");
		}

		// hacky based on current file names
		public static bool IsVanillaVersion
		{
			get => s_RootDirectory.EndsWith("pokeemerald-rogue\\", StringComparison.CurrentCultureIgnoreCase);
			set 
			{
				bool isVanilla = IsVanillaVersion;
				if (isVanilla != value)
				{
					s_RootDirectory = Path.Combine(s_RootDirectory, "..\\pokeemerald-rogue-ee\\");
					s_RootDirectory = Path.GetFullPath(s_RootDirectory);
				}
			}
		}

		public static Dictionary<string, string> SpeciesDefines
		{
			get
			{
				if (s_SpeciesDefines == null)
				{
					s_SpeciesDefines = new Dictionary<string, string>();
					ParseFileDefines("#define SPECIES_", SpeciesDefinesPath, s_SpeciesDefines);
				}

				return s_SpeciesDefines;
			}
		}

		public static bool IsUniqueSpeciesDefine(string define)
		{
			if (define == "SPECIES_NONE")
				return false;
			if (define == "SPECIES_COUNT")
				return false;
			if (define == "SPECIES_EGG")
				return false;
			if (define.StartsWith("SPECIES_OLD_"))
				return false;

			return true;
		}

		public static Dictionary<string, string> ItemDefines
		{
			get
			{
				if (s_ItemDefines == null)
				{
					s_ItemDefines = new Dictionary<string, string>();
					ParseFileDefines("#define ITEM_", ItemDefinesPath, s_ItemDefines);
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
					ParseFileDefines("#define TUTOR_MOVE_", PartyMenuDefinesPath, s_TutorMoveDefines);
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
					ParseFileDefines("#define MOVE_", MovesDefinesPath, s_MoveDefines);
				}

				return s_MoveDefines;
			}
		}

		public static Dictionary<string, string> AbilityDefines
		{
			get
			{
				if (s_AbilitiesDefines == null)
				{
					s_AbilitiesDefines = new Dictionary<string, string>();
					ParseFileDefines("#define ABILITY_", AbilitiesDefinesPath, s_AbilitiesDefines);
				}

				return s_AbilitiesDefines;
			}
		}

		public static Dictionary<string, string> NatureDefines
		{
			get
			{
				if (s_NaturesDefines == null)
				{
					s_NaturesDefines = new Dictionary<string, string>();
					ParseFileDefines("#define NATURE_", PokemonDefinesPath, s_NaturesDefines);
				}

				return s_NaturesDefines;
			}
		}

		public static Dictionary<string, string> TypesDefines
		{
			get
			{
				if (s_TypesDefines == null)
				{
					s_TypesDefines = new Dictionary<string, string>();
					ParseFileDefines("#define TYPE_", PokemonDefinesPath, s_TypesDefines);
				}

				return s_TypesDefines;
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

		private static string GetPokemonPalettePath(string mon, string pal)
		{
			string[] parts = mon.Split('_');

			if (parts.Length > 1)
			{
				string baseSpecies = parts[0];
				string form = string.Join("_", parts.Skip(1).ToArray());

				return Path.Combine(RootDirectory, $"graphics\\pokemon\\{baseSpecies}\\{form}\\{pal}.pal");
			}
			else
			{
				return Path.Combine(RootDirectory, $"graphics\\pokemon\\{mon}\\{pal}.pal");
			}
		}

		public static string GetPokemonNormalPalettePath(string mon)
		{
			return GetPokemonPalettePath(mon, "normal");
		}

		public static string GetPokemonShinyPalettePath(string mon)
		{
			return GetPokemonPalettePath(mon, "shiny");
		}

		public static string FormatKeyword(string keyword)
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
