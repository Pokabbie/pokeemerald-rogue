using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant
{
	public interface IRogueAssistantBehaviour
	{
		void OnAttach(RogueAssistant assistant);

		void OnDetach(RogueAssistant assistant);

		void OnUpdate(RogueAssistant assistant);
	}
}
