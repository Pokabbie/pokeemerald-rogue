using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataRemover
{
	public class Program
	{
		public static readonly string c_RootDirectory = Path.GetFullPath("..\\..\\..\\..\\..\\..");

		static void Main(string[] args)
		{
			//Console.WriteLine("1 - Vanilla");
			//Console.WriteLine("2 - EX");
			//bool isVanillaVersion = ReadOption(1, 2) == 1;

			Console.WriteLine("1 - Delete Single Map");
			Console.WriteLine("2 - Delete Non Rogue Maps");
			int action = ReadOption(1, 2);

			switch (action)
			{
				case 1:
					Console.WriteLine("==Deleting Single Map==");

					Console.WriteLine("Name of map:");
					string mapName = Console.ReadLine();

					MapDeleter.Setup();
					MapDeleter.DeleteSingleMap(mapName);
					MapDeleter.Shutdown();
					break;

				case 2:
					Console.WriteLine("==Deleting Multi Map==");

					MapDeleter.Setup();
					MapDeleter.DeleteNonRogueMaps();
					MapDeleter.Shutdown();
					break;
			}


			Console.WriteLine("Press any key to exit...");
			Console.ReadKey();
		}

		private static int ReadOption(int min, int max)
		{
			do
			{
				Console.WriteLine("Select an option:");
				string raw = Console.ReadLine();

				if (int.TryParse(raw, out int result))
				{
					if (result >= min && result <= max)
						return result;
				}
			}
			while (true);
		}

		private static int ReadNumber(string prompt, int min, int max)
		{
			do
			{
				Console.WriteLine(prompt);
				string raw = Console.ReadLine();

				if (int.TryParse(raw, out int result))
				{
					if (result >= min && result <= max)
						return result;
				}
			}
			while (true);
		}

		private static bool ReadBool(string prompt)
		{
			do
			{
				Console.WriteLine(prompt);
				string raw = Console.ReadLine();

				switch (raw.ToLower())
				{
					case "y":
					case "yes":
					case "t":
					case "true":
					case "1":
						return true;
					case "n":
					case "no":
					case "f":
					case "false":
					case "0":
						return false;
				}
			}
			while (true);
		}
	}
}
