using AutoCoordinator.Game;
using AutoCoordinator.Game.Tests;
using System;

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

			PokemonTest test = new BattleSoakTest();
			test.Run(game);

			return;
		}
	}
}
