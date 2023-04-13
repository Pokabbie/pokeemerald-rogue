using RogueAssistantNET.Assistant;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantUI.Assistant
{
	public interface IRogueAssistantView
    {
        bool IsViewVisible(RogueAssistant assistant);

        string GetViewName(RogueAssistant assistant);

        void SubmitUI(RogueAssistant assistant);

        void SubmitSecondaryUI(RogueAssistant assistant);
    }
}
