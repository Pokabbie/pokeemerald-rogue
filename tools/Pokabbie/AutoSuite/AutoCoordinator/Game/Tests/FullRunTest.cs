using AutoCoordinator.GameConsole;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace AutoCoordinator.Game.Tests
{
	public class FullRunTest : PokemonTest
	{
		private class TestState
		{
			public bool m_HasAdventureStarted = false;
			public int m_CurrentDifficulty = 0;
			public int m_MovementCounter = 0;
			public PokemonMapLayoutID m_PrevMapLayoutID = PokemonMapLayoutID.Unknown;
		}

		private TestState m_State = null;

		public FullRunTest() : base("Full Run Test")
		{ 
		}

		public override void Run(PokemonGame game)
		{
			PokemonGame.GameInputState prevState = PokemonGame.GameInputState.Unknown;
			DateTime stateEnterTime = DateTime.UtcNow;

			game.ResetGame();

			while (true)
			{
				PokemonGame.GameInputState inputState = game.GetInputState();
				bool justChangedState = prevState != inputState;

				if (justChangedState)
				{
					LogTestMessage($"Switched to state '{inputState}'");
					stateEnterTime = DateTime.UtcNow;
				}

				if (IsTestActive)
				{
					TimeSpan stuckTimeout = (DateTime.UtcNow - stateEnterTime);
					if (stuckTimeout >= TimeSpan.FromMinutes(4))
						LogTestFail($"Stuck for {stuckTimeout}");
				}

				switch (inputState)
				{
					case PokemonGame.GameInputState.TitleMenu:
						Test_TitleMenu(game);
						break;

					case PokemonGame.GameInputState.Battle:
						Test_Battle(game, justChangedState);
						break;

					case PokemonGame.GameInputState.Overworld:
						Test_Overworld(game, justChangedState);
						break;
				}

				if (prevState == inputState)
					Thread.Sleep(30);

				prevState = inputState;
				Thread.Sleep(10);
			}
		}

		private void Test_TitleMenu(PokemonGame game)
		{
			if (IsTestActive && m_State != null && m_State.m_HasAdventureStarted)
			{
				LogTestMessage("Reached Title Screen");
				LogTestSuccess();
				return;
			}

			game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.A);
		}

		private void Test_Battle(PokemonGame game, bool justEnteredState)
		{
			if (justEnteredState && m_State != null)
			{
				m_State.m_CurrentDifficulty = game.GetVar(PokemonVarID.RogueDifficulty);

				LogTestMessage($"Starting Battle in Difficulty {m_State.m_CurrentDifficulty}...");
				LogPlayerPartyInfo(game, CalculatePlayerPartySize(game));
				LogEnemyPartyInfo(game, CalculateEnemyPartySize(game));
			}

			game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.A);
		}

		private void ResetDifficultySettings(PokemonGame game)
		{
			game.SetFlag(PokemonFlagID.RogueExpAll, true);
			game.SetFlag(PokemonFlagID.RogueCanOverLvl, true);
			game.SetFlag(PokemonFlagID.RogueDoubleBattles, false);
			game.SetFlag(PokemonFlagID.RogueHardTrainers, false);
			game.SetFlag(PokemonFlagID.RogueEasyTrainers, false);
			game.SetFlag(PokemonFlagID.RogueEasyItems, false);
			game.SetFlag(PokemonFlagID.RogueHardItems, false);
			game.SetFlag(PokemonFlagID.RogueEVsEnabled, true);
			game.SetFlag(PokemonFlagID.RogueGauntletMode, false);
			game.SetFlag(PokemonFlagID.RogueRainbowMode, false);
			game.SetFlag(PokemonFlagID.RogueForceBasicBag, false);

			game.SetFlag(PokemonFlagID.RogueHoennRoutes, true);
			game.SetFlag(PokemonFlagID.RogueHoennBosses, false);
			game.SetFlag(PokemonFlagID.RogueKantoRoutes, true);
			game.SetFlag(PokemonFlagID.RogueKantoBosses, false);
			game.SetFlag(PokemonFlagID.RogueJohtoRoutes, true);
			game.SetFlag(PokemonFlagID.RogueJohtoBosses, false);

			game.SetFlag(PokemonFlagID.RogueEasyLegendaries, false);
			game.SetFlag(PokemonFlagID.RogueHardLegendaries, false);

			game.SetFlag(PokemonFlagID.SeedEnabled, false);
			game.SetFlag(PokemonFlagID.SeedItems, true);
			game.SetFlag(PokemonFlagID.SeedTrainers, true);
			game.SetFlag(PokemonFlagID.SeedBosses, true);
			game.SetFlag(PokemonFlagID.SeedWildMons, true);

			game.SetVar(PokemonVarID.RogueEnabledGenLimit, 8);
			game.SetVar(PokemonVarID.RogueRegionDexLimit, 0);
		}

		private void SelectRandomSettings(PokemonGame game)
		{
			bool SetFlagPerc(PokemonFlagID flag, int perc)
			{
				if (RNG.Next(100) < perc)
				{
					LogTestMessage($"Flag '{flag}': true");
					game.SetFlag(flag, true);
					return true;
				}
				else
				{
					LogTestMessage($"Flag '{flag}': false");
					game.SetFlag(flag, false);
					return false;
				}
			}

			void SetDifficultyPerc(string id, PokemonFlagID easyFlag, PokemonFlagID hardFlag)
			{
				int rng = RNG.Next(100);
				if (rng < 33)
				{
					LogTestMessage($"Difficulty '{id}': easy");
					game.SetFlag(easyFlag, true);
					game.SetFlag(hardFlag, false);
				}
				else if (rng < 66)
				{
					LogTestMessage($"Difficulty '{id}': average");
					game.SetFlag(easyFlag, false);
					game.SetFlag(hardFlag, false);
				}
				else
				{
					LogTestMessage($"Difficulty '{id}': hard");
					game.SetFlag(easyFlag, false);
					game.SetFlag(hardFlag, true);
				}
			}

			SetFlagPerc(PokemonFlagID.RogueDoubleBattles, 10);

			if (!SetFlagPerc(PokemonFlagID.RogueRainbowMode, 40))
				SetFlagPerc(PokemonFlagID.RogueGauntletMode, 10);

			SetFlagPerc(PokemonFlagID.RogueHoennBosses, 50);
			SetFlagPerc(PokemonFlagID.RogueKantoBosses, 50);
			SetFlagPerc(PokemonFlagID.RogueJohtoBosses, 50);

			SetDifficultyPerc("Trainers", PokemonFlagID.RogueEasyTrainers, PokemonFlagID.RogueHardTrainers);
			SetDifficultyPerc("Items", PokemonFlagID.RogueEasyItems, PokemonFlagID.RogueHardItems);
			SetDifficultyPerc("Legendaries", PokemonFlagID.RogueEasyLegendaries, PokemonFlagID.RogueHardLegendaries);

			bool exVersion = game.GetFlag(PokemonFlagID.RogueExpansionActive);
			int maxGen = exVersion ? 8 : 3;

			int regionRng = RNG.Next(1, maxGen + 1);
			bool regionalDex = RNG.Next(100) < 30;

			LogTestMessage($"Using Gen Limit {regionRng} ({(regionalDex ? "regional" : "national")})");
			game.SetVar(PokemonVarID.RogueEnabledGenLimit, regionRng);
			game.SetVar(PokemonVarID.RogueRegionDexLimit, regionalDex ? regionRng : 0);

			game.SetFlag(PokemonFlagID.SeedEnabled, true);
			int seed0 = RNG.Next(0, ushort.MaxValue + 1);
			int seed1 = RNG.Next(0, ushort.MaxValue + 1);

			LogTestMessage($"Setting Rogue Seed: {seed0}, {seed1}");
			game.SetRogueSeed(seed0, seed1);
		}

		private void Test_Overworld(PokemonGame game, bool justEnteredState)
		{
			if (!IsTestActive)
			{
				StartNextTest();

				game.SetAutomationFlag(PokemonGame.AutomationFlag.Player_AutoPickMoves, true);
				game.SetAutomationFlag(PokemonGame.AutomationFlag.Trainer_ForceLevel5, true);

				m_State = null;
				game.ResetGame();
				return;
			}

			if (m_State == null)
			{
				m_State = new TestState();
				ResetDifficultySettings(game);
				SelectRandomSettings(game);

				bool exVersion = game.GetFlag(PokemonFlagID.RogueExpansionActive);
				int metagrossSpecies = exVersion ? 376 : 400;

				game.ClearPlayerParty();
				game.SetPlayerMon(0, metagrossSpecies, 100, 31);
				game.SetPlayerMonData(0, PokemonDataID.Move1, 228); // Pursuit
				game.SetPlayerMonData(0, PokemonDataID.Move2, 332); // Aerial Ace
				game.SetPlayerMonData(0, PokemonDataID.Move3, 0);
				game.SetPlayerMonData(0, PokemonDataID.Move4, 0);

				LogTestMessage("Warping to 'RogueHubTransition'");
				game.Warp(RogueMapID.RogueHubTransition, 0);

				m_State.m_HasAdventureStarted = true;
				return;
			}

			if (justEnteredState)
			{
				int difficulty = game.GetVar(PokemonVarID.RogueDifficulty);

				if (m_State.m_CurrentDifficulty != difficulty)
				{
					m_State.m_CurrentDifficulty = difficulty;
					LogTestMessage($"========= Difficulty changed {m_State.m_CurrentDifficulty} ========= ");
				}
			}

			PokemonMapLayoutID currentMapLayout = game.GetMapLayoutID();

			if (currentMapLayout != m_State.m_PrevMapLayoutID)
			{
				LogTestMessage($"Warped '{currentMapLayout}'");
				m_State.m_PrevMapLayoutID = currentMapLayout;
			}

			switch (currentMapLayout)
			{
				case PokemonMapLayoutID.RogueHubAdventureEntrance:
					if (m_State.m_HasAdventureStarted)
					{
						LogTestMessage("Back at hub enterance (Assuming must have lost run)");
						LogTestSuccess();
					}
					break;

				case PokemonMapLayoutID.RogueHubTransition:
					game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.A | ConsoleButtons.UP);
					break;

				case PokemonMapLayoutID.RogueAdventurePaths:
					game.WarpNextAdventureEncounter();
					// Randomally smash buttons
					//switch (RNG.Next(5))
					//{
					//	case 0:
					//		game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.UP, 3);
					//		break;
					//	case 1:
					//		game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.DOWN, 3);
					//		break;
					//	default:
					//		game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.RIGHT | ConsoleButtons.A, 2);
					//		break;
					//}
					break;

				case PokemonMapLayoutID.RogueBoss0:
				case PokemonMapLayoutID.RogueBoss8:
				case PokemonMapLayoutID.RogueBoss9:
				case PokemonMapLayoutID.RogueBoss10:
				case PokemonMapLayoutID.RogueBoss11:
				case PokemonMapLayoutID.RogueBoss12:
				case PokemonMapLayoutID.RogueBoss13:
					// Just enter battle
					game.Connection.Cmd_Emu_TapKeys(ConsoleButtons.UP | ConsoleButtons.A);
					break;

				default:
					// Must be in a route...
					game.Warp(RogueMapID.RogueAdventurePaths, 0);
					m_State.m_MovementCounter = 0;
					break;
			}
		}
	}
}
