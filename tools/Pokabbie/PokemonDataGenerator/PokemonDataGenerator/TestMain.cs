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
        private static readonly string c_LatestDir = "D:\\Dev\\Pokemon\\GBA\\Other Sources\\pokeemerald-expansion-rh-hideout\\latest";

        public static void Run()
        {
            Console.WriteLine("Default species name (Case sensitive):");

            string speciesName = Console.ReadLine().Trim();
            string speciesKeyword = "SPECIES_" + speciesName.ToUpper();
            string megaKeyword = speciesKeyword + "_MEGA";

            // graphics
            {
                string dstFolder = Path.Combine(GameDataHelpers.RootDirectory, $"graphics\\pokemon\\{speciesName.ToLower()}", "mega");
                string srcFolder = Path.Combine(c_LatestDir, $"graphics\\pokemon\\{speciesName.ToLower()}\\mega");

                if(Directory.Exists(dstFolder))
                {
                    Directory.Delete(dstFolder, true);
                }

                Directory.CreateDirectory(dstFolder);

                foreach (string srcFile in Directory.EnumerateFiles(srcFolder, "*.*"))
                {
                    File.Copy(srcFile, Path.Combine(dstFolder, Path.GetFileName(srcFile)));
                }
            }

            // pokemon.h
            {
                int replaceCount = 0;
                string pokemonHeader = Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\graphics\\pokemon.h");

                StringWriter output = new StringWriter();

                foreach(string line in File.ReadLines(pokemonHeader))
                {
                    output.WriteLine(line);

                    if (line.Contains($"const u8 gMonFootprint_{speciesName}"))
                    {
                        output.WriteLine($"");
                        output.WriteLine($"    const u32 gMonFrontPic_{speciesName}Mega[] = INCBIN_U32(\"graphics/pokemon/{speciesName.ToLower()}/mega/front.4bpp.lz\");");
                        output.WriteLine($"    const u32 gMonPalette_{speciesName}Mega[] = INCBIN_U32(\"graphics/pokemon/{speciesName.ToLower()}/mega/normal.gbapal.lz\");");
                        output.WriteLine($"    const u32 gMonBackPic_{speciesName}Mega[] = INCBIN_U32(\"graphics/pokemon/{speciesName.ToLower()}/mega/back.4bpp.lz\");");
                        output.WriteLine($"    const u32 gMonShinyPalette_{speciesName}Mega[] = INCBIN_U32(\"graphics/pokemon/{speciesName.ToLower()}/mega/shiny.gbapal.lz\");");
                        output.WriteLine($"    const u8 gMonIcon_{speciesName}Mega[] = INCBIN_U8(\"graphics/pokemon/{speciesName.ToLower()}/mega/icon.4bpp\");");
                        replaceCount++;
                    }
                }

                if (replaceCount != 1)
                    throw new Exception("Failed");

                File.WriteAllText(pokemonHeader, output.ToString());
            }

            // front_pic_anims.h
            {
                int replaceCount = 0;
                string frontPicAnims = Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\pokemon_graphics\\front_pic_anims.h");

                StringWriter output = new StringWriter();
                bool readingAnims = false;

                foreach (string line in File.ReadLines(frontPicAnims))
                {
                    if (line.Contains($"static const union AnimCmd sAnim_{speciesName}") || line.Contains($"PLACEHOLDER_ANIM_SINGLE_FRAME({speciesName})"))
                    {
                        output.WriteLine(line);
                        readingAnims = true;
                    }
                    else if(readingAnims && line.Contains($"#endif"))
                    {
                        output.WriteLine();
                        output.WriteLine($"PLACEHOLDER_ANIM_SINGLE_FRAME({speciesName}Mega);");
                        output.WriteLine();
                        output.WriteLine(line);

                        readingAnims = false;
                        replaceCount++;
                    }
                    else if(line.Contains($"SINGLE_ANIMATION({speciesName})"))
                    {
                        output.WriteLine(line);
                        output.WriteLine(line.Replace(speciesName, speciesName + "Mega"));
                        replaceCount++;
                    }
                    else
                    {
                        output.WriteLine(line);
                    }
                }

                if (replaceCount != 2)
                    throw new Exception("Failed");

                File.WriteAllText(frontPicAnims, output.ToString());
            }

            // species_info
            {
                string srcFolder = Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\pokemon\\species_info");
                string latestFolder = Path.Combine(c_LatestDir, "src\\data\\pokemon\\species_info");

                foreach(string genFile in Directory.EnumerateFiles(srcFolder, "gen_*.h"))
                {
                    StringWriter output = new StringWriter();
                    StringWriter megaOutput = new StringWriter();
                    bool readingSpeciesInfo = false;
                    bool megaFound = false;

                    foreach (string line in File.ReadLines(genFile))
                    {
                        if (!readingSpeciesInfo)
                        {
                            output.WriteLine(line);

                            if (megaFound)
                                continue;

                            if (line.Contains($"[{speciesKeyword}]"))
                            {
                                readingSpeciesInfo = true;
                                megaFound = true;
                                megaOutput.WriteLine(line.Replace(speciesKeyword, megaKeyword));
                            }
                        }
                        else
                        {
                            if(line.Trim() == "},")
                            {
                                output.WriteLine($"        .formSpeciesIdTable = s{speciesName}FormSpeciesIdTable,");
                                output.WriteLine($"        .formChangeTable = s{speciesName}FormChangeTable,");
                                output.WriteLine(line);

                                megaOutput.WriteLine($"        .formSpeciesIdTable = s{speciesName}FormSpeciesIdTable,");
                                megaOutput.WriteLine($"        .formChangeTable = s{speciesName}FormChangeTable,");
                                megaOutput.WriteLine($"        .isMegaEvolution = TRUE,");
                                megaOutput.WriteLine(line);

                                readingSpeciesInfo = false;

                                output.WriteLine();
                                output.WriteLine(megaOutput.ToString());
                            }
                            else
                            {
                                output.WriteLine(line);

                                megaOutput.WriteLine(CorrectMegaDataLine(speciesName, speciesKeyword, megaKeyword, line, Path.Combine(latestFolder, Path.GetFileNameWithoutExtension(genFile) + "_families.h")));
                            }
                        }
                    }

                    if(megaFound)
                    {
                        File.WriteAllText(genFile, output.ToString());
                        break;
                    }
                }

            }

            return;
        }

        private static string CorrectMegaDataLine(string speciesName, string speciesKeyword, string megaKeyword, string srcLine, string latestFile)
        {
            bool readingSpeciesInfo = false;
            bool megaFound = false;

            if (srcLine.Contains("=") || srcLine.Contains("_PIC("))
            {
                string lineStart = srcLine.Contains("=") ? srcLine.Split('=')[0] : srcLine.Split('(')[0];

                if (
                    lineStart.Contains("baseHP") ||
                    lineStart.Contains("baseAttack") ||
                    lineStart.Contains("baseDefense") ||
                    lineStart.Contains("baseSpeed") ||
                    lineStart.Contains("baseSpAttack") ||
                    lineStart.Contains("baseSpDefense") ||
                    lineStart.Contains("types") ||
                    lineStart.Contains("abilities") ||
                    lineStart.Contains("frontPicYOffset") ||
                    lineStart.Contains("backPicYOffset") ||
                    lineStart.Contains("frontAnimFrames") ||
                    lineStart.Contains("FRONT_PIC") ||
                    lineStart.Contains("BACK_PIC")
                    )
                {
                    if (srcLine.Contains(speciesName + ",") || srcLine.Contains("_" + speciesName))
                    {
                        if(srcLine.Contains("_PIC("))
                        {
                            return srcLine.Split(',')[0].Replace(speciesName, speciesName + "Mega") + ", 64, 64),";
                        }

                        return srcLine.Replace(speciesName, speciesName + "Mega");
                    }

                    foreach (string latestLine in File.ReadLines(latestFile))
                    {
                        if (!readingSpeciesInfo)
                        {
                            if (megaFound)
                                continue;

                            if (latestLine.Contains($"[{megaKeyword}]"))
                            {
                                readingSpeciesInfo = true;
                                megaFound = true;
                            }
                        }
                        else
                        {
                            if (latestLine.Trim() == "},")
                            {
                                break;
                            }
                            else
                            {
                                if (latestLine.StartsWith(lineStart))
                                {
                                    if(latestLine.Contains("MON_TYPES("))
                                    {
                                        return latestLine.Replace("MON_TYPES", "").Replace("(", "{ ").Replace(")", " }");
                                    }

                                    return latestLine;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (srcLine.Contains(speciesName))
                {
                    return srcLine.Replace(speciesName, speciesName + "Mega");
                }
            }

            return srcLine;
        }
    }
}
