using RogueAssistantNET.Assistant.Behaviours;
using RogueAssistantNET.Assistant;
using RogueAssistantNET.Game;
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
			controller.OnUpdate += OnUpdate;

			controller.Run();
		}

		private static void OnUpdate(RogueAssistant assistant)
		{
			if (assistant.State.RequestState == GameRequestState.MultiplayerHost)
			{
				if (!assistant.HasBehaviour<MultiplayerServerBehaviour>())
				{
					Console.WriteLine("== Multiplayer [HOST] ==");
					Console.Write($"Desired port (default: {MultiplayerServerBehaviour.c_DefaultPort}): ");

					if (!int.TryParse(Console.ReadLine(), out int port))
					{
						port = MultiplayerServerBehaviour.c_DefaultPort;
					}

					assistant.AddBehaviour(new MultiplayerServerBehaviour(port));
				}
			}
			else if (assistant.State.RequestState == GameRequestState.MultiplayerJoin)
			{
				if (!assistant.HasBehaviour<MultiplayerClientBehaviour>())
				{
					Console.WriteLine("== Multiplayer [JOIN] ==");
					Console.Write($"Host address: ");

					string address = Console.ReadLine() ?? "";
					assistant.AddBehaviour(new MultiplayerClientBehaviour(address));
				}
			}
		}
	}
}
