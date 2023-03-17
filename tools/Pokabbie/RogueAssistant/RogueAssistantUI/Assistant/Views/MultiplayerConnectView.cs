using RogueAssistantNET.Assistant.Behaviours;
using RogueAssistantNET.Assistant;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;

namespace RogueAssistantUI.Assistant.Views
{
    // TEMP - Until have method to connect
    public class MultiplayerConnetView : IRogueAssistantView
    {
        private string m_Address = "localhost";
        private int m_Port = MultiplayerServerBehaviour.c_DefaultPort;

        public string GetViewName(RogueAssistant assistant)
        {
            return "Multiplayer";
        }

        public bool IsViewVisible(RogueAssistant assistant)
        {
            return !assistant.HasBehaviour<MultiplayerClientBehaviour>() && !assistant.HasBehaviour<MultiplayerServerBehaviour>();
        }

        public void SubmitUI(RogueAssistant assistant)
        {
            ImGui.InputText("Address", ref m_Address, 256);
            ImGui.InputInt("Port", ref m_Port);

            if (ImGui.Button("Host"))
            {
                assistant.AddBehaviour(new MultiplayerServerBehaviour(m_Port));
            }

            ImGui.SameLine();
            if (ImGui.Button("Join"))
            {
                assistant.AddBehaviour(new MultiplayerClientBehaviour(m_Address, m_Port));
            }
        }

        public void SubmitSecondaryUI(RogueAssistant assistant)
        {
        }
    }
}
