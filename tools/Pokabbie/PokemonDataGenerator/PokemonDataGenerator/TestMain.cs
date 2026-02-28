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
        private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
        {
            Formatting = Formatting.Indented,
            Converters = new List<JsonConverter>(new[]
            {
                new StringEnumConverter()
            }),
            NullValueHandling = NullValueHandling.Ignore,
        };

        private static void DoAbc()
        {
            string inputFile = "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue-ee - Copy\\src\\data\\rogue_pokemon_profiles.h";
            string outputFolder = "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue\\src\\data\\rogue\\pokemon\\expansion";

            Dictionary<string, PokemonProfile> profiles = new Dictionary<string, PokemonProfile>();

            string[] lines = File.ReadAllLines(inputFile);

            for(int i = 0; i < lines.Length;)
            {
                string line = lines[i++].Trim();

                if (line.StartsWith("struct RoguePokemonProfile const gRoguePokemonProfiles"))
                    break;

                if(line.StartsWith("static struct LevelUpMove const sLevelUpMoves_"))
                {
                    string species = line.Substring("static struct LevelUpMove const sLevelUpMoves_".Length).Split('[')[0];
                    List<LevelUpMove> moves = new List<LevelUpMove>();

                    if (!profiles.ContainsKey(species))
                    {
                        profiles[species] = new PokemonProfile();
                        profiles[species].Species = new List<string> { species };
                    }

                    i++; // skip {
                    
                    while(true)
                    {
                        line = lines[i++].Trim();
                        if (line.StartsWith("};"))
                            break;

                        string[] parts = line.Replace("{", "").Replace("}", "").Split(',');
                        LevelUpMove move = new LevelUpMove { Move = parts[0].Split('=')[1].Trim(), Level = int.Parse(parts[1].Split('=')[1]) };

                        if(move.Move != "MOVE_NONE")
                            moves.Add(move);
                    }

                    profiles[species].LevelUpMoves = moves;
                }
                else if (line.StartsWith("static u16 const sTutorMoves_"))
                {
                    string species = line.Substring("static u16 const sTutorMoves_".Length).Split('[')[0];
                    List<string> moves = new List<string>();

                    if (!profiles.ContainsKey(species))
                    {
                        profiles[species] = new PokemonProfile();
                        profiles[species].Species.Add(species);
                    }

                    i++; // skip {

                    while (true)
                    {
                        line = lines[i++].Trim();
                        if (line.StartsWith("};"))
                            break;

                        string[] parts = line.Replace("{", "").Replace("}", "").Split(',');
                        string move = parts[0].Trim();

                        if (move != "MOVE_NONE")
                            moves.Add(move);
                    }

                    profiles[species].TutorMoves = moves;
                }
                else if (line.StartsWith("static struct RoguePokemonCompetitiveSet const sCompetitiveSets_"))
                {
                    string species = line.Substring("static struct RoguePokemonCompetitiveSet const sCompetitiveSets_".Length).Split('[')[0];
                    List<PokemonCompetitiveSet> sets = new List<PokemonCompetitiveSet>();
                    PokemonCompetitiveSet currentSet = new PokemonCompetitiveSet();

                    if (!profiles.ContainsKey(species))
                    {
                        profiles[species] = new PokemonProfile();
                        profiles[species].Species.Add(species);
                    }

                    i++; // skip {

                    while (true)
                    {
                        line = lines[i++].Trim();
                        if (line.StartsWith("};"))
                            break;

                        if (line.StartsWith("},"))
                        {
                            sets.Add(currentSet);
                            currentSet = new PokemonCompetitiveSet();
                        }
                        else if (line.StartsWith(".flags="))
                        {
                            string value = line.Split('=')[1].Trim();

                            foreach (var e in value.Replace("(", "").Replace(")", "").Split('|'))
                            {
                                if (e.Trim().StartsWith("MON_FLAGS_"))
                                {
                                    currentSet.SourceTiers.Add(e.Replace(",", "").Trim().Substring("MON_FLAGS_".Length));
                                }
                            }
                        }
                        else if (line.StartsWith(".heldItem="))
                        {
                            string value = line.Split('=')[1].Replace(",", "").Trim();
                            if (value != "ITEM_NONE")
                                currentSet.Item = value;
                        }
                        else if (line.StartsWith(".ability="))
                        {
                            currentSet.Ability = line.Split('=')[1].Replace(",", "").Trim();
                        }
                        else if (line.StartsWith(".hiddenPowerType="))
                        {
                            string value = line.Split('=')[1].Replace(",", "").Trim();
                            if (value != "TYPE_NONE")
                                currentSet.HiddenPower = value;
                        }
                        else if (line.StartsWith(".teraType="))
                        {
                            string value = line.Split('=')[1].Replace(",", "").Trim();
                            if (value != "TYPE_NONE")
                                currentSet.TeraType = value;
                        }
                        else if (line.StartsWith(".nature="))
                        {
                            currentSet.Nature = line.Split('=')[1].Replace(",", "").Trim();
                        }
                        else if (line.StartsWith(".moves="))
                        {
                            i++; // skip {

                            while(true)
                            {
                                line = lines[i++].Trim();
                                if (!line.StartsWith("MOVE_") || line.StartsWith("MOVE_NONE"))
                                    break;

                                currentSet.Moves.Add(line.Replace(",", ""));
                            }
                        }
                    }

                    profiles[species].CompetitiveSets = sets;
                }
            }

            // Match up
            for (int i = 0; i < lines.Length;)
            {
                string line = lines[i++].Trim();

                if(line.StartsWith("[SPECIES_"))
                {
                    string species = line.Replace("[", "").Split(']')[0];

                    i++;

                    bool done = false;
                    while(true)
                    {
                        line = lines[i++].Trim();
                        if (line.StartsWith("},"))
                            break;

                        // Skip for simplisity
                        if (line.StartsWith(".competitiveSetCount"))
                            continue;

                        if(line.Contains("="))
                        {
                            if(!line.Contains("_" + species))
                            {
                                if(!done)
                                {
                                    int index = line.IndexOf("SPECIES_");
                                    string desiredSpecies = line.Substring(index).Replace(",", "").Trim();

                                    profiles[desiredSpecies].Species.Add(species);

                                    done = true;
                                }
                            }
                        }
                    }
                }

            }

            foreach(var species in profiles.Keys)
            {
                string outputDir = Path.Combine(outputFolder, species.Substring("SPECIES_".Length).ToLower());
                Directory.CreateDirectory(outputDir);

                string outputFile = Path.Combine(outputDir, "expansion_profile.json");

                JObject data = JObject.FromObject(profiles[species], new JsonSerializer { NullValueHandling = NullValueHandling.Ignore });

                //JObject.FromObject(deltaProfile)

                string outJsonStr = JsonConvert.SerializeObject(data, c_JsonSettings);
                File.WriteAllText(outputFile, outJsonStr);
            }

            return;
        }

        public static void AWEBAWEAWE()
        {
            string outputDirectory = "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue\\src\\data\\rogue\\pokemon\\vanilla";
            string pokemonProfileFilename = "vanilla_profile.json";


            foreach (string path in Directory.EnumerateFiles(outputDirectory, pokemonProfileFilename, SearchOption.AllDirectories))
            {
                string inJsonStr = File.ReadAllText(path);
                JObject existingProfile = JsonConvert.DeserializeObject<JObject>(inJsonStr, c_JsonSettings);

                if(existingProfile.ContainsKey("RevisedMode"))
                {
                    JObject revisedMode = existingProfile["RevisedMode"] as JObject;

                    if(revisedMode.ContainsKey("LevelUpMoves"))
                    {
                        JObject buildSettings = FindOrCreate<JObject>(revisedMode, "BuildSettings");
                        buildSettings["LevelUpMoves"] = "MERGE";
                    }
                    if (revisedMode.ContainsKey("LevelUpMoves"))
                    {
                        JObject buildSettings = FindOrCreate<JObject>(revisedMode, "BuildSettings");
                        buildSettings["TutorMoves"] = "MERGE";
                    }
                    if (revisedMode.ContainsKey("CompetitiveSets"))
                    {
                        JObject buildSettings = FindOrCreate<JObject>(revisedMode, "BuildSettings");
                        buildSettings["CompetitiveSets"] = "MERGE";
                    }

                    JObject perSpeciesData = new JObject();

                    if (revisedMode.ContainsKey("Types"))
                    {
                        perSpeciesData["Types"] = revisedMode["Types"];
                        revisedMode.Remove("Types");
                    }
                    if (revisedMode.ContainsKey("Abilities"))
                    {
                        perSpeciesData["Abilities"] = revisedMode["Abilities"];
                        revisedMode.Remove("Abilities");
                    }
                    if (revisedMode.ContainsKey("BaseStats"))
                    {
                        perSpeciesData["BaseStats"] = revisedMode["BaseStats"];
                        revisedMode.Remove("BaseStats");
                    }

                    if(perSpeciesData.HasValues)
                    {
                        JObject perSpeciesGroup = new JObject();
                        string species = (existingProfile["Species"] as JArray)[0].ToString();

                        perSpeciesGroup[species] = perSpeciesData;

                        revisedMode["PerSpecies"] = perSpeciesGroup;
                    }


                    string outputPath = Path.Combine(Path.GetDirectoryName(path), Path.GetFileNameWithoutExtension(path) + "_revised.json");
                    File.WriteAllText(outputPath, revisedMode.ToString());

                    existingProfile.Remove("RevisedMode");
                    File.WriteAllText(path, existingProfile.ToString());
                }
            }
        }

        public static void Run()
        {
            //AWEBAWEAWE();
            //return;

            // Format from old data sets
            //DoAbc();
            //return;

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

                string speciesDataDirectory = "C:\\Users\\Digit\\Downloads\\Trainer Data\\Dray\\test_data.json";
                ParseSpeciesData_Dray(speciesDataDirectory, otherProfiles, outputDirectory, pokemonProfileFilename);
            }


            Dictionary<string, string> speciesToProfilePath = new Dictionary<string, string>();
            foreach (string path in Directory.EnumerateFiles(outputDirectory, pokemonProfileFilename, SearchOption.AllDirectories))
            {
                string inJsonStr = File.ReadAllText(path);
                PokemonProfile existingProfile = JsonConvert.DeserializeObject<PokemonProfile>(inJsonStr, c_JsonSettings);

                foreach(var s in existingProfile.Species)
                {
                    speciesToProfilePath[s] = path;
                }
            }

            HashSet<string> loadedPaths = new HashSet<string>();

            foreach(PokemonProfile drayProfile in otherProfiles.Values)
            {
                string species = drayProfile.Species[0];
                string dataPath = speciesToProfilePath[species];
                string revisedDataPath = dataPath.Replace(pokemonProfileFilename, Path.GetFileNameWithoutExtension(pokemonProfileFilename) + "_revised.json");

                string inJsonStr = File.ReadAllText(dataPath);
                PokemonProfile existingProfile = JsonConvert.DeserializeObject<PokemonProfile>(inJsonStr, c_JsonSettings);

                PokemonProfile deltaProfile = new PokemonProfile();
                deltaProfile.BaseStats = drayProfile.BaseStats;
                deltaProfile.Abilities = drayProfile.Abilities;
                deltaProfile.Types = drayProfile.Types;
                deltaProfile.CompetitiveSets = new List<PokemonCompetitiveSet>();
                deltaProfile.LevelUpMoves = new List<LevelUpMove>();
                deltaProfile.TutorMoves = new List<string>();

                if (drayProfile.CompetitiveSets != null)
                {
                    foreach (var set in drayProfile.CompetitiveSets)
                    {
                        if (!existingProfile.HasCompatibleCompetitiveSet(set))
                        {
                            bool isDoubles = set.SourceTiers.Where(t => t.ToLower().Contains("double")).Any();
                            set.SourceTiers = new List<string>(new[] { isDoubles ? "SINGLES_REVISED_MODE" : "DOUBLES_REVISED_MODE" });

                            deltaProfile.CompetitiveSets.Add(set);
                        }
                    }
                }

                if (drayProfile.LevelUpMoves != null)
                {
                    foreach (var lvlMove in drayProfile.LevelUpMoves)
                    {
                        if (existingProfile.GetLevelUpMoveLvl(lvlMove.Move) != lvlMove.Level)
                        {
                            deltaProfile.LevelUpMoves.Add(lvlMove);
                        }
                    }
                }

                if (drayProfile.TutorMoves != null)
                {
                    foreach (var tutorMove in drayProfile.TutorMoves)
                    {
                        if (!existingProfile.HasTutorMove(tutorMove))
                        {
                            deltaProfile.TutorMoves.Add(tutorMove);
                        }
                    }
                }

                JObject jsonObject = JsonConvert.DeserializeObject<JObject>(inJsonStr, c_JsonSettings);
                JObject revisedModeObject = loadedPaths.Contains(revisedDataPath) ? JsonConvert.DeserializeObject<JObject>(File.ReadAllText(revisedDataPath), c_JsonSettings) : new JObject();

                JObject buildSettings = FindOrCreate<JObject>(revisedModeObject, "BuildSettings");

                JObject perSpeciesData = new JObject();

                if (deltaProfile.Types != null)
                    perSpeciesData["Types"] = JArray.FromObject(deltaProfile.Types);
                if (deltaProfile.Abilities != null)
                    perSpeciesData["Abilities"] = JArray.FromObject(deltaProfile.Abilities);
                if (deltaProfile.BaseStats != null)
                    perSpeciesData["BaseStats"] = JObject.FromObject(deltaProfile.BaseStats);

                if (perSpeciesData.HasValues)
                {
                    JObject perSpeciesGroup = FindOrCreate<JObject>(revisedModeObject, "PerSpecies");
                    perSpeciesGroup[species] = perSpeciesData;
                }

                if (deltaProfile.LevelUpMoves.Count != 0)
                {
                    if(deltaProfile.LevelUpMoves.Count <= 3)
                    {
                        buildSettings["LevelUpMoves"] = "MERGE";

                        revisedModeObject["LevelUpMoves"] = JArray.FromObject(deltaProfile.LevelUpMoves);
                    }
                    else
                    {
                        buildSettings["LevelUpMoves"] = "REPLACE";

                        revisedModeObject["LevelUpMoves"] = JArray.FromObject(drayProfile.LevelUpMoves);
                    }
                }
                if (deltaProfile.TutorMoves.Count != 0)
                {
                    buildSettings["TutorMoves"] = "MERGE";

                    revisedModeObject["TutorMoves"] = JArray.FromObject(deltaProfile.TutorMoves);
                }
                if (deltaProfile.CompetitiveSets.Count != 0)
                {
                    if (deltaProfile.CompetitiveSets.Count <= 3)
                    {
                        buildSettings["CompetitiveSets"] = "MERGE";

                        revisedModeObject["CompetitiveSets"] = JArray.FromObject(deltaProfile.CompetitiveSets);
                    }
                    else
                    {
                        buildSettings["CompetitiveSets"] = "REPLACE";

                        revisedModeObject["CompetitiveSets"] = JArray.FromObject(drayProfile.CompetitiveSets);
                    }
                }

                if(!buildSettings.HasValues)
                {
                    revisedModeObject.Remove("BuildSettings");
                }

                jsonObject.Remove("RevisedMode");


                string outJsonStr = JsonConvert.SerializeObject(jsonObject, c_JsonSettings);
                if (inJsonStr != outJsonStr)
                {
                    File.WriteAllText(dataPath, outJsonStr);
                }

                if(revisedModeObject.HasValues)
                {
                    outJsonStr = JsonConvert.SerializeObject(revisedModeObject, c_JsonSettings);

                    File.WriteAllText(revisedDataPath, outJsonStr);
                    loadedPaths.Add(revisedDataPath);
                }
            }

            //foreach (string dir in Directory.EnumerateDirectories(outputDirectory))
            //{
            //    string species = "SPECIES_" + GameDataHelpers.FormatKeyword(Path.GetFileName(dir));
            //    string dataPath = Path.Combine(dir, pokemonProfileFilename);
            //
            //    if (otherProfiles.ContainsKey(species) && File.Exists(dataPath))
            //    {
            //        string inJsonStr = File.ReadAllText(dataPath);
            //
            //        PokemonProfile newProfile = otherProfiles[species];
            //        PokemonProfile existingProfile = JsonConvert.DeserializeObject<PokemonProfile>(inJsonStr, c_JsonSettings);
            //        JObject jsonObject = JsonConvert.DeserializeObject<JObject>(inJsonStr, c_JsonSettings);
            //
            //        // Combine data
            //        {
            //            JObject outData = FindOrCreate<JObject>(jsonObject, "RevisedMode");
            //
            //            if (newProfile.LevelUpMoves.Count > 0)
            //            {
            //                JArray outLevelUpMoves = Create<JArray>(outData, "LevelUpMoves");
            //
            //                foreach (LevelUpMove move in newProfile.LevelUpMoves)
            //                {
            //                    if (existingProfile.GetLevelUpMoveLvl(move.Move) != move.Level)
            //                    {
            //                        outLevelUpMoves.Add(JObject.FromObject(move));
            //                    }
            //                }
            //            }
            //
            //            if (newProfile.TutorMoves.Count > 0)
            //            {
            //                JArray outTutorMoves = Create<JArray>(outData, "TutorMoves");
            //
            //                foreach (string move in newProfile.TutorMoves)
            //                {
            //                    if (!existingProfile.HasTutorMove(move))
            //                    {
            //                        outTutorMoves.Add(move);
            //                    }
            //                }
            //            }
            //
            //            if (newProfile.CompetitiveSets.Count > 0)
            //            {
            //                JArray outCompetitiveSets = Create<JArray>(outData, "CompetitiveSets");
            //
            //                foreach (PokemonCompetitiveSet set in newProfile.CompetitiveSets)
            //                {
            //                    if (!existingProfile.HasCompatibleCompetitiveSet(set))
            //                    {
            //                        outCompetitiveSets.Add(JObject.FromObject(set));
            //                    }
            //                }
            //            }
            //        }
            //
            //        string outJsonStr = JsonConvert.SerializeObject(jsonObject, c_JsonSettings);
            //        if (inJsonStr != outJsonStr)
            //        {
            //            File.WriteAllText(dataPath, outJsonStr);
            //        }
            //    }
            //}
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


        private class RawPokeStats
        {
            public int baseHP;
            public int baseAttack;
            public int baseDefense;
            public int baseSpeed;
            public int baseSpAttack;
            public int baseSpDefense;
            public int type1;
            public int type2;
            public int ability0;
            public int ability1;
            public int ability2;
        }

        private static void ParseSpeciesData_Dray(string filePath, Dictionary<string, PokemonProfile> profilesProfiles, string outputDirectory, string pokemonProfileFilename)
        {
            Dictionary<int, string> speciesNameLookup = new Dictionary<int, string>();


            string inJsonStr = File.ReadAllText(filePath);
            JArray mainArray = JsonConvert.DeserializeObject<JArray>(inJsonStr, c_JsonSettings);

            Dictionary<string, string> speciesDefines = new Dictionary<string, string>();
            GameDataHelpers.ParseFileDefines("#define", "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue-ee\\include\\constants\\species.h", speciesDefines);

            Dictionary<string, string> typesDefines = new Dictionary<string, string>();
            GameDataHelpers.ParseFileDefines("#define TYPE_", "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue-ee\\include\\constants\\pokemon.h", typesDefines);

            Dictionary<string, string> abilityDefines = new Dictionary<string, string>();
            GameDataHelpers.ParseFileDefines("#define", "D:\\Dev\\Pokemon\\GBA\\pokeemerald-rogue-ee\\include\\constants\\abilities.h", abilityDefines);

            foreach (var elem in mainArray)
            {
                JObject currObj = elem as JObject;

                string species = GameDataHelpers.FindKeyFromConstant(speciesDefines, currObj["Species"].Value<int>());

                if (!profilesProfiles.ContainsKey(species))
                {
                    profilesProfiles[species] = new PokemonProfile();
                    profilesProfiles[species].Species = new List<string> { species };
                }

                PokemonProfile activeProfile = profilesProfiles[species];

                RawPokeStats baseData = currObj["Base"].ToObject<RawPokeStats>();
                RawPokeStats drayData = currObj["Dray"].ToObject<RawPokeStats>();

                if (
                    baseData.baseHP != drayData.baseHP ||
                    baseData.baseAttack != drayData.baseAttack ||
                    baseData.baseDefense != drayData.baseDefense ||
                    baseData.baseSpeed != drayData.baseSpeed ||
                    baseData.baseSpAttack != drayData.baseSpAttack ||
                    baseData.baseSpDefense != drayData.baseSpDefense
                )
                {
                    activeProfile.BaseStats = new PokemonBaseStats();

                    activeProfile.BaseStats.HP = drayData.baseHP;
                    activeProfile.BaseStats.Attack = drayData.baseAttack;
                    activeProfile.BaseStats.Defense = drayData.baseDefense;
                    activeProfile.BaseStats.Speed = drayData.baseSpeed;
                    activeProfile.BaseStats.SpAttack = drayData.baseSpAttack;
                    activeProfile.BaseStats.SpDefense = drayData.baseSpDefense;
                }

                if (
                    baseData.type1 != drayData.type1 ||
                    baseData.type2 != drayData.type2
                )
                {
                    activeProfile.Types = new List<string>(new[]{
                        GameDataHelpers.FindKeyFromConstant(typesDefines, drayData.type1),
                        GameDataHelpers.FindKeyFromConstant(typesDefines, drayData.type2),
                    });
                }

                if (
                    baseData.ability0 != drayData.ability0 ||
                    baseData.ability1 != drayData.ability1 ||
                    baseData.ability2 != drayData.ability2
                )
                {
                    activeProfile.Abilities = new List<string>(new[]{
                        GameDataHelpers.FindKeyFromConstant(abilityDefines, drayData.ability0),
                        GameDataHelpers.FindKeyFromConstant(abilityDefines, drayData.ability1),
                        GameDataHelpers.FindKeyFromConstant(abilityDefines, drayData.ability2),
                    });
                }
            }
        }
    }
}
