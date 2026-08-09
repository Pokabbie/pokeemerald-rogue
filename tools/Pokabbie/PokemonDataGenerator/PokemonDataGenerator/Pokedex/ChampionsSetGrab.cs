using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Linq;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static PokemonDataGenerator.Pokedex.PokemonProfileGenerator;

namespace PokemonDataGenerator.Pokedex
{
    internal class ChampionsSetGrab
    {
        // Manually grab from link like https://www.pokemon-zone.com/champions/pokemon/scrafty-mega-scrafty/#builds
        // Then save file with https://www.firecrawl.dev/tools/website-to-text at this location
        private static readonly string c_SourceDir = "res://ChampionSets";

        private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
        {
            Formatting = Newtonsoft.Json.Formatting.Indented,
            Converters = new List<JsonConverter>(new[]
            {
                new StringEnumConverter()
            }),
            NullValueHandling = NullValueHandling.Ignore,
        };

        public static void Run()
        {
            Console.WriteLine($"Default species name (Case sensitive):");

            string speciesName = Console.ReadLine().Trim();
            string speciesKeyword = "SPECIES_" + speciesName.ToUpper();

            string textContent = ContentCache.GetHttpContent(Path.Combine(c_SourceDir, $"www-pokemon-zone-com-champions-pokemon-{speciesName}-mega-{speciesName}-.txt"));

            List<PokemonCompetitiveSet> newSets = new List<PokemonCompetitiveSet>();

            using (StringReader reader = new StringReader(textContent))
            {
                bool isReadingSets = false;

                while (true)
                {
                    string line = NextNonBlankLine(reader);

                    if (line.StartsWith("Builds ranked by raw usage across all teams."))
                        isReadingSets = true;

                    if (isReadingSets)
                    {
                        if (line.StartsWith("#"))
                        {
                            string abilityItem = NextNonBlankLine(reader);
                            NextNonBlankLine(reader); // win rate
                            string moves = NextNonBlankLine(reader);
                            string nature = NextNonBlankLine(reader).Replace("Open in Builder", "");

                            newSets.Add(ParseSet(abilityItem, moves, nature));
                        }
                        else if (line.StartsWith("Most Common Teammates"))
                            break; // finished
                    }
                }
            }


            string pokemonProfilePath = Path.Combine(GameDataHelpers.PokemonProfilesDirectory, speciesName, "expansion_profile.json");

            JsonSerializer internalSerializer = JsonSerializer.Create(c_JsonSettings);
            JObject profileObj = JObject.Parse(File.ReadAllText(pokemonProfilePath));
            JArray compSets = (JArray)profileObj["CompetitiveSets"];

            foreach(var set in newSets)
            {
                compSets.Add(JObject.FromObject(set, internalSerializer));
            }

            string outJsonStr = JsonConvert.SerializeObject(profileObj, c_JsonSettings);
            File.WriteAllText(pokemonProfilePath, outJsonStr);
        }

        private static string NextNonBlankLine(StringReader reader)
        {
            while (true)
            {
                string line = reader.ReadLine();

                if (string.IsNullOrEmpty(line))
                    continue;

                return line.Replace("  ", " ").Replace("  ", " ").Replace("  ", " ");
            }
        }

        private static PokemonCompetitiveSet ParseSet(string abilityItemRaw, string movesRaw, string natureRaw)
        {
            PokemonCompetitiveSet output = new PokemonCompetitiveSet();
            string[] aiParts = abilityItemRaw.Split(' ');

            output.Ability = "ABILITY_" + GameDataHelpers.FormatKeyword(aiParts[0]);
            output.Item = "ITEM_" + GameDataHelpers.FormatKeyword(aiParts[1]);
            output.Nature = "NATURE_" + GameDataHelpers.FormatKeyword(natureRaw.Split(',')[0].Split('%')[1].Trim());
            output.SourceTiers.Add("CHAMPIONS_SINGLES_S2");
            output.SourceTiers.Add("CHAMPIONS_DOUBLES_S2");

            string movesLeft = movesRaw;

            for(int m = 0; m < 4; ++m)
            {
                string[] parts = movesLeft.Split(' ');

                for(int i = 1; true; ++i)
                {
                    string potentialMove = string.Join(" ", parts.Take(i));
                    string moveKeyword = "MOVE_" + GameDataHelpers.FormatKeyword(potentialMove);

                    if (GameDataHelpers.MoveDefines.ContainsKey(moveKeyword))
                    {
                        output.Moves.Add(moveKeyword);
                        movesLeft = movesLeft.Substring(potentialMove.Length).TrimStart();
                        break;
                    }
                }
            }

            return output;
        }
    }
}
