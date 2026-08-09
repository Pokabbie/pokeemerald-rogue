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
using System.Xml;
using PokemonDataGenerator.Utils;

namespace PokemonDataGenerator.Pokedex
{
    public class NewPokemonProfileGenerator
    {
        private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
        {
            Formatting = Newtonsoft.Json.Formatting.Indented,
            Converters = new List<JsonConverter>(new[]
            {
                new StringEnumConverter()
            }),
            NullValueHandling = NullValueHandling.Ignore,
        };

        public static void GatherProfile(string speciesName)
        {
            string speciesKeyword = "SPECIES_" + GameDataHelpers.FormatKeyword(speciesName);

            PokemonProfileGenerator.PokemonProfile newProfile = PokemonProfileGenerator.GatherProfileFor(speciesKeyword, false);
            string exportFile = PokemonProfileGenerator.GetExportFilePathFor(newProfile);

            AppendChampionsSets(speciesName, speciesKeyword, newProfile);

            string inJsonStr = File.ReadAllText(exportFile);
            JObject jsonObject = JsonConvert.DeserializeObject<JObject>(inJsonStr, c_JsonSettings);
            PokemonProfileGenerator.PokemonProfile existingProfile = JsonConvert.DeserializeObject<PokemonProfile>(inJsonStr, c_JsonSettings);

            JsonSerializer internalSerializer = JsonSerializer.Create(c_JsonSettings);

            // Lvl Moves
            {
                
                jsonObject["LevelUpMoves"] = JArray.FromObject(newProfile.LevelUpMoves, internalSerializer);
            }

            // Tutor Moves
            {
                JArray target = jsonObject["TutorMoves"] as JArray;

                foreach (string move in newProfile.TutorMoves)
                {
                    if (!existingProfile.HasTutorMove(move))
                    {
                        target.Add(move);
                    }
                }
            }

            // Competitive Sets
            {
                JArray target = jsonObject["CompetitiveSets"] as JArray;

                foreach(PokemonCompetitiveSet set in newProfile.CompetitiveSets)
                {
                    if(!existingProfile.HasCompatibleCompetitiveSet(set))
                    {
                        target.Add(JObject.FromObject(set, internalSerializer));
                    }
                }
            }

            string outJsonStr = JsonConvert.SerializeObject(jsonObject, c_JsonSettings);
            if (inJsonStr != outJsonStr)
            {
                File.WriteAllText(exportFile, outJsonStr);
            }
        }

        private static void AppendChampionsSets(string speciesName, string speciesKeyword, PokemonProfileGenerator.PokemonProfile newProfile)
        {
            foreach(var newSet in GrabChampionsSets(speciesName, speciesKeyword))
            {
                if (!newProfile.HasCompatibleCompetitiveSet(newSet))
                    newProfile.CompetitiveSets.Add(newSet);
            }
        }

        public static List<PokemonCompetitiveSet> GrabChampionsSets(string speciesName, string speciesKeyword, string suffix = "")
        {
            string httpContent = ContentCache.ParseHttpContentPlainText($"https://www.pokemon-zone.com/champions/pokemon/{speciesName.ToLower()}-mega-{speciesName.ToLower()}{suffix}/");
            List<PokemonCompetitiveSet> newSets = new List<PokemonCompetitiveSet>();
            
            using (StringReader reader = new StringReader(httpContent))
            {
                bool isReadingSets = false;
                string line;

                while ((line = NextNonBlankLine(reader)) != null)
                {
                    if (line.StartsWith("Builds ranked by raw usage across all teams.>"))
                    {
                        line = line.Substring("Builds ranked by raw usage across all teams.>".Length);
                        isReadingSets = true;
                    }
            
                    if (isReadingSets)
                    {
                        int splitIndex = line.IndexOf("Open in Builder#");
                        if(splitIndex != -1)
                        {
                            line = line.Substring(splitIndex + "Open in Builder".Length);
                        }

                        if (line.StartsWith("#"))
                        {
                            newSets.Add(ChampsParseSet(line.Substring("#1".Length)));
                        }
                        else if (line.StartsWith("Submissions that don't match in-game data"))
                            break; // finished
                    }
                }
            }

            return newSets;
        }

        private static string NextNonBlankLine(StringReader reader)
        {
            while (true)
            {
                string line = reader.ReadLine();

                if(line == null)
                    return null;

                if (string.IsNullOrEmpty(line))
                    continue;

                return line.Trim().Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ").Replace("  ", " ");
            }
        }

        private static PokemonCompetitiveSet ChampsParseSet(string rawLine)
        {
            PokemonCompetitiveSet output = new PokemonCompetitiveSet();
            string[] inputParts = rawLine.Split('>');
            inputParts[0] = inputParts[0].Replace("e X", "e_X").Replace("e Y", "e_Y").Replace("e Z", "e_Z");

            string[] aiParts = inputParts[0].Split(' ');

            output.Ability = "ABILITY_" + GameDataHelpers.FormatKeyword(string.Join(" ", aiParts.Take(aiParts.Length - 1)));
            output.Item = "ITEM_" + GameDataHelpers.FormatKeyword(aiParts.Last());
            output.Nature = "NATURE_" + GameDataHelpers.FormatKeyword(inputParts[6].Split(':')[1].Trim());
            output.SourceTiers.Add("CHAMPIONS_SINGLES_S2");
            output.SourceTiers.Add("CHAMPIONS_DOUBLES_S2");

            for(int m = 0; m < 4; ++m)
            {
                output.Moves.Add("MOVE_" + GameDataHelpers.FormatKeyword(inputParts[m + 2]));
            }

            return output;
        }

        private static string ExtractXmlNode(string source, ref int index)
        {
            string output = "";
            int depth = 0;

            for (int i = index; i < source.Length; i++)
            {
                output += source[i];

                if (output.EndsWith("<div "))
                {
                    ++depth;
                }
                else if (output.EndsWith("</div>"))
                {
                    --depth;

                    if (depth == 0)
                    {
                        index = i;
                        break;
                    }
                }
            }

            return output;
        }
    }
}
