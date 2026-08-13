using Newtonsoft.Json.Converters;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;
using System.IO;
using static PokemonDataGenerator.Pokedex.PokemonProfileGenerator;
using PokemonDataGenerator.Utils;
using System.Windows.Media.Animation;

namespace PokemonDataGenerator
{
    internal class TestMain
    {
        public static void Run()
        {
            GameDataToJsonRun();
        }

        private static void GameDataToJsonRun()
        {
            string pokemonProfilesDir = Path.Combine(GameDataHelpers.PokemonProfilesDirectory);
            string exportedStatsPath = Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\rogue_exported_stats.json");

            JArray exportedStatsArr = (JArray)JObject.Parse(File.ReadAllText(exportedStatsPath))["base"];

            foreach (var profileFile in Directory.EnumerateFiles(pokemonProfilesDir, "*_profile.json", SearchOption.AllDirectories))
            {
                string srcTest = File.ReadAllText(profileFile);
                JObject profileObj = JObject.Parse(srcTest);

                if (!profileObj.ContainsKey("PerSpecies") || true)
                {
                    JObject perSpeciesOutput = new JObject();

                    JArray speciesArray = (JArray)profileObj["Species"];

                    foreach(string species in speciesArray)
                    {
                        JObject speciesOutput = new JObject();

                        int speciesIndex = GameDataHelpers.GetSpeciesNum(species);
                        JObject exportedSpeciesStats = (JObject)exportedStatsArr[speciesIndex];

                        // Base stats
                        {
                            JObject baseStatsOutput = new JObject();

                            baseStatsOutput["HP"] = exportedSpeciesStats["baseHP"];
                            baseStatsOutput["Attack"] = exportedSpeciesStats["baseAttack"];
                            baseStatsOutput["Defense"] = exportedSpeciesStats["baseDefense"];
                            baseStatsOutput["Speed"] = exportedSpeciesStats["baseSpeed"];
                            baseStatsOutput["SpAttack"] = exportedSpeciesStats["baseSpAttack"];
                            baseStatsOutput["SpDefense"] = exportedSpeciesStats["baseSpDefense"];

                            speciesOutput["BaseStats"] = baseStatsOutput;
                        }

                        // Types
                        {
                            JArray typesOutput = new JArray();

                            foreach(int typeIndex in exportedSpeciesStats["types"])
                            {
                                typesOutput.Add(GameDataHelpers.FindKeyFromConstant(GameDataHelpers.TypesDefines, typeIndex));
                            }

                            speciesOutput["Types"] = typesOutput;
                        }

                        // Abilities
                        {
                            JArray abilitiesOutput = new JArray();

                            foreach (int abilityIndex in exportedSpeciesStats["abilities"])
                            {
                                abilitiesOutput.Add(GameDataHelpers.FindKeyFromConstant(GameDataHelpers.AbilityDefines, abilityIndex));
                            }

                            speciesOutput["Abilities"] = abilitiesOutput;
                        }


                        perSpeciesOutput[species] = speciesOutput;
                    }

                    profileObj["PerSpecies"] = perSpeciesOutput;
                }

                string outputTest = profileObj.ToString().Replace("\r\n", "\n");

                if (srcTest != outputTest)
                {
                    File.WriteAllText(profileFile, outputTest);
                }
            }
        }
    }
}
