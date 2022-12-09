using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace AutoCoordinator.Game.Tests
{
	public abstract class PokemonTest
	{
		private string m_TestName;
		private int m_TestIdCounter;
		private int m_CurrentTestID;
		private int m_CurrentSaveStateSlot;

		private DateTime m_TestStartTime = DateTime.UtcNow;
		private Random m_RNG = new Random();
		private bool m_TestActive = false;
		private StreamWriter m_LogStream = null;

		public PokemonTest(string testName)
		{
			m_TestName = testName;
		}

		public string ScratchDir
		{
			get
			{
				string path = Path.GetFullPath($"Test({m_TestName})\\Scratch");
				return path;
			}
		}

		public abstract void Run(PokemonGame game);

		public string TestName
		{
			get => m_TestName;
		}

		public int CurrentTestID
		{
			get => m_CurrentTestID;
		}

		public TimeSpan CurrentTestDuration
		{
			get => m_TestActive ? DateTime.UtcNow - m_TestStartTime : TimeSpan.FromSeconds(0);
		}

		public Random RNG
		{
			get => m_RNG;
		}

		public bool IsTestActive
		{
			get => m_TestActive;
		}

		public void StartNextTest()
		{
			ResetScratch();

			m_CurrentTestID = m_TestIdCounter++;
			m_TestStartTime = DateTime.UtcNow;
			m_TestActive = true;

			LogTestMessage("========================================");
			LogTestMessage($"Starting '{m_TestName}' (ID:{CurrentTestID})");
		}

		public void LogTestMessage(string msg)
		{
			msg = $"[{DateTime.UtcNow.TimeOfDay.ToString()}] {msg}";

			Console.WriteLine(msg);

			if (m_LogStream != null)
				m_LogStream.WriteLine(msg);
		}

		public void LogTestSuccess()
		{
			LogTestMessage($"Finished successfully (ID:{CurrentTestID})");
			m_TestActive = false;
		}

		private void ResetScratch()
		{
			if (m_LogStream != null)
			{
				m_LogStream.Dispose();
				m_LogStream = null;
			}

			string dir = ScratchDir;

			if (Directory.Exists(dir))
				Directory.Delete(dir, true);

			Directory.CreateDirectory(dir);

			string logFilePath = Path.Combine(ScratchDir, "output.txt");
			m_LogStream = new StreamWriter(File.Create(logFilePath));
			m_LogStream.AutoFlush = true;

			LogTestMessage($"Reset testing scratch '{dir}'");
		}

		private void CopyScratchDir(string newDir)
		{
			if (m_LogStream != null)
			{
				m_LogStream.Dispose();
				m_LogStream = null;
			}

			string oldDir = ScratchDir;

			foreach (string file in Directory.EnumerateFiles(oldDir, "*.*", SearchOption.AllDirectories))
			{
				string copyFile = file.Replace(oldDir, newDir, StringComparison.CurrentCultureIgnoreCase);
				Directory.CreateDirectory(Path.GetDirectoryName(copyFile));
				File.Copy(file, copyFile);
			}
		}

		public void ClearSaveStates(PokemonGame game)
		{
			LogTestMessage($"Clearing save states");
			game.Connection.Cmd_Emu_SaveStateSlot(1);
			game.Connection.Cmd_Emu_SaveStateSlot(2);
			game.Connection.Cmd_Emu_SaveStateSlot(3);
			game.Connection.Cmd_Emu_SaveStateSlot(4);
			game.Connection.Cmd_Emu_SaveStateSlot(5);
			game.Connection.Cmd_Emu_SaveStateSlot(6);
			game.Connection.Cmd_Emu_SaveStateSlot(7);
			game.Connection.Cmd_Emu_SaveStateSlot(8);
			game.Connection.Cmd_Emu_SaveStateSlot(9);
			m_CurrentSaveStateSlot = 0;
		}

		public void PushSaveState(PokemonGame game, string id)
		{
			int slot = 1 + m_CurrentSaveStateSlot;
			LogTestMessage($"Saving to state {slot}");
			game.Connection.Cmd_Emu_SaveStateSlot(slot);
			m_CurrentSaveStateSlot = (m_CurrentSaveStateSlot + 1) % 9;

			// More interested in these files than the above
			string savePath = Path.Combine(ScratchDir, $"{id}.ss0");
			LogTestMessage($"Saving to state {savePath}");
			game.Connection.Cmd_Emu_SaveStateFile(savePath);
		}

		public void LogTestFail(string errorMessage)
		{
			LogTestMessage($"Failed (ID:{CurrentTestID})");
			LogTestMessage("Error: " + errorMessage);
			m_TestActive = false;

			string crashDir = Path.Combine(Path.GetDirectoryName(ScratchDir), $"Fail_{CurrentTestID}_{DateTime.Now.ToString("yyyy_MM_dd_HH_mm_ss")}");

			CopyScratchDir(crashDir);
		}

		public int CalculatePlayerPartySize(PokemonGame game)
		{
			int partySize = 0;

			for (int i = 0; i < 6; ++i)
			{
				if (game.GetPlayerMonData(i, PokemonDataID.Species) != 0)
					++partySize;
				else
					break;
			}

			return partySize;
		}

		public int CalculateEnemyPartySize(PokemonGame game)
		{
			int partySize = 0;

			for (int i = 0; i < 6; ++i)
			{
				if (game.GetEnemyMonData(i, PokemonDataID.Species) != 0)
					++partySize;
				else
					break;
			}

			return partySize;
		}

		protected void LogPlayerPartyInfo(PokemonGame game, int partySize)
		{
			LogTestMessage($"== Player Party (size:{partySize}) ==");
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
			LogTestMessage($"== Enemy Party (size:{partySize}) ==");
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
	}
}
