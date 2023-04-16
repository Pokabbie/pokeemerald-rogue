using ImGuiNET;
using RogueAssistantNET.Assistant;
using RogueAssistantNET.Assistant.Behaviours;
using RogueAssistantNET.Game;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantUI.Assistant.Views
{
	public class MultiplayerHostView : MultiplayerPlayersView
	{
		private int m_DesiredPort = MultiplayerServerBehaviour.c_DefaultPort;
		private bool m_WasSuccessful = true;

		public override string GetViewName(RogueAssistant assistant)
		{
			return "Multiplayer [HOST]";
		}

		public override bool IsViewVisible(RogueAssistant assistant)
		{
			return assistant.Connection.State.InternalGameState == InternalGameState.MultiplayerHost || assistant.HasBehaviour<MultiplayerServerBehaviour>();
		}

		public override void SubmitUI(RogueAssistant assistant)
		{
			var server = assistant.FindBehaviour<MultiplayerServerBehaviour>();

			if(server == null)
			{
				ImGui.InputInt("Port", ref m_DesiredPort);
				if(ImGui.Button("Host Multiplayer"))
				{
					m_WasSuccessful = false;
					assistant.AddBehaviour(new MultiplayerServerBehaviour(m_DesiredPort));
				}
				else if(!m_WasSuccessful)
				{
					ImGui.TextColored(new Vector4(1, 0, 0, 1), "Unable to Host on given port");
				}
			}
			else
			{
				m_WasSuccessful = true;
				ImGui.Text($"Hosting on port:{server.Port}");

				ImGui.SameLine();
				if(ImGui.Button("Stop"))
				{
					assistant.RemoveBehaviour(server);
				}

				base.SubmitUI(assistant);
			}
		}

		public override void SubmitSecondaryUI(RogueAssistant assistant)
		{

		}
	}
}
