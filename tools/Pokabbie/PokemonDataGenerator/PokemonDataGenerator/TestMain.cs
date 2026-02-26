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

namespace PokemonDataGenerator
{
    internal class TestMain
    {
        private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
        {
            Formatting = Formatting.Indented,
            Converters = new List<JsonConverter>(new[]
            {
                new StringEnumConverter()
            })
        };

        public static void Run()
        {
            string outputDirectory;
            string pokemonProfileFilename;

            // Parse profiles from csv
            Dictionary<string, PokemonProfile> otherProfiles = new Dictionary<string, PokemonProfile>();

            if (false) // GameDataHelpers.IsVanillaVersion)
            {
                outputDirectory = "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue\\src\\data\\rogue\\pokemon\\vanilla";
                pokemonProfileFilename = "vanilla_profile.json";

                string movesetPath = "C:\\Users\\Digit\\Downloads\\Learnsets - Royal Sapphire (1.7) - Learnsets.csv";
                string trainerDataDir = "C:\\Users\\Digit\\Downloads\\Trainer Data";

                ParseMovesetDataRoyalSapphire(movesetPath, otherProfiles);

                foreach (string filePath in Directory.EnumerateFiles(trainerDataDir))
                {
                    ParseTrainerCsvRoyalSapphire(filePath, otherProfiles);
                }
            }
            else
            {
                outputDirectory = "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue\\src\\data\\rogue\\pokemon\\expansion";
                pokemonProfileFilename = "expansion_profile.json";

                string drayanoDatasets = "D:\\Dev\\Pokemon\\GBA\\Other Sources\\pokeemerald-rogue-drayano\\tools\\Pokabbie\\PokemonDataGenerator\\PokemonDataGenerator\\Resources\\PokemonProfiles\\Rebalanced";

                foreach (string filePath in Directory.EnumerateFiles(drayanoDatasets))
                {
                    JObject jsonData = JsonConvert.DeserializeObject<JObject>(File.ReadAllText(filePath), c_JsonSettings);
                    string species = jsonData["Species"].ToString();
                    jsonData["Species"] = new JArray(new object [] { species });

                    PokemonProfile profile = jsonData.ToObject<PokemonProfile>();
                    otherProfiles[species.ToString()] = profile;
                }
            }


            foreach (string dir in Directory.EnumerateDirectories(outputDirectory))
            {
                string species = "SPECIES_" + GameDataHelpers.FormatKeyword(Path.GetFileName(dir));
                string dataPath = Path.Combine(dir, pokemonProfileFilename);

                if (otherProfiles.ContainsKey(species) && File.Exists(dataPath))
                {
                    string inJsonStr = File.ReadAllText(dataPath);

                    PokemonProfile royalSapphireProfile = otherProfiles[species];
                    PokemonProfile gameProfile = JsonConvert.DeserializeObject<PokemonProfile>(inJsonStr, c_JsonSettings);
                    JObject jsonObject = JsonConvert.DeserializeObject<JObject>(inJsonStr, c_JsonSettings);

                    // Combine data
                    {
                        JObject outData = FindOrCreate<JObject>(jsonObject, "RevisedMode");

                        if (royalSapphireProfile.LevelUpMoves.Count > 0)
                        {
                            JArray outLevelUpMoves = Create<JArray>(outData, "LevelUpMoves");

                            foreach (LevelUpMove move in royalSapphireProfile.LevelUpMoves)
                            {
                                if (gameProfile.GetLevelUpMoveLvl(move.Move) != move.Level)
                                {
                                    outLevelUpMoves.Add(JObject.FromObject(move));
                                }
                            }
                        }

                        if (royalSapphireProfile.TutorMoves.Count > 0)
                        {
                            JArray outTutorMoves = Create<JArray>(outData, "TutorMoves");

                            foreach (string move in royalSapphireProfile.TutorMoves)
                            {
                                if (!gameProfile.HasTutorMove(move))
                                {
                                    outTutorMoves.Add(move);
                                }
                            }
                        }

                        if (royalSapphireProfile.CompetitiveSets.Count > 0)
                        {
                            JArray outCompetitiveSets = Create<JArray>(outData, "CompetitiveSets");

                            foreach (PokemonCompetitiveSet set in royalSapphireProfile.CompetitiveSets)
                            {
                                if (!gameProfile.HasCompatibleCompetitiveSet(set))
                                {
                                    outCompetitiveSets.Add(JObject.FromObject(set));
                                }
                            }
                        }
                    }

                    string outJsonStr = JsonConvert.SerializeObject(jsonObject, c_JsonSettings);
                    if (inJsonStr != outJsonStr)
                    {
                        File.WriteAllText(dataPath, outJsonStr);
                    }
                }
            }
        }

