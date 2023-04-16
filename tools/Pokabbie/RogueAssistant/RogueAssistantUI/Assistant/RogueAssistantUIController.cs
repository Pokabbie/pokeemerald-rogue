using ImGuiNET;
using RogueAssistantNET.Assistant;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantUI.Assistant
{
	public class RogueAssistantUIController
	{
        public enum EmulatorOption
        {
            None,
            mGBA,
            Other,
        }

        private RogueAssistantController m_Assistant = null;
        private Thread m_UpdateThread = null;
        private uint m_ViewportDockID = 0;
        private float m_DeltaTime = 0.0f;

		private EmulatorOption m_TutorialOption = EmulatorOption.None;
		private List<IRogueAssistantView> m_AvailableViews = new List<IRogueAssistantView>();

#if DEBUG
        private HashSet<IRogueAssistantView> m_ForcedVisibleViews = new HashSet<IRogueAssistantView>();
#endif


        public RogueAssistantUIController()
        {
            m_Assistant = new RogueAssistantController();

			m_UpdateThread = new Thread(ThreadFunc);
            m_UpdateThread.Start();

			// Construct views
			var viewtypes = AppDomain.CurrentDomain.GetAssemblies()
                .SelectMany(s => s.GetTypes())
                .Where(p => typeof(IRogueAssistantView).IsAssignableFrom(p) && !p.IsAbstract && p.IsClass);

            foreach (var viewtype in viewtypes) 
            {
                m_AvailableViews.Add((IRogueAssistantView)Activator.CreateInstance(viewtype));
            }
        }

        private void ThreadFunc()
        {
			m_Assistant.Run();
        }

        public void SubmitUI(float deltaTime)
        {
            m_ViewportDockID = ImGui.DockSpaceOverViewport();
            m_DeltaTime = deltaTime;

#if DEBUG
            if (ImGui.BeginMainMenuBar())
            {
                if (ImGui.BeginMenu("Debug"))
				{
					if (m_Assistant.ActiveAssistants.Any())
                    {
                        var defaultAssistant = m_Assistant.ActiveAssistants.First();

						if (ImGui.BeginMenu("Open View..."))
                        {
                            foreach (var view in m_AvailableViews)
                            {
                                bool isVisible = m_ForcedVisibleViews.Contains(view);

                                if (ImGui.Checkbox(view.GetViewName(defaultAssistant), ref isVisible))
                                {
                                    if (isVisible)
                                        m_ForcedVisibleViews.Add(view);
                                    else
                                        m_ForcedVisibleViews.Remove(view);
								}

                            }

                            ImGui.EndMenu();
                        }
                        ImGui.EndMenu();
                    }
                }

                ImGui.EndMainMenuBar();
            }
#endif

            if (!m_Assistant.ActiveAssistants.Any())
            {
                ImGui.SetNextWindowDockID(m_ViewportDockID, ImGuiCond.FirstUseEver);

                if (ImGui.Begin("Waiting for connection"))
				{
					ImGui.Text("How to connect to Pokemon Emerald Rogue");
					ImGui.Separator();
					ImGui.Text("Which emulator are you using to play the game?");

					if (ImGui.Button("mGBA"))
					{
                        m_TutorialOption = EmulatorOption.mGBA;
					}

                    ImGui.SameLine();
					if (ImGui.Button("Other..."))
					{
						m_TutorialOption = EmulatorOption.Other;
					}

                    switch(m_TutorialOption)
					{
						case EmulatorOption.mGBA:
							ImGui.Text("\nTo connect RogueAssistant to mGBA:");
                            ImGui.Separator();
                            ImGui.Text("1.) Make sure your mGBA version is at least 0.10.0");
							ImGui.Text("2.) Make sure Pokemon Emerald Rogue is running in mGBA");
							ImGui.Text("3.) In mGBA select Tools > Scripting...");
							ImGui.Text("4.) In the new Scripting window select File > Load script...");
							ImGui.Text("5.) Select the RogueAssistant_mGBA.lua file that you previously downloaded\n (It should be in the same zip folder which contained the .ups patch files)");
							ImGui.Text("\nAfter completing these steps, RogueAssistant will be connected automatically");
							break;

						case EmulatorOption.Other:
							ImGui.Text("\nUnfortunately, your emulator isn't currently supported by RogueAssistant");
							ImGui.Text("Please download an alternate emulator to use RogueAssistant");
							break;
					}
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

        private bool IsViewVisible(RogueAssistant assistant, IRogueAssistantView view)
        {
#if DEBUG
            if (m_ForcedVisibleViews.Contains(view))
                return true;
#endif
            return view.IsViewVisible(assistant);
        }

        private void SubmitAssistantUI(RogueAssistant assistant, int id)
        {
            ImGui.SetNextWindowDockID(m_ViewportDockID, ImGuiCond.FirstUseEver);

            var activeViews = m_AvailableViews.Where((v) => IsViewVisible(assistant, v));

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

                    ImGui.Text($"Attached to Game [Version: {assistant.Connection.Header.GameEdition}, Player: {state.PlayerNameStr}]");

                    if (activeViews.Any())
                    {

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
                    else
					{
						ImGui.Text("\nAll set!\nWaiting for game to use RogueAssistant");
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
