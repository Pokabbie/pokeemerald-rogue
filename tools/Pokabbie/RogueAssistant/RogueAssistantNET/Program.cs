using RogueAssistantNET.Assistant;
using RogueAssistantNET.Assistant.Behaviours;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET
{
	class Program
	{
		static void Main(string[] args)
		{
			RogueAssistantController controller = new RogueAssistantController();
			controller.OnConnect += OnConnect;

			controller.Run();
		}

		private static bool s_IsHostActive = false;

		private static void OnConnect(RogueAssistant assistant)
		{
			if (!s_IsHostActive)
			{
				s_IsHostActive = true;
				assistant.AddBehaviour(new MultiplayerServerBehaviour(20010));
			}
			else
			{
				assistant.AddBehaviour(new MultiplayerClientBehaviour("localhost", 20010));
			}
		}
	}
}
