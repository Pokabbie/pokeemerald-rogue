using AutoCoordinator.Game;
using AutoCoordinator.Game.Tests;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace AutoCoordinator
{
	class Program
	{

		static void Main(string[] args)
		{
			PokemonGame game = new PokemonGame();

			if (!game.CheckConnection())
			{
				Console.Error.WriteLine("Unable to validate game connection");
				return;
			}

			List<PokemonTest> avaliableTests = new List<PokemonTest>();

			foreach (var testClass in Assembly.GetEntryAssembly().GetTypes().Where((t) => t.IsSubclassOf(typeof(PokemonTest)) && !t.IsAbstract))
			{
				PokemonTest testInstance = (PokemonTest)Activator.CreateInstance(testClass);
				avaliableTests.Add(testInstance);
			}

			Console.WriteLine($"Found {avaliableTests.Count} test types");
			for (int i = 0; i < avaliableTests.Count; ++i)
			{
				Console.WriteLine($"{i} - {avaliableTests[i].TestName}");
			}

			int selectedIndex;
			while (true)
			{
				Console.WriteLine($"Which test should be ran?");
				string input = Console.ReadLine();

				if (int.TryParse(input, out selectedIndex))
				{
					if (selectedIndex < 0 || selectedIndex >= avaliableTests.Count)
						Console.WriteLine($"Invalid Input");
					else
						break;
				}
				else
				{
					Console.WriteLine($"Invalid Input");
				}
			}

			PokemonTest test = avaliableTests[selectedIndex];
			Console.WriteLine($"Running {test.TestName}...");

			test.Run(game);

			return;
		}
	}
}
