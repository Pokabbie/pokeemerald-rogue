using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace AutoCoordinator.GameConsole
{
	public enum CommunicationStatus
	{
		Success,
		Error
	}

	public class ConsoleCommunication
	{
		private IPEndPoint m_EndPoint;
		private TcpClient m_Client = null;
		private byte[] m_ReadBuffer = new byte[4096];

		public ConsoleCommunication(IPEndPoint endPoint)
		{
			m_EndPoint = endPoint;
		}

		private TcpClient GetValidClient()
		{
			if (m_Client == null || !m_Client.Connected)
			{
				m_Client = new TcpClient();
				m_Client.Connect(m_EndPoint);
			}

			return m_Client;
		}

		public CommunicationStatus TrySendIgnoredMessage(params string[] inArgs)
		{
			string msg = string.Join(";", inArgs);

			while (true)
			{
				try
				{
					TcpClient client = GetValidClient();
					client.GetStream().Write(Encoding.ASCII.GetBytes(msg));
					break;
				}
				catch (Exception e)
				{
					Console.WriteLine($"Exception for cmd '{inArgs[0]}': {e.Message}");
					Thread.Sleep(10000);
				}
			}

			return CommunicationStatus.Success;
		}

		public CommunicationStatus TrySendMessage(out string outMessage, params string[] inArgs)
		{
			string msg = string.Join(";", inArgs);
			int readCount;

			while (true)
			{
				try
				{
					TcpClient client = GetValidClient();
					client.GetStream().Write(Encoding.ASCII.GetBytes(msg));

					readCount = client.GetStream().Read(m_ReadBuffer);
					if (readCount == 0)
					{
						outMessage = null;
						return CommunicationStatus.Error;
					}

					break;
				}
				catch (Exception e)
				{
					Console.WriteLine($"Exception for cmd '{inArgs[0]}': {e.Message}");
					Thread.Sleep(10000);
				}
			}

			outMessage = Encoding.ASCII.GetString(m_ReadBuffer.Take(readCount).ToArray());
			return CommunicationStatus.Success;
		}

		public string SendMessage(params string[] inArgs)
		{
			CommunicationStatus status = TrySendMessage(out string outMsg, inArgs);

			if (status == CommunicationStatus.Error)
				Console.WriteLine($"Error for cmd '{inArgs[0]}'");

			return outMsg;
		}
	}
}
