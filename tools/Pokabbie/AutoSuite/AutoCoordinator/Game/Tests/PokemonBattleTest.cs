using AutoCoordinator.GameConsole;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace AutoCoordinator.Game.Tests
{
	public abstract class PokemonBattleTest : PokemonTest
	{
		private int m_SpeciesCount;

		public PokemonBattleTest(string testName) : base(testName)
		{
		}

		public override void Run(PokemonGame game)
		{
			game.ResetGame();

			ConfigureInitialRun(game);

			RunBattleSoakLoop(game);
		}

		protected abstract void ConfigureInitialRun(PokemonGame game);

		protected abstract void ConfigureTest(PokemonGame game);

		protected void LogPlayerPartyInfo(PokemonGame game, int partySize)
		{
			LogTestMessage($"== Player Party ==");
			for (int i = 0; i < partySize; ++i)
			{
				LogTestMessage($"=({i})=");
				LogTestMessage($"Species: {game.GetPlayerMonData(i, PokemonDataID.Species)} ({game.GetSpeciesName(game.GetPlayerMonData(i, PokemonDataID.Species))})");
				LogTestMessage($"Level: {game.GetPlayerMonData(i, PokemonDataID.Level)}");
				LogTestMessage($"HeldItem: {game.GetPlayerMonData(i, PokemonDataID.HeldItem)}");
				LogTestMessage($"AbilityNum: {game.GetPlayerMonData(i, PokemonDataID.AbilityNum)}");
				LogTestMessage($"Move1: {game.GetPlayerMonData(i, PokemonDataID.Move1)} ({game.GetMoveName(game.GetPlayerMonData(i, PokemonDataID.Move1))})");
				LogTestMessage($"Move2: {game.GetPlayerMonData(i, PokemonDataID.Move2)} ({game.GetMoveName(game.GetPlayerMonData(i, PokemonDataID.Move2))})");
				LogTestMessage($"Move3: {game.GetPlayerMonData(i, PokemonDataID.Move3)} ({game.GetMoveName(game.GetPlayerMonData(i, PokemonDataID.Move3))})");
				LogTestMessage($"Move4: {game.GetPlayerMonData(i, PokemonDataID.Move4)} ({game.GetMoveName(game.GetPlayerMonData(i, PokemonDataID.Move4))})");
			}
		}

		protected void LogEnemyPartyInfo(PokemonGame game, int partySize)
		{
			LogTestMessage($"== Enemy Party ==");
			for (int i = 0; i < partySize; ++i)
			{
				LogTestMessage($"=({i})=");
				LogTestMessage($"Species: {game.GetEnemyMonData(i, PokemonDataID.Species)} ({game.GetSpeciesName(game.GetEnemyMonData(i, PokemonDataID.Species))})");
				LogTestMessage($"Level: {game.GetEnemyMonData(i, PokemonDataID.Level)}");
				LogTestMessage($"HeldItem: {game.GetEnemyMonData(i, PokemonDataID.HeldItem)}");
				LogTestMessage($"AbilityNum: {game.GetEnemyMonData(i, PokemonDataID.AbilityNum)}");
				LogTestMessage($"Move1: {game.GetEnemyMonData(i, PokemonDataID.Move1)} ({game.GetMoveName(game.GetEnemyMonData(i, PokemonDataID.Move1))})");
				LogTestMessage($"Move2: {game.GetEnemyMonData(i, PokemonDataID.Move2)} ({game.GetMoveName(game.GetEnemyMonData(i, PokemonDataID.Move2))})");
				LogTestMessage($"Move3: {game.GetEnemyMonData(i, PokemonDataID.Move3)} ({game.GetMoveName(game.GetEnemyMonData(i, PokemonDataID.Move3))})");
				LogTestMessage($"Move4: {game.GetEnemyMonData(i, PokemonDataID.Move4)} ({game.GetMoveName(game.GetEnemyMonData(i, PokemonDataID.Move4))})");
			}
		}

		private void RunBattleSoakLoop(PokemonGame game)
		{
			int internalState = -1;

			PokemonGame.GameInputState prevState = PokemonGame.GameInputState.Unknown;

			while (true)
			{
				PokemonGame.GameInputState inputState = game.GetInputState();

				if (CurrentTestDuration >= TimeSpan.FromMinutes(5))
				{
					LogTestFail("Test timed out");
					game.ResetGame();
					internalState = -1;
					inputState = PokemonGame.GameInputState.TitleMenu;
				}

				if (internalState == -1)
				{
					internalState = 0;
				}

				switch (inputState)
				{
					case PokemonGame.GameInputState.TitleMenu:
						internalState = 0;
						game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.A);
						break;

					case PokemonGame.GameInputState.Battle:
						game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.A);
						internalState = 2;
						break;

					case PokemonGame.GameInputState.Overworld:
						if (internalState == 0)
						{
							StartNextTest();
							ConfigureTest(game);

							game.StartTrainerBattle();
							internalState = 1;
						}
						else if (internalState == 2)
						{
							// Finished this round, so just loop
							internalState = -1;
							LogTestSuccess();

							if ((CurrentTestID % 100) == 0)
							{
								LogTestMessage("Executing periodic reboot");
								game.ResetGame();
							}
						}
						break;
				}

				if (prevState == inputState)
					Thread.Sleep(30);

				prevState = inputState;
			}
		}
	}
}
