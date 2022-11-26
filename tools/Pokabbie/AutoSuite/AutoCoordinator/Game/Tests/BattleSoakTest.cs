using AutoCoordinator.GameConsole;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace AutoCoordinator.Game.Tests
{
	public class BattleSoakTest : PokemonBattleTest
	{
		public BattleSoakTest() : base("1v1 Soak Test")
		{
		}

		protected override void ConfigureInitialRun(PokemonGame game)
		{
		}

		protected override void ConfigureTest(PokemonGame game)
		{
			game.ClearPlayerParty();
			game.ClearEnemyParty();

			int difficultyLevel = RNG.Next(0, 13);
			LogTestMessage($"Difficulty level set to {difficultyLevel}");
			game.SetRunDifficulty(difficultyLevel);

			int weatherNum = RNG.Next(0, 17);
			LogTestMessage($"Weather set to {weatherNum}");
			game.SetWeather(weatherNum);

			// Use Roxanne's ID so we get boss style generation
			int teamSize = 1;
			game.GeneratePlayerParty(265, teamSize);
			game.GenerateEnemyParty(265, teamSize);

			LogPlayerPartyInfo(game, teamSize);
			LogEnemyPartyInfo(game, teamSize);
		}
	}
}
