using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Sprites
{
    public class ShinySpriteHelper
    {
        public static ImagePalette GenerateShinyPaletteFromRef(ImagePalette srcPal, ImagePalette refNormalPal, ImagePalette refShinyPal)
        {
            ImagePalette output = new ImagePalette(srcPal.PaletteSize);

            output[0] = srcPal[0];

            for (int i = 1; i < srcPal.PaletteSize; i++)
            {
                if (srcPal[i].A == 0)
                {
                    output[i] = srcPal[0];
                }
                else
                {
                    int matchingIdx = refNormalPal.GetClosestMatchIndex(srcPal[i]);
                    output[i] = refShinyPal[matchingIdx];
                }
            }

            return output;
        }

        private class PokemonGraphicsPaths
        {
            public string NormalPalette;
            public string ShinyPalette;
            public string Icon;
            public string ShinyIcon;
        }

        private static void ExtractMonPathInfoFromLine(string line, out string key, out string path)
        {
            key = string.Join("_", line.Split('[')[0].Split('_').Skip(1)).Trim();

            if (line.Contains("=") && line.Contains("\""))
                path = line.Split('\"')[1];
            else
                path = null;
        }

        private static Dictionary<string, PokemonGraphicsPaths> GetPokemonGraphicsPaths(List<string> lines)
        {
            Dictionary<string, PokemonGraphicsPaths> outTable = new Dictionary<string, PokemonGraphicsPaths>();

            foreach (string line in lines)
            {
                if (line.TrimStart().StartsWith("//"))
                    continue;

                if (line.Contains("gMonPalette_"))
                {
                    string species;
                    string path;
                    ExtractMonPathInfoFromLine(line, out species, out path);

                    if(!outTable.ContainsKey(species))
                    {
                        outTable.Add(species, new PokemonGraphicsPaths());
                    }

                    outTable[species].NormalPalette = path;
                }
                else if (line.Contains("gMonShinyPalette_"))
                {
                    string species;
                    string path;
                    ExtractMonPathInfoFromLine(line, out species, out path);

                    if (!outTable.ContainsKey(species))
                    {
                        outTable.Add(species, new PokemonGraphicsPaths());
                    }

                    outTable[species].ShinyPalette = path;
                }
                else if (line.Contains("gMonIcon_"))
                {
                    string species;
                    string path;
                    ExtractMonPathInfoFromLine(line, out species, out path);

                    if (!outTable.ContainsKey(species))
                    {
                        outTable.Add(species, new PokemonGraphicsPaths());
                    }

                    outTable[species].Icon = path;
                }
                else if (line.Contains("gMonShinyIcon_"))
                {
                    string species;
                    string path;
                    ExtractMonPathInfoFromLine(line, out species, out path);

                    if (!outTable.ContainsKey(species))
                    {
                        outTable.Add(species, new PokemonGraphicsPaths());
                    }

                    outTable[species].ShinyIcon = path;
                }
            }

            outTable.Remove("Egg");
            outTable.Remove("QuestionMark");

            return outTable;
        }

        private static void DetermineGenerationPalettes(Dictionary<string, PokemonGraphicsPaths> pkmnGraphicsLookup, string key, out string normalPalFile, out string shinyPalFile)
        {
            if (key.StartsWith("Unown"))
            {
                key = "Unown";
            }
            else if (GameDataHelpers.IsVanillaVersion)
            {
                if (key.StartsWith("Deoxys"))
                {
                    key = "Deoxys";
                }
            }

            var monPaths = pkmnGraphicsLookup[key];

            if (!GameDataHelpers.IsVanillaVersion && (monPaths.NormalPalette == null || monPaths.ShinyPalette == null))
            {
                if(key.EndsWith("F"))
                {
                    monPaths = pkmnGraphicsLookup[key.Substring(0, key.Length - 1)];
                }
                else if(key == "Arceus")
                {
                    monPaths = pkmnGraphicsLookup["ArceusNormal"];
                }
                else if (key == "Silvally")
                {
                    monPaths = pkmnGraphicsLookup["SilvallyNormal"];
                }
                else if(key.StartsWith("Minior"))
                {
                    monPaths = pkmnGraphicsLookup["MiniorMeteor"];
                }
                else if (key.StartsWith("AlcremieStrawberry"))
                {
                    normalPalFile = Path.Combine(GameDataHelpers.RootDirectory, monPaths.NormalPalette.Replace(".lz", "").Replace(".gbapal", ".pal"));

                    monPaths = pkmnGraphicsLookup["AlcremieStrawberry"];
                    shinyPalFile = Path.Combine(GameDataHelpers.RootDirectory, monPaths.ShinyPalette.Replace(".lz", "").Replace(".gbapal", ".pal"));
                    return;
                }
                else if (key == "Urshifu")
                {
                    monPaths = pkmnGraphicsLookup["UrshifuSingleStrikeStyle"];
                }
                else if (key.StartsWith("Maushold"))
                {
                    monPaths = pkmnGraphicsLookup["Maushold"];
                }
            }

            normalPalFile = Path.Combine(GameDataHelpers.RootDirectory, monPaths.NormalPalette.Replace(".lz", "").Replace(".gbapal", ".pal"));
            shinyPalFile = Path.Combine(GameDataHelpers.RootDirectory, monPaths.ShinyPalette.Replace(".lz", "").Replace(".gbapal", ".pal"));

            if (GameDataHelpers.IsVanillaVersion)
            {
                switch (key)
                {
                    case "Castform":
                        normalPalFile = normalPalFile.Replace(".pal", "_normal_form.pal");
                        shinyPalFile = shinyPalFile.Replace(".pal", "_normal_form.pal");
                        break;
                }
            }
        }

        public static void GeneratePartyIcons()
        {
            // Determine icons to update
            string dataSrcFile = Path.Combine(GameDataHelpers.SrcDataDirectory, "graphics\\pokemon.h");
            List<string> dataSrcLines = File.ReadAllLines(dataSrcFile).ToList();
            List<string> iconsToGenerate = new List<string>();

            Dictionary<string, PokemonGraphicsPaths> pkmnGraphicsLookup = GetPokemonGraphicsPaths(dataSrcLines);



            string graphicsDir = Path.Combine(GameDataHelpers.RootDirectory, "graphics\\pokemon");

            //string palettesDir = Path.Combine(graphicsDir, "icon_palettes");
            //List<ImagePalette> iconPalettes = new List<ImagePalette>();
            //
            //for(int i = 0; true; ++i)
            //{
            //    string palFile = Path.Combine(palettesDir, $"pal{i}.pal");
            //    if(File.Exists(palFile))
            //    {
            //        iconPalettes.Add(ImagePalette.FromFile(palFile, ImagePalette.DistanceMethod.YUV));
            //    }
            //    else
            //    {
            //        break;
            //    }
            //}

            Dictionary<string, string> normalToShinyPaths = new Dictionary<string, string>();

            foreach (var kvp in pkmnGraphicsLookup) 
            {
                var monPaths = kvp.Value;

                if (monPaths.Icon == null || monPaths.ShinyIcon != null)
                    continue;

                string iconFile = Path.Combine(GameDataHelpers.RootDirectory, monPaths.Icon.Replace(".4bpp", ".png"));
                string outputFile = Path.Combine(GameDataHelpers.RootDirectory, monPaths.Icon.Replace(".4bpp", "_shiny.png"));
                string normalPalFile;
                string shinyPalFile;
                DetermineGenerationPalettes(pkmnGraphicsLookup, kvp.Key, out normalPalFile, out shinyPalFile);

                Bitmap iconImage = new Bitmap(iconFile);
                ImagePalette normalPal = ImagePalette.FromFile(normalPalFile, ImagePalette.DistanceMethod.YUV);
                ImagePalette shinyPal = ImagePalette.FromFile(shinyPalFile, ImagePalette.DistanceMethod.YUV);

                Bitmap iconInNormalPal = normalPal.CreateIndexedBitmap(iconImage);
                Bitmap shinyIcon = shinyPal.OverwriteIndexedBitmap(iconInNormalPal);
                shinyIcon.Save(outputFile);

                Console.WriteLine($"{kvp.Key} shiny_icon: done");
                normalToShinyPaths[iconFile] = outputFile;

                //Bitmap iconImage = new Bitmap(iconFile, true);
                //ImagePalette iconPalette = ImagePalette.FromImage(iconImage, ImagePalette.DistanceMethod.YUV);
                //iconImage = iconPalette.CreateIndexedBitmap(iconImage); // can't import it, so this is a work around
                //
                //ImagePalette normalPal = ImagePalette.FromFile(normalPalFile, ImagePalette.DistanceMethod.YUV);
                //ImagePalette shinyPal = ImagePalette.FromFile(shinyPalFile, ImagePalette.DistanceMethod.YUV);
                //
                //ImagePalette updatedPal = GenerateShinyPaletteFromRef(iconPalette, normalPal, shinyPal);
                //Bitmap shinyIcon = updatedPal.OverwriteIndexedBitmap(iconImage);

                // Now compare against all the valid icon palettes
                //shinyIcon.Save(Path.Combine(pkmnDir, "icon_shiny_raw.png"));
                //
                //int bestIndex = 0;
                //double bestScore = iconPalettes[0].GetBitmapMatchScoreBespoke(shinyIcon);
                //
                //for(int i = 1; i < iconPalettes.Count; i++)
                //{
                //    double currScore = iconPalettes[i].GetBitmapMatchScoreBespoke(shinyIcon);
                //    if (currScore < bestScore)
                //    {
                //        bestIndex = i;
                //        bestScore = currScore;
                //    }
                //}
                //
                //Console.WriteLine($"{pkmnDir} shiny_icon: pal{bestIndex}");
                //Bitmap finalIcon = iconPalettes[bestIndex].CreateIndexedBitmap(shinyIcon);
                //finalIcon.Save(outputFile);
            }

            // If we succeed everything else, update data

            // Export icon table
            if (GameDataHelpers.IsVanillaVersion)
            {
                string targetFile = Path.Combine(GameDataHelpers.RootDirectory, "src\\pokemon_icon.c");
                List<string> inputLines = File.ReadLines(targetFile).ToList();
                List<string> outputLines = new List<string>();

                foreach (string line in inputLines)
                {
                    outputLines.Add(line);

                    if (line.Contains("gMonIcon_"))
                    {
                        foreach (var kvp in pkmnGraphicsLookup)
                        {
                            if (kvp.Value.ShinyIcon == null)
                            {
                                if (kvp.Value.Icon != null && line.Contains("gMonIcon_" + kvp.Key))
                                {
                                    outputLines.Add(
                                        line
                                        .Replace("gMonIcon_", "gMonShinyIcon_")
                                        .Replace("]", " + ICON_SHINY_OFFSET]")
                                    );
                                    break;
                                }
                            }
                        }
                    }
                }

                if (inputLines.Count != outputLines.Count)
                    File.WriteAllLines(targetFile, outputLines);
            }

            // Export graphics externs
            if (GameDataHelpers.IsVanillaVersion)
            {
                string targetFile = Path.Combine(GameDataHelpers.RootDirectory, "include\\graphics.h");
                List<string> inputLines = File.ReadLines(targetFile).ToList();
                List<string> outputLines = new List<string>();

                foreach (string line in inputLines)
                {
                    outputLines.Add(line);

                    if (line.Contains("gMonIcon_"))
                    {
                        ExtractMonPathInfoFromLine(line, out string key, out _);

                        if (!pkmnGraphicsLookup.ContainsKey(key))
                            continue;

                        var graphicsPaths = pkmnGraphicsLookup[key];

                        if (graphicsPaths.ShinyIcon == null)
                        {
                            outputLines.Add(
                                line
                                .Replace("gMonIcon_", "gMonShinyIcon_")
                            );
                        }
                    }
                }

                if (inputLines.Count != outputLines.Count)
                    File.WriteAllLines(targetFile, outputLines);
            }

            // Export graphics data header
            {
                List<string> outputLines = new List<string>();

                foreach (string line in dataSrcLines)
                {
                    outputLines.Add(line);

                    if (line.Contains("gMonIcon_"))
                    {
                        ExtractMonPathInfoFromLine(line, out string key, out _);

                        if (!pkmnGraphicsLookup.ContainsKey(key))
                            continue;

                        var graphicsPaths = pkmnGraphicsLookup[key];

                        if(graphicsPaths.ShinyIcon == null)
                        {
                            outputLines.Add(
                                line
                                .Replace("gMonIcon_", "gMonShinyIcon_")
                                .Replace(".4bpp", "_shiny.4bpp")
                            );
                        }
                    }
                }

                if(dataSrcLines.Count != outputLines.Count)
                    File.WriteAllLines(dataSrcFile, outputLines);
            }
        }
    }
}
