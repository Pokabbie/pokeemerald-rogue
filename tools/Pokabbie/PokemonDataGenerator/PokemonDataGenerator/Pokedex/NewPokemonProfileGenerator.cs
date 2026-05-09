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

        public static Stream ToStream(string s)
        {
            var stream = new MemoryStream();
            var writer = new StreamWriter(stream);
            writer.Write(s);
            writer.Flush();
            stream.Position = 0;
            return stream;
        }

        private static void AppendChampionsSets(string speciesName, string speciesKeyword, PokemonProfileGenerator.PokemonProfile newProfile)
        {
            string httpContent = ContentCache.GetHttpContent($"https://www.pokemon-zone.com/champions/pokemon/{speciesName.ToLower()}-mega-{speciesName.ToLower()}/");
            int nextIndex = 0;

            while(nextIndex != -1)
            {
                nextIndex = httpContent.IndexOf("<div class=\"build-card__full-member", nextIndex);

                if (nextIndex == -1)
                    break;

                string monData = ExtractXmlNode(httpContent, ref nextIndex);

                string currSpecies = "";
                string currAbility = "";
                string currItem = "";

                // Read mon entry
                {
                    int headIndex = monData.IndexOf("<div class=\"build-card__full-member-info\"", 0);
                    string headData = ExtractXmlNode(monData, ref headIndex);

                    using (XmlReader xml = XmlReader.Create(ToStream(headData)))
                    {
                        while(xml.Read())
                        {
                            if (xml.Name == "a")
                            {
                                currSpecies = xml.ReadInnerXml();
                            }
                            if (xml.Name == "span")
                            {
                                currAbility = xml.ReadInnerXml();
                                break;
                            }
                        }
                    }

                    string[] parts = headData.Replace('>', '<').Split('<');
                    currItem = parts[20].Trim();
                }

                if(speciesKeyword == "SPECIES_" + GameDataHelpers.FormatKeyword(currSpecies))
                {
                    PokemonCompetitiveSet newSet = new PokemonCompetitiveSet();
                    newSet.Nature = "NATURE_TODO";
                    newSet.SourceTiers.Add("CHAMPIONS_SINGLES_S1");
                    newSet.SourceTiers.Add("CHAMPIONS_DOUBLES_S1");
                    newSet.Ability = "ABILITY_" + GameDataHelpers.FormatKeyword(currAbility);
                    newSet.Item = "ITEM_" + GameDataHelpers.FormatKeyword(currItem);

                    // Read moves
                    {
                        int movesIndex = monData.IndexOf("<div class=\"build-card__full-member-moves\"", 0);
                        string movesData = ExtractXmlNode(monData, ref movesIndex);

                        string[] parts = movesData.Replace('>', '<').Split('<');

                        newSet.Moves.Add("MOVE_" + GameDataHelpers.FormatKeyword(parts[6].Trim()));
                        newSet.Moves.Add("MOVE_" + GameDataHelpers.FormatKeyword(parts[12].Trim()));
                        newSet.Moves.Add("MOVE_" + GameDataHelpers.FormatKeyword(parts[18].Trim()));
                        newSet.Moves.Add("MOVE_" + GameDataHelpers.FormatKeyword(parts[24].Trim()));
                    }

                    if (!newProfile.HasCompatibleCompetitiveSet(newSet))
                        newProfile.CompetitiveSets.Add(newSet);
                }
            }
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
