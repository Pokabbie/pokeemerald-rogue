using RogueAssistantNET.Assistant.Behaviours;
using RogueAssistantNET.Assistant;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Numerics;
using RogueAssistantNET.Game;

namespace RogueAssistantUI.Assistant.Views
{
	public class MultiplayerConnectView : MultiplayerPlayersView
	{
		private string m_AddressWithPort = "localhost:" + MultiplayerServerBehaviour.c_DefaultPort;
		private bool m_WasSuccessful = true;

		public override string GetViewName(RogueAssistant assistant)
		{
			return "Multiplayer [CONNECT]";
		}

		public override bool IsViewVisible(RogueAssistant assistant)
		{
			return assistant.Connection.State.InternalGameState == InternalGameState.MultiplayerJoin || assistant.HasBehaviour<MultiplayerClientBehaviour>();
		}

		public override void SubmitUI(RogueAssistant assistant)
		{
			var client = assistant.FindBehaviour<MultiplayerClientBehaviour>();

			if (client == null)
			{
				ImGui.InputText("Address", ref m_AddressWithPort, 256);
				if (ImGui.Button("Connect to Host"))
				{
					m_WasSuccessful = false;
					assistant.AddBehaviour(new MultiplayerClientBehaviour(m_AddressWithPort));
				}
				else if(!m_WasSuccessful)
				{
					ImGui.TextColored(new Vector4(1, 0, 0, 1), "Unable to connect");
				}
			}
			else if (client.IsConnecting)
			{
				ImGui.Text("Attempting to connect..");
			}
			else if(!client.IsDisconnected)
			{
				m_WasSuccessful = true;
				ImGui.Text($"Connected to Host");
				ImGui.SameLine();
				if (ImGui.Button("Stop"))
				{
					assistant.RemoveBehaviour(client);
				}

				base.SubmitUI(assistant);
			}
		}

		public override void SubmitSecondaryUI(RogueAssistant assistant)
		{

		}
	}
}
