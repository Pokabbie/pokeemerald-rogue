using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game
{
	public struct GameConnectionHeader
	{
		public uint GameVersion;
		public uint GameLanguage;
		public string GameName;
		public uint MonSpeciesNamesAddress;
		public uint MoveNamesAddress;
		public uint NationalDexCount;
		public uint RogueHeaderAddress;

		public uint RogueInputBufferSize;
		public uint RogueInputBufferAddress;
		public uint RogueOutputBufferSize;
		public uint RogueOutputBufferAddress;
	}
}
