using RogueAssistantNET.Game;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantNET
{
	public class Server
	{
		private readonly string m_RogueHandshake1 = "3to8UEaoManH7wB4lKlLRgywSHHKmI0g";
		private readonly string m_RogueHandshake2 = "Em68TrzBAFlyhBCOm4XQIjGWbdNhuplY";

		private int m_TargetPort = 30150;
		private TcpListener m_Server = null;

		public Server()
		{
		}

		public void Run()
		{
			Console.WriteLine($"Listening to Port:{m_TargetPort}");
			m_Server = new TcpListener(IPAddress.Parse("127.0.0.1"), m_TargetPort);

			m_Server.Start();

			while(true)
			{
				using (TcpClient client = m_Server.AcceptTcpClient())
				{
					//client.ReceiveTimeout = 30 * 1000;
					//client.SendTimeout = 30 * 1000;

					if (!TryAccept(client))
					{
						Console.WriteLine($"Connection invalid..");
						client.Close();
						continue;
					}

					Console.WriteLine($"Connection accepted!");
					GameConnection conn = new GameConnection(client);

					MainLoop(conn);
				}
			}
		}

		private void MainLoop(GameConnection conn)
		{
			while (true)
			{
				conn.Update();
				Thread.Sleep(1000 / 60);
			}
		}

		private bool TryAccept(TcpClient client)
		{
			Console.WriteLine($"Checking incoming connection...");
			byte[] buffer = new byte[4096];


			// Check first handshake
			int readCount = client.Client.Receive(buffer);
			string handshake1 = Encoding.ASCII.GetString(buffer, 0, readCount);

			if (handshake1 != m_RogueHandshake1)
				return false;

			// Send 2nd continue signal
			int count = Encoding.ASCII.GetBytes("con", 0, 3, buffer, 0);
			client.Client.Send(buffer, 0, count, SocketFlags.None);

			readCount = client.Client.Receive(buffer);
			string handshake2 = Encoding.ASCII.GetString(buffer, 0, readCount);

			if (handshake2 != m_RogueHandshake2)
				return false;

			return true;
		}
	}
}
