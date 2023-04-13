using ImGuiNET;
using RogueAssistantNET.Assistant;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantUI.Assistant
{
	public class RogueAssistantUIController
	{
        private RogueAssistantController m_Assistant = null;
        private uint m_ViewportDockID = 0;

        private List<IRogueAssistantView> m_AvailableViews = new List<IRogueAssistantView>();

        public RogueAssistantUIController()
        {
            m_Assistant = new RogueAssistantController();
            m_Assistant.Open();

            // Construct views
            var viewtypes = AppDomain.CurrentDomain.GetAssemblies()
                .SelectMany(s => s.GetTypes())
                .Where(p => typeof(IRogueAssistantView).IsAssignableFrom(p) && !p.IsAbstract && p.IsClass);

            foreach (var viewtype in viewtypes) 
            {
                m_AvailableViews.Add((IRogueAssistantView)Activator.CreateInstance(viewtype));
            }
        }

        private void Update()
        {
            m_Assistant.Update();
        }

		public void SubmitUI()
        {
            Update();

            m_ViewportDockID = ImGui.DockSpaceOverViewport();

            //if (ImGui.BeginMainMenuBar())
            //{
            //    if (ImGui.BeginMenu("File"))
            //    {
            //        ImGui.EndMenu();
            //    }
            //    if (ImGui.BeginMenu("Edit"))
            //    {
            //        ImGui.EndMenu();
            //    }
            //    ImGui.EndMainMenuBar();
            //}

            if (!m_Assistant.ActiveAssistants.Any())
            {
                ImGui.SetNextWindowDockID(m_ViewportDockID, ImGuiCond.FirstUseEver);

                if (ImGui.Begin("Waiting for connection"))
                {
                    ImGui.Text("Waiting for connection..");
                }
                ImGui.End();
            }
            else
            {
                int id = 0;
                foreach(var assistant in m_Assistant.ActiveAssistants.ToArray())
                {
                    ImGui.PushID(assistant.GetHashCode());
                    SubmitAssistantUI(assistant, ++id);
                    ImGui.PopID();
                }
            }
        }

        private void SubmitAssistantUI(RogueAssistant assistant, int id)
        {
            ImGui.SetNextWindowDockID(m_ViewportDockID, ImGuiCond.FirstUseEver);

            var activeViews = m_AvailableViews.Where((v) => v.IsViewVisible(assistant));

            // Primary UI window
            if (ImGui.Begin($"Connection {id}###{assistant.GetHashCode()}"))
            {
                if (assistant.HasDisconnected)
                {
                    ImGui.Text($"Game Disconnected.");
                    ImGui.Text($"\nReason:\n{assistant.DisconnectionMessage}");
                    if (ImGui.Button("Close Tab"))
                    {
                        m_Assistant.RemoveAssistant(assistant);
                    }
                }
                else if (!assistant.HasInitialised)
                {
                    ImGui.Text($"Initialising...");
                }
                else
                {
                    var state = assistant.Connection.State;

                    ImGui.Text($"Attached to Game (Name: {state.PlayerNameStr})");

                    if (ImGui.BeginTabBar("###views"))
                    {
                        foreach (var view in activeViews)
                        {
                            if (ImGui.BeginTabItem(view.GetViewName(assistant) + "###" + view.GetHashCode()))
                            {
                                view.SubmitUI(assistant);
                                ImGui.EndTabItem();
                            }
                        }

                        ImGui.EndTabBar();
                    }
                }
            }
            ImGui.End();

            if (assistant.HasInitialised && !assistant.HasDisconnected)
            {
                // Secondary UI, so views can create extra windows
                foreach (var view in activeViews)
                    view.SubmitSecondaryUI(assistant);
            }
        }
	}
}
