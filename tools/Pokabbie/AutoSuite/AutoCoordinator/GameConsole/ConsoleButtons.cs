using System;
using System.Collections.Generic;
using System.Text;

namespace AutoCoordinator.GameConsole
{
	[Flags]
	public enum ConsoleButtons
	{
		None	= 0,
		A		= (1 << 0),
		B		= (1 << 1),
		SELECT	= (1 << 2),
		START	= (1 << 3),
		RIGHT	= (1 << 4),
		LEFT	= (1 << 5),
		UP		= (1 << 6),
		DOWN	= (1 << 7),
		R		= (1 << 8),
		L		= (1 << 9),
	}
}
