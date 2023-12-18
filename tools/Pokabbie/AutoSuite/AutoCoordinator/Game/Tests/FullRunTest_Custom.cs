using System;
using System.Collections.Generic;
using System.Text;

namespace AutoCoordinator.Game.Tests
{
	public class FullRunTest_Custom : FullRunTest
	{
		public FullRunTest_Custom() : base("Custom Full Run Test")
		{ 
		}

		protected override void SelectRandomSettings(PokemonGame game)
		{
			bool SetFlagPerc(PokemonFlagID flag, bool state)
			{
				if (state)
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

			void SetDifficultyPerc(string id, PokemonFlagID easyFlag, PokemonFlagID hardFlag, int difficulty)
			{
				if (difficulty == 0)
				{
					LogTestMessage($"Difficulty '{id}': easy");
					game.SetFlag(easyFlag, true);
					game.SetFlag(hardFlag, false);
				}
				else if (difficulty == 1)
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

			const int difficulty_easy = 0;
			const int difficulty_average = 1;
			const int difficulty_hard = 2;

			SetFlagPerc(PokemonFlagID.RogueDoubleBattles, false);
			SetFlagPerc(PokemonFlagID.RogueRainbowMode, false);
			SetFlagPerc(PokemonFlagID.RogueGauntletMode, false);

			SetFlagPerc(PokemonFlagID.RogueHoennBosses, false);
			SetFlagPerc(PokemonFlagID.RogueKantoBosses, true);
			SetFlagPerc(PokemonFlagID.RogueJohtoBosses, true);


			SetDifficultyPerc("Trainers", PokemonFlagID.RogueEasyTrainers, PokemonFlagID.RogueHardTrainers, difficulty_average);
			SetDifficultyPerc("Items", PokemonFlagID.RogueEasyItems, PokemonFlagID.RogueHardItems, difficulty_easy);
			SetDifficultyPerc("Legendaries", PokemonFlagID.RogueEasyLegendaries, PokemonFlagID.RogueHardLegendaries, difficulty_average);

			int regionRng = 6;
			bool regionalDex = false;
			int seed0 = 19317;
			int seed1 = 28004;

			LogTestMessage($"Using Gen Limit {regionRng} ({(regionalDex ? "regional" : "national")})");
			game.SetVar(PokemonVarID.RogueEnabledGenLimit, regionRng);
			game.SetVar(PokemonVarID.RogueRegionDexLimit, regionalDex ? regionRng : 0);

			game.SetFlag(PokemonFlagID.SeedEnabled, true);

			LogTestMessage($"Setting Rogue Seed: {seed0}, {seed1}");
			game.SetRogueSeed(seed0, seed1);
		}
	}
}
