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
            // Please manually populate cache from link such as https://www.pokemon-zone.com/champions/pokemon/eelektross-mega-eelektross/ first, as we cannot grab automatically with basic web requset
            Console.WriteLine($"Default species name (Case sensitive):");

            string speciesName = Console.ReadLine().Trim();
            string speciesKeyword = "SPECIES_" + speciesName.ToUpper();

            List<PokemonCompetitiveSet> newSets = new List<PokemonCompetitiveSet>();
            //newSets.AddRange(NewPokemonProfileGenerator.GrabChampionsSets(speciesName, speciesKeyword, ""));
            newSets.AddRange(NewPokemonProfileGenerator.GrabChampionsSets(speciesName, speciesKeyword, "-x"));
            newSets.AddRange(NewPokemonProfileGenerator.GrabChampionsSets(speciesName, speciesKeyword, "-y"));

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
    }
}
