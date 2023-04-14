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
		private static Dictionary<string, JObject> s_AlteredJsons = new Dictionary<string, JObject>();

		public static void Setup()
		{
			Console.WriteLine($"Loading project files");
			s_EventScriptContent = File.ReadAllLines(c_EventScriptsFile).ToList();
			s_MapGroupContent = LoadJson(c_MapGroupsFile);
			s_LayoutsContent = LoadJson(c_LayoutsFile);
			s_EncountersContent = LoadJson(c_EncountersFile);
		}

		public static void Shutdown()
		{
			Console.WriteLine($"Exporting project files");
			JsonSerializerSettings settings = new JsonSerializerSettings
			{
				Formatting = Formatting.Indented
			};

			File.WriteAllLines(c_EventScriptsFile, s_EventScriptContent);

			foreach(var kvp in s_AlteredJsons)
			{
				// If still exists
				if(File.Exists(kvp.Key))
				{
					Console.WriteLine($"Updating '{kvp.Key}'");
					File.WriteAllText(kvp.Key, JsonConvert.SerializeObject(kvp.Value, settings));
				}
			}
		}

		private static JObject LoadJson(string path)
		{
			string key = path.Trim().ToLower().Replace("\\", "/");

			if (s_AlteredJsons.TryGetValue(key, out JObject obj))
				return obj;

			obj = JsonConvert.DeserializeObject<JObject>(File.ReadAllText(path));
			s_AlteredJsons.Add(key, obj);
			return obj;
		}

		public static void DeleteSingleMap(string map)
		{
			if(VerifyMapPathsExist(map))
			{
				Console.WriteLine($"Deleting '{map}'..");

				string mapFilePath = Path.Combine(c_MapsDirectory, map, "map.json");
				JObject mapFile = LoadJson(mapFilePath);
				string mapId = mapFile["id"].ToString();

				RemoveWarpEventsFor(mapId);
								
				RemoveEventScriptLine($".include \"data/maps/{map}/scripts.inc\"");
				RemoveMapLayout(map + "_Layout");
				RemoveMapGroupEntry(map);
				RemoveEncounters(mapId);

				string mapDir = Path.Combine(c_MapsDirectory, map);
				if (Directory.Exists(mapDir))
					Directory.Delete(mapDir, true);

				string layoutDir = Path.Combine(c_LayoutsDirectory, map);
				if (Directory.Exists(layoutDir))
					Directory.Delete(layoutDir, true);
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
				"BattlePyramid",
				"AlteringCave",
				"AquaHideout_",
				"ArtisanCave_",
				"BattleColosseum_",
				"CaveOfOrigin_",
				"ContestHall",
				"DesertUnderpass",
				"DewfordTown",
				"EverGrandeCity",
				"FallarborTown",
				"FieryPath",
				"FortreeCity",
				"GraniteCave_",
				"JaggedPass",
				"LavaridgeTown",
				"LilycoveCity",
				"LittlerootTown",
				"MagmaHideout_",
				"MauvilleCity",
				"MeteorFalls_",
				"MirageTower_",
				"MossdeepCity",
				"MtChimney",
				"MtPyre_",
				"NavelRock_Down",
				"NavelRock_Up",
				"NewMauville_",
				"OldaleTown",
				"PacifidlogTown",
				"Petalburg",
				"RecordCorner",
				"Route",
				"RustboroCity",
				"RusturfTunnel",
				"ScorchedSlab",
				"SeafloorCavern_",
				"SealedChamber_",
				"SecretBase_",
				"ShoalCave_",
				"SlateportCity",
				"SootopolisCity",
				"SSTidal",
				"TradeCenter",
				"TrainerHill",
				"Underwater",
				"UnionRoom",
				"UnusedContestHall",
				"VerdanturfTown",
				"VictoryRoad",

				"DesertRuins",
				"IslandCave",
				"AncientTomb",
				"BirthIsland_",
				"FarawayIsland_",
				"MarineCave_",
				"NavelRock_",
				"SkyPillar_",
				"SouthernIsland_",
				"TerraCave_",
			};

			bool ShouldDelete(string mapName)
			{
				if(mapPatternsToDelete.Where((p) => mapName.StartsWith(p, StringComparison.CurrentCultureIgnoreCase)).Any())
				{
					return !mapName.Equals("Route121_SafariZoneEntrance", StringComparison.CurrentCultureIgnoreCase);
				}

				return false;
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
			return true;// Directory.Exists(Path.Combine(c_MapsDirectory, map));// && Directory.Exists(Path.Combine(c_LayoutsDirectory, map));
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
			bool success = false;

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
							success = true;
						}
					}
				}
			}

			if (success)
				return true;

			Console.Error.WriteLine($"Error: \tFailed to locate map group entry '{map}'");
			return false;
		}

		private static bool RemoveEncounters(string mapId)
		{
			var encounterGroups = (JArray)s_EncountersContent["wild_encounter_groups"];

			for (int i = 0; i < encounterGroups.Count; ++i)
			{
				var encounters = (JArray)encounterGroups[i]["encounters"];

				for (int j = 0; j < encounters.Count; ++j)
				{
					var encounterEntry = (JObject)encounters[j];

					if (encounterEntry.ContainsKey("map") && encounterEntry["map"].ToString().Equals(mapId, StringComparison.CurrentCultureIgnoreCase))
					{
						encounters.RemoveAt(j);
						return true;
					}
				}
			}

			Console.Error.WriteLine($"Error: \tFailed to locate wild encounters '{mapId}'");
			return false;
		}

		private static void RemoveWarpEventsFor(string mapId)
		{
			foreach (var otherMapFilePath in Directory.EnumerateDirectories(c_MapsDirectory, "map.json", SearchOption.AllDirectories))
			{
				JObject otherMapFile = LoadJson(otherMapFilePath);

				if(otherMapFile["map"].ToString() != mapId)
				{
					var conns = (JArray)otherMapFile["connections"];
					var warps = (JArray)otherMapFile["warp_events"];

					for(int i = 0; i < conns.Count;)
					{
						if (conns[i]["map"].ToString() == mapId)
							conns.RemoveAt(i);
						else
							++i;
					}
					
					for (int i = 0; i < warps.Count;)
					{
						if (warps[i]["dest_map"].ToString() == mapId)
							warps.RemoveAt(i);
						else
							++i;
					}
				}
			}
		}
	}
}
