using RogueAssistantNET.Assistant;
using RogueAssistantNET.Assistant.Behaviours;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantUI.Assistant.Views
{
#if DEBUG
    public class NetPlayerSyncBehaviourView : IRogueAssistantView
	{
		public string GetViewName(RogueAssistant assistant)
		{
			return "[DEBUG]NetPlayerSync";
		}

		public bool IsViewVisible(RogueAssistant assistant)
		{
			return assistant.HasBehaviour<NetPlayerSyncBehaviour>();
        }

        public void SubmitUI(RogueAssistant assistant)
        {
			var playerSync = assistant.FindBehaviour<NetPlayerSyncBehaviour>();
        }

        public void SubmitSecondaryUI(RogueAssistant assistant)
		{
		}
	}
#endif
}
