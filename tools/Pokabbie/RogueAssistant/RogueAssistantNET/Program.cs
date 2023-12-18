using RogueAssistantNET.Assistant.Behaviours;
using RogueAssistantNET.Assistant;
using RogueAssistantNET.Game;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET
{

	class Program
	{
		// TESTING
		// DO NOT CHECK IN
		static uint Rng(ref uint seed)
		{
			seed = 1103515245 * (seed) + 24691;
			ushort result = (ushort)(seed >> 16);
			return (uint)result;
		}

		static uint InplaceRandomise(uint offset, uint min, uint max, uint seed)
		{
			uint finalMin = min;
			uint finalRange = (max - min + 1);
			uint finalOffset = Rng(ref seed);

			while (true)
			{
				uint range = (max - min + 1);

				uint splitMarker = 1 + Rng(ref seed) % (range - 1); // Don't place split marker at 0
				uint invSplitMarker = range - splitMarker; // Don't place split marker at 0

				// We now have 2 sets min -> split - 1 & split -> max
				if ((Rng(ref seed) % 2) == 0)
				{
					// Keep sets in current order
					if (offset < splitMarker)
					{
						// take current LHS
						max = min + splitMarker - 1;
					}
					else
					{
						// take current RHS
						min = min + splitMarker;
						offset -= splitMarker;

						seed = ~seed;
					}
				}
				else
				{

					// We're going to essentially swap them over
					if (offset < invSplitMarker)
					{
						// take old RHS
						min = min + splitMarker;
					}
					else
					{
						// take old LHS
						max = min + splitMarker - 1;
						offset -= invSplitMarker;

						seed = ~seed;
					}
				}

				if(min == max)
				{
					// We've found our value
					uint unshiftedvalue = min + finalOffset;
					return finalMin + (unshiftedvalue % finalRange);
				}
			}
		}

		static void Main(string[] args)
		{
			uint range = 100;
			List<List<uint>> counts = new List<List<uint>>((int)range);

			for (int i = 0; i < range; ++i)
			{
				List<uint> list = new List<uint>();

				for (int j = 0; j < range; ++j)
					list.Add(0);

				counts.Add(list);

			}

			for (int j = 0; j < 100; ++j)
			{
				int seed = j;
				//Console.WriteLine($"\nSeed = {seed}");
				for (uint i = 0; i < range; ++i)
				{
					uint val = InplaceRandomise(i, 0, range - 1, (uint)seed);

					++counts[(int)i][(int)val];

					//Console.WriteLine($"{i}\t= {val}");
				}
			}

			for (uint i = 0; i < range; ++i)
			{
				Console.WriteLine($"Index {i}");

				for (uint val = 0; val < range; ++val)
				{
					Console.WriteLine($"\t{val} = {counts[(int)i][(int)val]}");
				}

			}
			//RogueAssistantController controller = new RogueAssistantController();
			//controller.OnUpdate += OnUpdate;
			//
			//controller.Run();
			return;
		}

		private static void OnUpdate(RogueAssistant assistant)
		{
			if (assistant.State.RequestState == GameRequestState.MultiplayerHost)
			{
				if (!assistant.HasBehaviour<MultiplayerServerBehaviour>())
				{
					Console.WriteLine("== Multiplayer [HOST] ==");
					Console.Write($"Desired port (default: {MultiplayerServerBehaviour.c_DefaultPort}): ");

					if (!int.TryParse(Console.ReadLine(), out int port))
					{
						port = MultiplayerServerBehaviour.c_DefaultPort;
					}

					assistant.AddBehaviour(new MultiplayerServerBehaviour(port));
				}
			}
			else if (assistant.State.RequestState == GameRequestState.MultiplayerJoin)
			{
				if (!assistant.HasBehaviour<MultiplayerClientBehaviour>())
				{
					Console.WriteLine("== Multiplayer [JOIN] ==");
					Console.Write($"Host address: ");

					string address = Console.ReadLine() ?? "";
					assistant.AddBehaviour(new MultiplayerClientBehaviour(address));
				}
			}
		}
	}
}
