using System;
using System.Collections.Generic;
using System.Text;

namespace AutoCoordinator.Game.Tests
{
	public class BattleSoakTest_Custom : PokemonBattleTest
	{
		public BattleSoakTest_Custom() : base("Custom Soak Test")
		{
		}

		protected override void ConfigureInitialRun(PokemonGame game)
		{
		}

		protected override void ConfigureTest(PokemonGame game)
		{
			game.ClearPlayerParty();
			game.ClearEnemyParty();

			int difficultyLevel = 8;
			LogTestMessage($"Difficulty level set to {difficultyLevel}");
			game.SetRunDifficulty(difficultyLevel);

			int weatherNum = 0;
			LogTestMessage($"Weather set to {weatherNum}");
			game.SetWeather(weatherNum);

			game.SetPlayerMon(0, 373, 88, 11);
			game.SetPlayerMonData(0, PokemonDataID.HeldItem, 0);
			game.SetPlayerMonData(0, PokemonDataID.AbilityNum, 0);
			game.SetPlayerMonData(0, PokemonDataID.Move1, 742);
			game.SetPlayerMonData(0, PokemonDataID.Move2, 0);
			game.SetPlayerMonData(0, PokemonDataID.Move3, 0);
			game.SetPlayerMonData(0, PokemonDataID.Move4, 0);
			//game.SetPlayerMonData(0, PokemonDataID.Move1, 337);
			//game.SetPlayerMonData(0, PokemonDataID.Move2, 742);
			//game.SetPlayerMonData(0, PokemonDataID.Move3, 53);
			//game.SetPlayerMonData(0, PokemonDataID.Move4, 428);

			game.SetEnemyMon(0, 867, 88, 11);
			game.SetEnemyMonData(0, PokemonDataID.HeldItem, 0);
			game.SetEnemyMonData(0, PokemonDataID.AbilityNum, 0);
			game.SetEnemyMonData(0, PokemonDataID.Move1, 133);
			game.SetEnemyMonData(0, PokemonDataID.Move2, 0);
			game.SetEnemyMonData(0, PokemonDataID.Move3, 0);
			game.SetEnemyMonData(0, PokemonDataID.Move4, 0);

			LogPlayerPartyInfo(game, 1);
			LogEnemyPartyInfo(game, 1);
		}
	}
}
