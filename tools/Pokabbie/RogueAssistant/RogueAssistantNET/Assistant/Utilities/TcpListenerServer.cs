using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant.Utilities
{
	public class TcpListenerServer
	{
		private IPAddress m_Address;
		private int m_DesiredPort;
		private TcpListener m_Server = null;
		private Task<TcpClient> m_PendingClientTask = null;
		public TcpListenerServer(IPAddress ip, int desiredPort)
		{
			m_Address = ip;
			m_DesiredPort = desiredPort;
		}

		public bool Open(int portRange = -1)
		{
			// Attempt to connect to next avaliable port
			for(int i = 0; i < portRange || portRange < 0; ++i)
			{
				int port = m_DesiredPort + i;
				Console.WriteLine($"Listening to Port:{port}");
				m_Server = new TcpListener(m_Address, port);

				try
				{
					m_Server.Start();
					return true;
				}
				catch (SocketException)
				{
					//Console.WriteLine($"\tFailed to bind Port:{port} ({e.Message})");
				}
			}

			Console.WriteLine($"Failed to bind Port:{m_DesiredPort} ({portRange})");
			return false;
		}

		public void Close()
		{
			if(m_Server != null)
			{
				Console.WriteLine($"Closing Listener");
				m_Server.Stop();
				m_Server = null;
			}
		}

		public bool TryAccept(out TcpClient client)
		{
			if (m_PendingClientTask == null)
			{
				m_PendingClientTask = m_Server.AcceptTcpClientAsync();
			}

			if (m_PendingClientTask.IsCompleted)
			{
				client = m_PendingClientTask.Result;
				m_PendingClientTask = null;
				return true;
			}

			client = null;
			return false;
		}
	}
}
