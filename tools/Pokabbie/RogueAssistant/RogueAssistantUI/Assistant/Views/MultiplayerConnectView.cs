using RogueAssistantNET.Assistant.Behaviours;
using RogueAssistantNET.Assistant;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Numerics;

namespace RogueAssistantUI.Assistant.Views
{
	public class MultiplayerConnectView : MultiplayerPlayersView
	{
		private string m_AddressWithPort = "localhost:" + MultiplayerServerBehaviour.c_DefaultPort;
		private bool m_WasSuccessful = true;

		public string DesiredHostname
		{
			get
			{
				return m_AddressWithPort.Split(':')[0];
			}
		}

		public int DesiredPort
		{
			get
			{
				var parts = m_AddressWithPort.Split(':');

				if (parts.Length == 2 && int.TryParse(parts[1], out int val))
				{
					return val;
				}

				return 0;
			}
		}

		public override string GetViewName(RogueAssistant assistant)
		{
			return "Multiplayer [CONNECT]";
		}

		public override bool IsViewVisible(RogueAssistant assistant)
		{
			return !assistant.HasBehaviour<MultiplayerServerBehaviour>();
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
					assistant.AddBehaviour(new MultiplayerClientBehaviour(DesiredHostname, DesiredPort));
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
