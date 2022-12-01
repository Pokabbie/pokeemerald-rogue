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

		private DateTime m_TestStartTime = DateTime.UtcNow;
		private Random m_RNG = new Random();
		private bool m_TestActive = false;

		private StringBuilder m_LocalLog = new StringBuilder();

		public PokemonTest(string testName)
		{
			m_TestName = testName;
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
			m_LocalLog.Clear();
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
			m_LocalLog.AppendLine(msg);
		}

		public void LogTestSuccess()
		{
			LogTestMessage($"Finished successfully (ID:{CurrentTestID})");
			m_TestActive = false;

			//FlushLocalLogToFile("testOutput.txt");
		}

		public void LogTestFail(string errorMessage)
		{
			LogTestMessage($"Failed (ID:{CurrentTestID})");
			LogTestMessage("Error: " + errorMessage);
			m_TestActive = false;

			FlushLocalLogToFile();
		}

		private void FlushLocalLogToFile()
		{
			string path = $"testOutput({m_TestName}).txt";

			using (var stream = File.AppendText(path))
				stream.WriteLine(m_LocalLog.ToString());

			m_LocalLog.Clear();
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
