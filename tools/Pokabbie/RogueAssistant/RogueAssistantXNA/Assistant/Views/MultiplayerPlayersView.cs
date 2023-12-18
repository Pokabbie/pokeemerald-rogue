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
	public abstract class MultiplayerPlayersView : IRogueAssistantView
	{
		public abstract string GetViewName(RogueAssistant assistant);

		public abstract bool IsViewVisible(RogueAssistant assistant);

		public abstract void SubmitSecondaryUI(RogueAssistant assistant);

		public virtual void SubmitUI(RogueAssistant assistant)
		{
			var syncPlayers = assistant.FindBehaviour<NetPlayerSyncBehaviour>();

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
						if (playerData[i] != null)
						{

							ImGui.TableNextRow();
							ImGui.TableNextColumn();

							ImGui.Text(i.ToString());

							ImGui.TableNextColumn();
							ImGui.Text(playerData[i].PlayerName);

							ImGui.TableNextColumn();
							ImGui.Text(playerData[i].PosX + ", " + playerData[i].PosY);
						}
					}

					ImGui.EndTable();
				}
			}
		}
	}
}
