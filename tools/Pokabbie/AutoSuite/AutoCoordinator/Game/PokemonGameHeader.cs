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
		public uint MonSpeciesNamesAddr;
		public uint MoveNamesAddr;
		public uint NationalDexCount;
		public uint AutomationHeaderAddr;
		public uint ItemTableAddr;

		public static bool TryParseInternal(ConsoleConnection connection, bool applyExCheck, ref PokemonGameHeader header)
		{
			header = default;

			const uint romAddr = 0x08000000;
			const uint gfHeaderAddr = romAddr + 0x100;
			uint offsetHeaderAddr = applyExCheck ? gfHeaderAddr - 4 : gfHeaderAddr;

			header.GameVersion = connection.Cmd_Emu_Read32(gfHeaderAddr + 0);
			header.GameLanguage = connection.Cmd_Emu_Read32(gfHeaderAddr + 4);
			header.GameName = connection.Cmd_Emu_ReadRange(gfHeaderAddr + 8, 32);

			header.MonSpeciesNamesAddr = connection.Cmd_Emu_Read32(gfHeaderAddr + 68);
			header.MoveNamesAddr = connection.Cmd_Emu_Read32(gfHeaderAddr + 72);

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

			header.ItemTableAddr = connection.Cmd_Emu_Read32(offsetHeaderAddr + 200);

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
