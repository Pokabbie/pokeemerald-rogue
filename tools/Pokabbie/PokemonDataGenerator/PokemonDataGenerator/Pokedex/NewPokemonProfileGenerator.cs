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

namespace PokemonDataGenerator.Pokedex
{
    public class NewPokemonProfileGenerator
    {
        private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
        {
            Formatting = Formatting.Indented,
            Converters = new List<JsonConverter>(new[]
            {
                new StringEnumConverter()
            }),
            NullValueHandling = NullValueHandling.Ignore,
        };

        public static void GatherProfile(string speciesName)
        {
            string speciesKeyword = "SPECIES_" + speciesName.ToUpper();

            PokemonProfileGenerator.PokemonProfile newProfile = PokemonProfileGenerator.GatherProfileFor(speciesKeyword, false);
            string exportFile = PokemonProfileGenerator.GetExportFilePathFor(newProfile);

            string inJsonStr = File.ReadAllText(exportFile);
            JObject jsonObject = JsonConvert.DeserializeObject<JObject>(inJsonStr, c_JsonSettings);
            PokemonProfileGenerator.PokemonProfile existingProfile = JsonConvert.DeserializeObject<PokemonProfile>(inJsonStr, c_JsonSettings);

            AppendChampionsSets(speciesName, existingProfile);

            // Lvl Moves
            {
                JArray target = jsonObject["LevelUpMoves"] as JArray;
                target = JArray.FromObject(newProfile.LevelUpMoves);
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
                target = JArray.FromObject(newProfile.CompetitiveSets);

                //foreach (string move in newProfile.TutorMoves)
                //{
                //    if (!existingProfile.HasTutorMove(move))
                //    {
                //        target.Add(move);
                //    }
                //}
            }

            string outJsonStr = JsonConvert.SerializeObject(jsonObject, c_JsonSettings);
            if (inJsonStr != outJsonStr)
            {
                File.WriteAllText(exportFile, outJsonStr);
            }
        }

        private static void AppendChampionsSets(string speciesName, PokemonProfileGenerator.PokemonProfile newProfile)
        {
            string httpContent = ContentCache.GetHttpContent($"https://www.pokemon-zone.com/champions/pokemon/{speciesName}/?mode=meta#builds");
        }
    }
}