        private static bool IsLineEmpty(string[] lines)
        {
            foreach (string k in lines)
            {
                if (!string.IsNullOrWhiteSpace(k))
                {
                    return false;
                }
            }

            return true;
        }

        private static T FindOrCreate<T>(JObject obj, string name) where T : JToken, new()
        {
            if(!obj.ContainsKey(name))
            {
                obj[name] = new T();
            }

            return (T)obj[name];
        }
        private static T Create<T>(JObject obj, string name) where T : JToken, new()
        {
            obj[name] = new T();
            return (T)obj[name];
        }

        private static void ParseMovesetDataRoyalSapphire(string filePath, Dictionary<string, PokemonProfile> royalSapphireProfiles)

        {
            List<PokemonProfile> activeProfile = new List<PokemonProfile>();

            foreach (string line in File.ReadLines(filePath))
            {
                string[] lineParts = line.Split(',');

                if (IsLineEmpty(lineParts))
                {
                    activeProfile.Clear();
                }

                if (activeProfile.Count == 0)
                {
                    foreach (string s in lineParts.Skip(2))
                    {
                        if (string.IsNullOrEmpty(s))
                            break;

                        string species = "SPECIES_" + GameDataHelpers.FormatKeyword(s);

                        PokemonProfile profile = new PokemonProfile();
                        profile.Species = new List<string> { species };
                        profile.CompetitiveSets = new List<PokemonCompetitiveSet>();
                        profile.LevelUpMoves = new List<LevelUpMove>();
                        profile.TutorMoves = new List<string>();
                        royalSapphireProfiles[species] = profile;
                        activeProfile.Add(profile);
                    }
                }
                else
                {
                    string moveName = lineParts[1];
                    string moveKeyword = GameDataHelpers.FormatToMatch(GameDataHelpers.MoveDefines, "MOVE_" + GameDataHelpers.FormatKeyword(moveName));

                    if (!string.IsNullOrEmpty(moveName))
                    {
                        lineParts = lineParts.Skip(2).ToArray();

                        for (int i = 0; i < activeProfile.Count; ++i)
                        {
                            if (string.IsNullOrEmpty(lineParts[i]))
                                continue;

                            if (lineParts[i].Equals("Tutor", StringComparison.CurrentCultureIgnoreCase))
                            {
                                activeProfile[i].TutorMoves.Add(moveKeyword);
                            }
                            else if (int.TryParse(lineParts[i], out int lvl))
                            {
                                LevelUpMove lvlMove = new LevelUpMove();
                                lvlMove.Move = moveKeyword;
                                lvlMove.Level = lvl;
                                activeProfile[i].LevelUpMoves.Add(lvlMove);
                            }
                        }
                    }
                }

            }
        }

