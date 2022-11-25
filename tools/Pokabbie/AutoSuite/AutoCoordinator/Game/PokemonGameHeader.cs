using AutoCoordinator.GameConsole;
using AutoCoordinator.Util;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace AutoCoordinator.Game
{
	/// <summary>
	/// Wrapper for a Pokemon Game header
	/// </summary>
	public struct PokemonGameHeader
	{
		public uint GameVersion;
		public uint GameLanguage;
		public byte[] GameName;
		public uint NationalDexCount;
		public uint AutomationHeaderAddr;

		public static bool TryParse(string romPath, out PokemonGameHeader header)
		{
			header = default;

			// Note: the game values are big endian
			//
			using (FileStream stream = new FileStream(romPath, FileMode.Open))
			using (BinaryReader reader = new BinaryReader(stream))
			{
				// Starting location of struct GFRomHeader sGFRomHeader
				stream.Position = 0x100;

				header.GameVersion = reader.ReadUInt32();
				header.GameLanguage = reader.ReadUInt32();
				header.GameName = reader.ReadBytes(32);

				stream.Position += 4; // monFrontPics
				stream.Position += 4; // monBackPics
				stream.Position += 4; // monNormalPalettes
				stream.Position += 4; // monShinyPalettes
				stream.Position += 4; // monIcons
				stream.Position += 4; // monIconPaletteIds
				stream.Position += 4; // monIconPalettes
				stream.Position += 4; // monSpeciesNames
				stream.Position += 4; // moveNames
				stream.Position += 4; // decorations
				stream.Position += 4; // flagsOffset
				stream.Position += 4; // varsOffset
				stream.Position += 4; // pokedexOffset
				stream.Position += 4; // seen1Offset
				stream.Position += 4; // seen2Offset
				stream.Position += 4; // pokedexVar
				stream.Position += 4; // pokedexFlag
				stream.Position += 4; // mysteryEventFlag

				header.NationalDexCount = reader.ReadUInt32();

				stream.Position += 1; // playerNameLength
				stream.Position += 1; // unk2
				stream.Position += 1; // pokemonNameLength1
				stream.Position += 1; // pokemonNameLength2

				uint handshakeCode = reader.ReadUInt32();
				if (handshakeCode != 20012) // rogueAutomationHandshake1
				{
					Console.WriteLine("Failed to parse valid 'rogue handshake 1'");
					return false;
				}

				header.AutomationHeaderAddr = reader.ReadUInt32();

				handshakeCode = reader.ReadUInt32();
				if (handshakeCode != 30035) // rogueAutomationHandshake2
				{
					Console.WriteLine("Failed to parse valid 'rogue handshake 2'");
					return false;
				}
			}

			return true;
		}


		public static bool TryParseInternal(ConsoleConnection connection, bool applyExCheck, ref PokemonGameHeader header)
		{
			header = default;

			const uint romAddr = 0x08000000;
			const uint gfHeaderAddr = romAddr + 0x100;
			uint offsetHeaderAddr = applyExCheck ? gfHeaderAddr - 4 : gfHeaderAddr;

			header.GameVersion = connection.Cmd_Emu_Read32(gfHeaderAddr + 0);
			header.GameLanguage = connection.Cmd_Emu_Read32(gfHeaderAddr + 4);
			header.GameName = connection.Cmd_Emu_ReadRange(gfHeaderAddr + 8, 32);

			header.NationalDexCount = connection.Cmd_Emu_Read32(offsetHeaderAddr + 112);

			uint handshakeCode = connection.Cmd_Emu_Read32(offsetHeaderAddr + 120);
			if (handshakeCode != 20012) // rogueAutomationHandshake1
			{
				Console.WriteLine("Failed to parse valid 'rogue handshake 1'");
				return false;
			}

			header.AutomationHeaderAddr = connection.Cmd_Emu_Read32(offsetHeaderAddr + 124);

			handshakeCode = connection.Cmd_Emu_Read32(offsetHeaderAddr + 128);
			if (handshakeCode != 30035) // rogueAutomationHandshake2
			{
				Console.WriteLine("Failed to parse valid 'rogue handshake 2'");
				return false;
			}

			return true;
		}

		public static bool TryParse(ConsoleConnection connection, out PokemonGameHeader header)
		{
			header = default;

			if (TryParseInternal(connection, false, ref header))
				return true;

			Console.WriteLine("Attempting to check for EX version instead");

			return TryParseInternal(connection, true, ref header);
		}
	}
}
