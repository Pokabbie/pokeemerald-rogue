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

		private StringBuilder m_LocalLog = new StringBuilder();

		public PokemonTest(string testName)
		{
			m_TestName = testName;
		}

		public abstract void Run(PokemonGame game);

		public int CurrentTestID
		{
			get => m_CurrentTestID;
		}

		public TimeSpan CurrentTestDuration
		{
			get => DateTime.UtcNow - m_TestStartTime;
		}

		public Random RNG
		{
			get => m_RNG;
		}

		public void StartNextTest()
		{
			m_LocalLog.Clear();
			m_CurrentTestID = m_TestIdCounter++;
			m_TestStartTime = DateTime.UtcNow;

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

			//FlushLocalLogToFile("testOutput.txt");
		}

		public void LogTestFail(string errorMessage)
		{
			LogTestMessage($"Failed (ID:{CurrentTestID})");
			LogTestMessage("Error: " + errorMessage);

			FlushLocalLogToFile("testOutput.txt");
		}

		private void FlushLocalLogToFile(string path)
		{
			using (var stream = File.AppendText(path))
				stream.WriteLine(m_LocalLog.ToString());

			m_LocalLog.Clear();
		}
	}
}
