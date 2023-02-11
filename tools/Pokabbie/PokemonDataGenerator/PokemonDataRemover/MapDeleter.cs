using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataRemover
{
	public static class MapDeleter
	{
		public static readonly string c_MapsDirectory = Path.Combine(Program.c_RootDirectory, "data\\maps");
		public static readonly string c_LayoutsDirectory = Path.Combine(Program.c_RootDirectory, "data\\layouts");

		public static readonly string c_EventScriptsFile = Path.Combine(Program.c_RootDirectory, "data\\event_scripts.s");
		public static readonly string c_MapGroupsFile = Path.Combine(c_MapsDirectory, "map_groups.json");
		public static readonly string c_LayoutsFile = Path.Combine(c_LayoutsDirectory, "layouts.json");
		public static readonly string c_EncountersFile = Path.Combine(Program.c_RootDirectory, "src\\data\\wild_encounters.json");

		private static List<string> s_EventScriptContent = null;
		private static JObject s_MapGroupContent = null;
		private static JObject s_LayoutsContent = null;
		private static JObject s_EncountersContent = null;

		public static void Setup()
		{
			Console.WriteLine($"Loading project files");
			s_EventScriptContent = File.ReadAllLines(c_EventScriptsFile).ToList();
			s_MapGroupContent = JsonConvert.DeserializeObject<JObject>(File.ReadAllText(c_MapGroupsFile));
			s_LayoutsContent = JsonConvert.DeserializeObject<JObject>(File.ReadAllText(c_LayoutsFile));
			s_EncountersContent = JsonConvert.DeserializeObject<JObject>(File.ReadAllText(c_EncountersFile));
		}

		public static void Shutdown()
		{
			Console.WriteLine($"Exporting project files");
			JsonSerializerSettings settings = new JsonSerializerSettings
			{
				Formatting = Formatting.Indented
			};

			File.WriteAllLines(c_EventScriptsFile, s_EventScriptContent);
			File.WriteAllText(c_MapGroupsFile, JsonConvert.SerializeObject(s_MapGroupContent, settings));
			File.WriteAllText(c_LayoutsFile, JsonConvert.SerializeObject(s_LayoutsContent, settings));
			File.WriteAllText(c_EncountersFile, JsonConvert.SerializeObject(s_EncountersContent, settings));

		}

		public static void DeleteSingleMap(string map)
		{
			if(VerifyMapPathsExist(map))
			{
				Console.WriteLine($"Deleting '{map}'..");

				string mapDir = Path.Combine(c_MapsDirectory, map);
				if (Directory.Exists(mapDir))
					Directory.Delete(mapDir, true);

				string layoutDir = Path.Combine(c_LayoutsDirectory, map);
				if(Directory.Exists(layoutDir))
					Directory.Delete(layoutDir, true);
				
				RemoveEventScriptLine($".include \"data/maps/{map}/scripts.inc\"");
				RemoveMapLayout(map + "_Layout");
				RemoveMapGroupEntry(map);
				RemoveEncounters(map);
			}
			else
			{
				Console.Error.WriteLine($"Error: Cannot verify map paths for '{map}'");
			}
		}

		public static void DeleteNonRogueMaps()
		{
			// Safe to manually opt in patterns one at a time
			string[] mapPatternsToDelete = new string[] 
			{
				"AbandonedShip_",
				"BattleFrontier_",
				"BattlePyramid"
			};

			bool ShouldDelete(string mapName)
			{
				return mapPatternsToDelete.Where((p) => mapName.StartsWith(p, StringComparison.CurrentCultureIgnoreCase)).Any();
			}

			foreach(var mapPath in Directory.EnumerateDirectories(c_MapsDirectory))
			{
				string mapName = Path.GetFileName(mapPath);

				if(ShouldDelete(mapName))
				{
					DeleteSingleMap(mapName);
				}
				else
				{
					Console.WriteLine($"Skipping '{mapName}'");
				}
			}
		}

		private static bool VerifyMapPathsExist(string map)
		{
			return Directory.Exists(Path.Combine(c_MapsDirectory, map));// && Directory.Exists(Path.Combine(c_LayoutsDirectory, map));
		}

		private static bool RemoveEventScriptLine(string line)
		{
			for (int i = 0; i < s_EventScriptContent.Count; ++i)
			{
				if (s_EventScriptContent[i].Trim().Equals(line, StringComparison.CurrentCultureIgnoreCase))
				{
					s_EventScriptContent.RemoveAt(i);

					// Remove any empty lines after
					if(string.IsNullOrWhiteSpace(s_EventScriptContent[i]))
						s_EventScriptContent.RemoveAt(i);

					return true;
				}
			}

			Console.Error.WriteLine($"Error: \tFailed to locate script line '{line}'");
			return false;
		}

		private static bool RemoveMapLayout(string layout)
		{
			var layoutsArray = (JArray)s_LayoutsContent["layouts"];

			for (int i = 0; i < layoutsArray.Count; ++i)
			{
				var layoutObj = (JObject)layoutsArray[i];

				if (layoutObj["name"].ToString().Equals(layout, StringComparison.CurrentCultureIgnoreCase))
				{
					layoutsArray.RemoveAt(i);
					return true;
				}
			}

			Console.Error.WriteLine($"Error: \tFailed to locate map layout '{layout}'");
			return false;
		}

		private static bool RemoveMapGroupEntry(string map)
		{
			foreach(var kvp in s_MapGroupContent)
			{
				var entryArray = kvp.Value as JArray;

				if(entryArray != null)
				{
					for(int i = 0; i < entryArray.Count; ++i)
					{
						if (entryArray[i].ToString().Equals(map, StringComparison.CurrentCultureIgnoreCase))
						{
							entryArray.RemoveAt(i);
							return true;
						}
					}
				}
			}

			Console.Error.WriteLine($"Error: \tFailed to locate map group entry '{map}'");
			return false;
		}

		private static bool RemoveEncounters(string encounterLabel)
		{
			var encounterGroups = (JArray)s_EncountersContent["wild_encounter_groups"];

			for (int i = 0; i < encounterGroups.Count; ++i)
			{
				var encounters = (JArray)encounterGroups[i]["encounters"];

				for (int j = 0; j < encounters.Count; ++j)
				{
					if (encounters[j]["base_label"].ToString().Equals(encounterLabel, StringComparison.CurrentCultureIgnoreCase))
					{
						encounters.RemoveAt(j);
						return true;
					}
				}
			}

			Console.Error.WriteLine($"Error: \tFailed to locate wild encounters '{encounterLabel}'");
			return false;
		}
	}
}
