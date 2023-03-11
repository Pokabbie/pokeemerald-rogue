using RogueAssistantNET.Assistant.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant
{
	public class RogueAssistantController
	{
		public delegate void AssistantCallback(RogueAssistant assistant);

		private TcpListenerServer m_Server;
		private List<RogueAssistant> m_ActiveAssistants = new List<RogueAssistant>();

		private event AssistantCallback m_OnConnect;

		public RogueAssistantController()
		{
		}

		public AssistantCallback OnConnect
		{
			get => m_OnConnect;
			set => m_OnConnect = value;
		}

		public void Run()
		{
			Console.WriteLine("Entering main loop..");

			m_Server = new TcpListenerServer(IPAddress.Loopback, 30150);
			m_Server.Open();

			while (true)
			{
				Update();
				Thread.Sleep(1000 / 60);
			}

			//Console.WriteLine("Exiting main loop");
			//m_Server.Close();
		}

		private void Update()
		{
			if(m_Server.TryAccept(out TcpClient client))
			{
				if(RogueAssistant.TryAccept(client, out RogueAssistant assistant))
				{
					m_ActiveAssistants.Add(assistant);
					m_OnConnect.Invoke(assistant);
				}
				else
				{
					client.Close();
				}
			}

			foreach(var assistant in m_ActiveAssistants)
			{
				assistant.Update();
			}
		}
	}
}
