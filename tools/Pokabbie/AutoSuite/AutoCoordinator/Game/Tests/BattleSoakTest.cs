using AutoCoordinator.GameConsole;
using System;
using System.Collections.Generic;
using System.Text;

namespace AutoCoordinator.Game.Tests
{
	public class BattleSoakTest : PokemonTest
	{
		private Random m_RNG = new Random();
		private int m_SpeciesCount;
		private int m_CurrentLevel;

		public override void Run(PokemonGame game)
		{
			m_SpeciesCount = game.GetNumSpecies();
			RunBattleSoakLoop(game);
		}

		private void SetupPlayerMon(PokemonGame game, int slot)
		{
			int species = m_RNG.Next(1, m_SpeciesCount);
			int fixedIvs = m_RNG.Next(0, 32);
			game.SetPlayerMon(slot, species, m_CurrentLevel, fixedIvs);
			game.ApplyRandomPlayerMonPreset(slot);
		}

		private void SetupEnemyMon(PokemonGame game, int slot)
		{
			int species = m_RNG.Next(1, m_SpeciesCount);
			int fixedIvs = m_RNG.Next(0, 32);
			game.SetEnemyMon(slot, species, m_CurrentLevel, fixedIvs);
			game.ApplyRandomEnemyMonPreset(slot);
		}

		private void RunBattleSoakLoop(PokemonGame game)
		{
			int internalState = -1;

			while (true)
			{
				PokemonGame.GameInputState inputState = game.GetInputState();

				if (internalState == -1)
				{
					internalState = 0;
					m_CurrentLevel = m_RNG.Next(1, 10) * 10 + 5;

					if (m_CurrentLevel > 100)
						m_CurrentLevel = 100;
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
							game.ClearPlayerParty();
							game.ClearEnemyParty();
							for (int i = 0; i < 1; ++i)
							{
								SetupPlayerMon(game, i);
								SetupEnemyMon(game, i);
							}

							game.StartTrainerBattle();
							internalState = 1;
						}
						else if(internalState == 2)
						{
							// Finished this round, so just loop
							internalState = -1;
						}
						break;
				}
			}
		}
	}
}