        private static void ParseTrainerCsvRoyalSapphire(string filePath, Dictionary<string, PokemonProfile> royalSapphireProfiles)
        {
            string[] lines = File.ReadAllLines(filePath);

            for (int i = 0; i < lines.Length;)
            {
                // Hunt for trainer start block
                string line = lines[i++]
                    .Replace(",MANDATORY,", ",,")
                    .Replace(",OPTIONAL,", ",,")
                    .Replace(",-,", ",,");

                string[] parts = line.Split(',');

                string trainerName = parts[1];

                if (!string.IsNullOrWhiteSpace(trainerName))
                {
                    string checkStr = line.Replace(trainerName, "").Replace(",", "");
                    if(!string.IsNullOrWhiteSpace(checkStr))
                    {
                        continue;
                    }

                    bool isDoublesSet = line.ToLower().Contains("double");

                    parts = lines[i++].Replace(",-,", ",,").Split(',');

                    // No pokemon
                    if (string.IsNullOrWhiteSpace(parts[1]))
                        continue;

                    // Prepate to parse comp sets
                    List<PokemonProfile> activeProfiles = new List<PokemonProfile>();
                    List<PokemonCompetitiveSet> activeSets = new List<PokemonCompetitiveSet>();

                    foreach (string s in parts.Skip(1))
                    {
                        if(string.IsNullOrWhiteSpace(s))
                            break;

                        string species = GameDataHelpers.FormatToMatch(GameDataHelpers.SpeciesDefines, "SPECIES_" + GameDataHelpers.FormatKeyword(s));

                        if(!royalSapphireProfiles.ContainsKey(species))
                        {
                            PokemonProfile profile = new PokemonProfile();
                            profile.Species = new List<string> { species };
                            profile.CompetitiveSets = new List<PokemonCompetitiveSet>();
                            profile.LevelUpMoves = new List<LevelUpMove>();
                            profile.TutorMoves = new List<string>();
                            royalSapphireProfiles[species] = profile;
                        }

                        activeProfiles.Add(royalSapphireProfiles[species]);
                        activeSets.Add(new PokemonCompetitiveSet());
                    }

                    do
                    {
                        if (i == lines.Length)
                            return;

                        parts = lines[i++].Replace(",-,", ",,").Split(',');

                        if (!string.IsNullOrWhiteSpace(parts[1]))
                        {
                            if (int.TryParse(parts[1], out _)) // ignore lvl
                            {
                                parts = lines[i++].Replace(",-,", ",,").Split(',');
                                break;
                            }
                        }
                    }
                    while (true);

                    // Item
                    for (int j = 0; j < activeProfiles.Count; ++j)
                    {
                        if (!string.IsNullOrWhiteSpace(parts[j + 1]))
                        {
                            activeSets[j].Item = GameDataHelpers.FormatToMatch(GameDataHelpers.ItemDefines, GameDataHelpers.FormatKeyword("ITEM_" + parts[j + 1]));
                        }
                        else
                        {
                            activeSets[j].Item = "ITEM_NONE";
                        }
                    }

                    // Ability
                    parts = lines[i++].Replace(",-,", ",,").Split(',');

                    for (int j = 0; j < activeProfiles.Count; ++j)
                    {
                        activeSets[j].Ability = GameDataHelpers.FormatToMatch(GameDataHelpers.AbilityDefines, GameDataHelpers.FormatKeyword("ABILITY_" + parts[j + 1]));
                    }

                    // Nature
                    parts = lines[i++].Replace(",-,", ",,").Split(',');

                    for (int j = 0; j < activeProfiles.Count; ++j)
                    {
                        if (!string.IsNullOrWhiteSpace(parts[j + 1]))
                        {
                            activeSets[j].Nature = GameDataHelpers.FormatToMatch(GameDataHelpers.NatureDefines, GameDataHelpers.FormatKeyword("NATURE_" + parts[j + 1]));

                            if (!GameDataHelpers.NatureDefines.ContainsKey(activeSets[j].Nature))
                                return;
                        }
                    }

                    // Skip IVs
                    i++;

                    // Moves
                    for (int m = 0; m < 4; ++m)
                    {
                        parts = lines[i++].Replace(",-,", ",,").Split(',');

                        for (int j = 0; j < activeProfiles.Count; ++j)
                        {
                            if (!string.IsNullOrWhiteSpace(parts[j + 1]))
                            {
                                string moveKey = GameDataHelpers.FormatToMatch(GameDataHelpers.MoveDefines, GameDataHelpers.FormatKeyword("MOVE_" + parts[j + 1]));

                                if (moveKey == "MOVE_FEINT_ATTACK")
                                    moveKey = "MOVE_FAINT_ATTACK";
                                if (moveKey == "MOVE_OCTAZOOOKA")
                                    moveKey = "MOVE_OCTAZOOKA";
                                if (moveKey == "MOVE_PIN_MISSLE")
                                    moveKey = "MOVE_PIN_MISSILE";

                                if(moveKey != "MOVE__")
                                    activeSets[j].Moves.Add(moveKey);
                            }
                        }
                    }

                    for (int j = 0; j < activeProfiles.Count; ++j)
                    {
                        if (!activeProfiles[j].HasCompatibleCompetitiveSet(activeSets[j]))
                        {
                            activeSets[j].SourceTiers.Add(isDoublesSet ? "SINGLES_REVISED_MODE" : "DOUBLES_REVISED_MODE");

                            activeProfiles[j].CompetitiveSets.Add(activeSets[j]);
                        }
                    }

                    continue;
                }
            }
        }
    }
}
