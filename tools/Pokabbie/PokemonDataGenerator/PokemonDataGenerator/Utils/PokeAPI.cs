using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Utils
{
	public static class PokeAPI
	{
		private static readonly string s_PokeApiAddress = "https://pokeapi.co/api/v2/";
		private static readonly string s_ShowdownApiAddress = "https://play.pokemonshowdown.com/data/sets/";
		private static Dictionary<string, string> s_PokeApiBaseAddresses = null;

		private static Dictionary<string, string> s_CachedPokedexURIs = null;
		private static Dictionary<string, string> s_CachedPokemonURIs = null;
		private static Dictionary<string, string> s_CachedPokemonSpeciesURIs = null;

		static PokeAPI()
		{
			s_PokeApiBaseAddresses = new Dictionary<string, string>();
			JObject result = ContentCache.GetJsonContent(s_PokeApiAddress);

			foreach(var kvp in result)
			{
				s_PokeApiBaseAddresses[kvp.Key] = kvp.Value.Value<string>();
			}
		}

		private static JArray GetArrayInternal(string uri)
		{
			JObject result = ContentCache.GetJsonContent(uri);
			JArray output = new JArray();

			while(true)
			{
				foreach (var token in result["results"])
					output.Add(token);

				if(result.ContainsKey("next") && result["next"].Value<string>() != null)
					result = ContentCache.GetJsonContent(result["next"].Value<string>());
				else
					break;
			}

			return output;
		}

		private static Dictionary<string, string> GetResultsAsTableInternal(string uri)
		{
			Dictionary<string, string> output = new Dictionary<string, string>();
			JArray result = GetArrayInternal(uri);

			foreach(var entry in result)
			{
				output[entry["name"].Value<string>()] = entry["url"].Value<string>();
			}

			return output;
		}

		public static Dictionary<string, string> GetPokedexURIs()
		{
			if(s_CachedPokedexURIs == null)
			{
				s_CachedPokedexURIs = GetResultsAsTableInternal(s_PokeApiBaseAddresses["pokedex"]);
			}

			return s_CachedPokedexURIs;
		}

		public static Dictionary<string, string> GetPokemonURIs()
		{
			if (s_CachedPokemonURIs == null)
			{
				Dictionary<string, string> unformatedTable = GetResultsAsTableInternal(s_PokeApiBaseAddresses["pokemon"]);
				Dictionary<string, string> outputTable = new Dictionary<string, string>();

				foreach (var kvp in unformatedTable)
				{
					string speciesName = ApiNameToSpeciesName(kvp.Key);
					outputTable[speciesName] = kvp.Value;
				}

				s_CachedPokemonURIs = outputTable;
			}

			return s_CachedPokemonURIs;
		}

		public static Dictionary<string, string> GetPokemonSpeciesURIs()
		{
			if (s_CachedPokemonSpeciesURIs == null)
			{
				Dictionary<string, string> unformatedTable = GetResultsAsTableInternal(s_PokeApiBaseAddresses["pokemon-species"]);
				Dictionary<string, string> outputTable = new Dictionary<string, string>();

				foreach (var kvp in unformatedTable)
				{
					string speciesName = ApiNameToSpeciesName(kvp.Key);
					outputTable[speciesName] = kvp.Value;
				}

				s_CachedPokemonSpeciesURIs = outputTable;
			}

			return s_CachedPokemonSpeciesURIs;
		}

		public static JObject GetPokemonEntry(string speciesName)
		{
			string apiName = SpeciesNameToApiName(speciesName);
			return ContentCache.GetJsonContent(s_PokeApiAddress + "/pokemon/" + apiName);
		}

		public static JObject GetPokemonSpeciesEntry(string speciesName)
		{
			string overridePath = $"res://ManualPokeAPI/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json";

			if(ContentCache.ExistsInCache(overridePath))
			{
				return ContentCache.GetJsonContent(overridePath);
			}

			var uriLookup = GetPokemonSpeciesURIs();
			RedirectSpeciesLookupName(speciesName, out string lookupSpecies, out string lookupVariant);

			JObject speciesEntry = ContentCache.GetJsonContent(uriLookup[lookupSpecies]);

			foreach(var variant in speciesEntry["varieties"])
			{
				var variantEntry = variant["pokemon"];
				if(variantEntry["name"].Value<string>() == lookupVariant)
				{
					return ContentCache.GetJsonContent(variantEntry["url"].Value<string>());
				}
			}

			if (speciesName == lookupSpecies)
			{
				// Failed to find the form so just assume we mean the default
				foreach (var variant in speciesEntry["varieties"])
				{
					var variantEntry = variant["pokemon"];
					if (variant["is_default"].Value<bool>())
					{
						return ContentCache.GetJsonContent(variantEntry["url"].Value<string>());
					}
				}
			}

			Console.WriteLine($"Unable to find matching variety for '{speciesName}' -> '{lookupSpecies}':'{lookupVariant}'\nAvailable:");
			foreach (var variant in speciesEntry["varieties"])
			{
				var variantEntry = variant["pokemon"];
				Console.WriteLine("\t" + variantEntry["name"].Value<string>());
			}

			throw new InvalidOperationException("API species error");
		}

		private static string SpeciesNameToApiName(string speciesName)
		{
			if (speciesName.StartsWith("SPECIES_"))
				speciesName = speciesName.Substring("SPECIES_".Length);

			return speciesName.ToLower().Replace("_", "-");
		}

		private static string ApiNameToSpeciesName(string apiName)
		{
			return "SPECIES_" + GameDataHelpers.FormatKeyword(apiName);
		}

		private static void RedirectSpeciesLookupName(string speciesName, out string apiSpecies, out string variantName)
		{
			if (GameDataHelpers.IsVanillaVersion)
			{
				if (speciesName.StartsWith("SPECIES_UNOWN"))
					speciesName = "SPECIES_UNOWN";
			}

			apiSpecies = speciesName;
			variantName = SpeciesNameToApiName(speciesName);
			
			if(speciesName.StartsWith("SPECIES_DEOXYS"))
			{
				apiSpecies = "SPECIES_DEOXYS";

				if (speciesName == "SPECIES_DEOXYS")
					variantName = "deoxys-normal";
			}


			if (!GameDataHelpers.IsVanillaVersion)
			{
				if (speciesName.EndsWith("_ALOLAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_ALOLAN".Length);
					variantName = variantName.Replace("-alolan", "-alola");
				}
				else if (speciesName.EndsWith("_GALARIAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_GALARIAN".Length);
					variantName = variantName.Replace("-galarian", "-galar");
				}
				else if (speciesName.EndsWith("_HISUIAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_HISUIAN".Length);
					variantName = variantName.Replace("-hisuian", "-hisui");
				}
				else if (speciesName.EndsWith("_ORIGIN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_ORIGIN".Length);
				}
				else if (speciesName.EndsWith("_THERIAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_THERIAN".Length);
				}
				else if (speciesName.StartsWith("SPECIES_PIKACHU_"))
				{
					apiSpecies = "SPECIES_PIKACHU";
				}
				else if (speciesName.StartsWith("SPECIES_ROTOM_"))
				{
					apiSpecies = "SPECIES_ROTOM";
				}
				else if (speciesName.StartsWith("SPECIES_SHAYMIN_"))
				{
					apiSpecies = "SPECIES_SHAYMIN";
				}
				else if (speciesName.StartsWith("SPECIES_BASCULIN_"))
				{
					apiSpecies = "SPECIES_BASCULIN";
				}
				else if (speciesName.StartsWith("SPECIES_GRENINJA_"))
				{
					apiSpecies = "SPECIES_GRENINJA";
				}
				else if (speciesName.StartsWith("SPECIES_ZYGARDE_"))
				{
					apiSpecies = "SPECIES_ZYGARDE";
				}
				else if (speciesName.StartsWith("SPECIES_ORICORIO_"))
				{
					apiSpecies = "SPECIES_ORICORIO";
				}
				else if (speciesName.StartsWith("SPECIES_LYCANROC_"))
				{
					apiSpecies = "SPECIES_LYCANROC";
				}
				else if (speciesName.StartsWith("SPECIES_CALYREX_"))
				{
					apiSpecies = "SPECIES_CALYREX";
					variantName = variantName.Replace("-rider", "");
				}

				switch (speciesName)
				{
					case "SPECIES_DARMANITAN_GALARIAN":
						apiSpecies = "SPECIES_DARMANITAN";
						variantName = "darmanitan-galar-standard";
						break;

					case "SPECIES_PIKACHU_PH_D":
						apiSpecies = "SPECIES_PIKACHU";
						variantName = "pikachu-phd";
						break;

					case "SPECIES_PICHU_SPIKY_EARED":
						apiSpecies = "SPECIES_PICHU";
						variantName = "pichu";
						break;

						//case "SPECIES_WORMADAM":
						//	apiSpecies = "SPECIES_WORMADAM";
						//	variantName = "wormadam-plant";
						//	break;
						//
					case "SPECIES_WORMADAM_SANDY_CLOAK":
						apiSpecies = "SPECIES_WORMADAM";
						variantName = "wormadam-sandy";
						break;
					
					case "SPECIES_WORMADAM_TRASH_CLOAK":
						apiSpecies = "SPECIES_WORMADAM";
						variantName = "wormadam-trash";
						break;

					case "SPECIES_KYUREM_WHITE":
					case "SPECIES_KYUREM_BLACK":
						apiSpecies = "SPECIES_KYUREM";
						break;

					case "SPECIES_MEOWSTIC_FEMALE":
						apiSpecies = "SPECIES_MEOWSTIC";
						break;

					case "SPECIES_HOOPA_UNBOUND":
						apiSpecies = "SPECIES_HOOPA";
						break;

					case "SPECIES_ROCKRUFF_OWN_TEMPO":
						apiSpecies = "SPECIES_ROCKRUFF";
						break;

					case "SPECIES_NECROZMA_DUSK_MANE":
						apiSpecies = "SPECIES_NECROZMA";
						variantName = "necrozma-dusk";
						break;
					case "SPECIES_NECROZMA_DAWN_WINGS":
						apiSpecies = "SPECIES_NECROZMA";
						variantName = "necrozma-dawn";
						break;
					case "SPECIES_NECROZMA_ULTRA":
						apiSpecies = "SPECIES_NECROZMA";
						variantName = "necrozma-ultra";
						break;


					case "SPECIES_TOXTRICITY_LOW_KEY":
						apiSpecies = "SPECIES_TOXTRICITY";
						break;

					case "SPECIES_INDEEDEE_FEMALE":
						apiSpecies = "SPECIES_INDEEDEE";
						break;

					case "SPECIES_ZACIAN_CROWNED_SWORD":
						apiSpecies = "SPECIES_ZACIAN";
						variantName = "zacian-crowned";
						break;
					case "SPECIES_ZAMAZENTA_CROWNED_SHIELD":
						apiSpecies = "SPECIES_ZAMAZENTA";
						variantName = "zamazenta-crowned";
						break;

					case "SPECIES_URSHIFU_RAPID_STRIKE_STYLE":
						apiSpecies = "SPECIES_URSHIFU";
						variantName = "urshifu-rapid-strike";
						break;

					case "SPECIES_BASCULEGION_FEMALE":
						apiSpecies = "SPECIES_BASCULEGION";
						break;

						//
						//case "SPECIES_GIRATINA":
						//	apiSpecies = "SPECIES_GIRATINA";
						//	variantName = "giratina-altered";
						//	break;
						//
						//case "SPECIES_SHAYMIN":
						//	apiSpecies = "SPECIES_SHAYMIN";
						//	variantName = "shaymin-land";
						//	break;
						//
						//case "SPECIES_BASCULIN":
						//	apiSpecies = "SPECIES_BASCULIN";
						//	variantName = "basculin-red-striped";
						//	break;
						//
						//case "SPECIES_DARMANITAN":
						//	apiSpecies = "SPECIES_DARMANITAN";
						//	variantName = "darmanitan-standard";
						//	break;
						//
						//case "SPECIES_TORNADUS":
						//	apiSpecies = "SPECIES_TORNADUS";
						//	variantName = "tornadus-incarnate";
						//	break;
						//case "SPECIES_THUNDURUS":
						//	apiSpecies = "SPECIES_THUNDURUS";
						//	variantName = "thundurus-incarnate";
						//	break;
						//case "SPECIES_LANDORUS":
						//	apiSpecies = "SPECIES_LANDORUS";
						//	variantName = "landorus-incarnate";
						//	break;
				}
			}
		}

		public static JObject GetPokemonSpeciesCompetitiveSets(string speciesName)
		{
			List<string> lookupGens = new List<string>();

			if(GameDataHelpers.IsVanillaVersion)
			{
				lookupGens.Add("gen3");
			}
			else
			{
				lookupGens.Add("gen6");
				lookupGens.Add("gen7");
				lookupGens.Add("gen8");


				// For now, we're only accepting hisui sets
				bool allowGen9 = false;

				switch (speciesName)
				{
					case "SPECIES_WYRDEER":
					case "SPECIES_KLEAVOR":
					case "SPECIES_URSALUNA":
					case "SPECIES_BASCULEGION":
					case "SPECIES_SNEASLER":
					case "SPECIES_OVERQWIL":
					case "SPECIES_ENAMORUS":
					case "SPECIES_DIALGA_ORIGIN":
					case "SPECIES_PALKIA_ORIGIN":
					case "SPECIES_BASCULIN_WHITE_STRIPED":
					case "SPECIES_ENAMORUS_THERIAN":
					case "SPECIES_BASCULEGION_F":
						allowGen9 = true;
						break; // accept these

					default:
						if (speciesName.EndsWith("_HISUIAN", StringComparison.CurrentCultureIgnoreCase))
							allowGen9 = true;
						break;
				};

				if(allowGen9)
					lookupGens.Add("gen9");
			}

			JObject output = new JObject();

			foreach(var lookupGen in lookupGens)
			{
				var compSets = ContentCache.GetJsonContent(s_ShowdownApiAddress + lookupGen + ".json");

				foreach (var compTierKvp in compSets)
				{
					if (compTierKvp.Key.EndsWith("hackmons", StringComparison.CurrentCultureIgnoreCase) ||
						compTierKvp.Key.EndsWith("cap", StringComparison.CurrentCultureIgnoreCase) ||
						compTierKvp.Key.EndsWith("anyability", StringComparison.CurrentCultureIgnoreCase))
						continue;

					JArray tierOutput = new JArray();

					foreach (var upperGroupingKvp in compTierKvp.Value.Value<JObject>())
					{
						foreach (var speciesSetGroupKvp in upperGroupingKvp.Value.Value<JObject>())
						{
							// Verify that the mons we're examining are correctly handled or have an equivilant species in game
							if(!compTierKvp.Key.StartsWith("gen9"))
								VerifyCompetitiveSpecies(speciesSetGroupKvp.Key);

							string currentSpeciesName = CompetitiveApiNameToSpeciesName(speciesSetGroupKvp.Key);

							if (currentSpeciesName != speciesName)
								continue;

							foreach (var currentSetKvp in speciesSetGroupKvp.Value.Value<JObject>())
							{
								var currentSet = currentSetKvp.Value.Value<JObject>();

								tierOutput.Add(currentSet);
							}
						}
					}

					output[compTierKvp.Key] = tierOutput;
				}
			}

			return output;
		}

		private static void VerifyCompetitiveSpecies(string apiName)
		{
			// Dud method to verify that we're properly supporting all of the cases we need to be
			string speciesName = CompetitiveApiNameToSpeciesName(apiName);

			if (GameDataHelpers.IsVanillaVersion)
			{
				switch (apiName)
				{
					case "Deoxys-Speed":
					case "Deoxys-Defense":
					case "Deoxys-Attack":
						return;
				}
			}
			else
			{
				switch (apiName)
				{
					// Ignored
					case "Eevee-Starter":
						return;
				}
			}

			if (!GameDataHelpers.SpeciesDefines.ContainsKey(speciesName))
				throw new NotImplementedException();
		}

		private static string CompetitiveApiNameToSpeciesName(string apiName)
		{
			if (apiName.EndsWith("-mega", StringComparison.CurrentCultureIgnoreCase))
			{
				apiName = apiName.Substring(0, apiName.Length - "-mega".Length);
			}
			else if (apiName.EndsWith("-mega-x", StringComparison.CurrentCultureIgnoreCase) || apiName.EndsWith("-mega-y", StringComparison.CurrentCultureIgnoreCase))
			{
				apiName = apiName.Substring(0, apiName.Length - "-mega-x".Length);
			}
			else if (apiName.EndsWith("-primal", StringComparison.CurrentCultureIgnoreCase))
			{
				apiName = apiName.Substring(0, apiName.Length - "-primal".Length);
			}

			if (!GameDataHelpers.IsVanillaVersion)
			{
				if(apiName.EndsWith("-Alola"))
					apiName = apiName.Replace("-Alola", "-Alolan");

				if (apiName.EndsWith("-Galar"))
					apiName = apiName.Replace("-Galar", "-Galarian");

				switch (apiName)
				{
					case "Darmanitan-Zen":
						apiName = "Darmanitan-Zen-mode";
						break;
					case "Darmanitan-Galar-Zen":
						apiName = "Darmanitan-Zen-mode-Galarian";
						break;

					case "Wormadam-Sandy":
						apiName = "Wormadam-Sandy-Cloak";
						break;
					case "Wormadam-Trash":
						apiName = "Wormadam-Trash-Cloak";
						break;

					case "Meowstic-F":
						apiName = "Meowstic-Female";
						break;

					case "Indeedee-F":
						apiName = "Indeedee-Female";
						break;

					case "Calyrex-Ice":
						apiName = "Calyrex-Ice-Rider";
						break;

					case "Calyrex-Shadow":
						apiName = "Calyrex-Shadow-Rider";
						break;

					case "Urshifu-Rapid-Strike":
						apiName = "Urshifu-Rapid-Strike-Style";
						break;
				}
			}


			return "SPECIES_" + GameDataHelpers.FormatKeyword(apiName);
		}
	}
}
