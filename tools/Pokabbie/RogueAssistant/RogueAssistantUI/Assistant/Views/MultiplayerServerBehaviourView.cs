using ImGuiNET;
using RogueAssistantNET.Assistant;
using RogueAssistantNET.Assistant.Behaviours;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantUI.Assistant.Views
{
	public class MultiplayerServerBehaviourView : IRogueAssistantView
	{
		public string GetViewName(RogueAssistant assistant)
		{
			return "Multiplayer " + (assistant.HasBehaviour<MultiplayerServerBehaviour>() ? "[HOST]" : "[CLIENT]");
		}

		public bool IsViewVisible(RogueAssistant assistant)
		{
			return assistant.HasBehaviour<MultiplayerServerBehaviour>() || assistant.HasBehaviour<MultiplayerClientBehaviour>();
		}

        public void SubmitUI(RogueAssistant assistant)
        {
            var server = assistant.FindBehaviour<MultiplayerServerBehaviour>();
            var client = assistant.FindBehaviour<MultiplayerClientBehaviour>();
            var syncPlayers = assistant.FindBehaviour<NetPlayerSyncBehaviour>();

			if(server != null)
				ImGui.Text($"Hosting on port:{server.Port}");

            if (client != null)
                ImGui.Text($"Connected to Host");

			if (syncPlayers != null)
			{
				if (ImGui.BeginTable("Players###player_data", 3))
                {
                    ImGui.TableSetupColumn("ID", ImGuiTableColumnFlags.None, 1);
                    ImGui.TableSetupColumn("Name", ImGuiTableColumnFlags.None, 5);
                    ImGui.TableSetupColumn("Location", ImGuiTableColumnFlags.None, 5);
                    ImGui.TableHeadersRow();


                    List<NetPlayerData> playerData = new List<NetPlayerData>();
                    playerData.Add(syncPlayers.LocalPlayer);
                    playerData.AddRange(syncPlayers.OnlinePlayerData);

					for (int i = 0; i < playerData.Count; ++i)
                    {
                        ImGui.TableNextRow();
                        ImGui.TableNextColumn();

                        ImGui.Text(i.ToString());

                        ImGui.TableNextColumn();
						ImGui.Text(playerData[i].PlayerName);

                        ImGui.TableNextColumn();
                        ImGui.Text(playerData[i].PosX + ", " + playerData[i].PosY);

                    }

					ImGui.EndTable();
				}
			}
        }

        public void SubmitSecondaryUI(RogueAssistant assistant)
		{
		}
	}
}
