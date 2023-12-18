using AutoCoordinator.GameConsole;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace AutoCoordinator.Game.Tests
{
	public abstract class PokemonBattleTest : PokemonTest
	{
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

							game.SetAutomationFlag(PokemonGame.AutomationFlag.Trainer_ForceCompMovesets, true);
							game.SetAutomationFlag(PokemonGame.AutomationFlag.Trainer_DisablePartyGeneration, false);
							game.SetAutomationFlag(PokemonGame.AutomationFlag.Trainer_RandomAI, true);
							game.SetAutomationFlag(PokemonGame.AutomationFlag.Player_AutoPickMoves, true);
							game.SetAutomationFlag(PokemonGame.AutomationFlag.Trainer_ForceLevel5, false);

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
