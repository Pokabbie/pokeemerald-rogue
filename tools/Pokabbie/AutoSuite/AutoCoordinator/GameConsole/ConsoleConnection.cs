using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;

namespace AutoCoordinator.GameConsole
{
	public class ConsoleConnection
	{
		private ConsoleCommunication m_Connection;

		public ConsoleConnection(IPEndPoint endPoint)
		{
			m_Connection = new ConsoleCommunication(endPoint);
		}

		public void Cmd_Hello()
		{
			Console.WriteLine($"From connection: '{m_Connection.SendMessage("hello")}'");
		}

		public byte Cmd_Emu_Read8(uint addr)
		{
			string result = m_Connection.SendMessage("emu_read8", addr.ToString());
			if (byte.TryParse(result, out byte value))
				return value;

			Console.WriteLine($"Error when parsing byte '{result}'");
			return 0;
		}

		public ushort Cmd_Emu_Read16(uint addr)
		{
			string result = m_Connection.SendMessage("emu_read16", addr.ToString());
			if (ushort.TryParse(result, out ushort value))
				return value;

			Console.WriteLine($"Error when parsing ushort '{result}'");
			return 0;
		}

		public uint Cmd_Emu_Read32(uint addr)
		{
			string result = m_Connection.SendMessage("emu_read32", addr.ToString());
			if (uint.TryParse(result, out uint value))
				return value;

			Console.WriteLine($"Error when parsing uint '{result}'");
			return 0;
		}

		public byte[] Cmd_Emu_ReadRange(uint addr, uint range)
		{
			Console.WriteLine($"TODO - Cmd_Emu_ReadRange");
			return null;
		}

		public void Cmd_Emu_Write8(uint addr, byte value)
		{
			m_Connection.SendMessage("emu_write8", addr.ToString(), value.ToString());
		}

		public void Cmd_Emu_Write16(uint addr, ushort value)
		{
			m_Connection.SendMessage("emu_write16", addr.ToString(), value.ToString());
		}

		public void Cmd_Emu_Write32(uint addr, uint value)
		{
			m_Connection.SendMessage("emu_write32", addr.ToString(), value.ToString());
		}

		public void Cmd_Emu_SetKeys(ConsoleButtons buttons)
		{
			m_Connection.SendMessage("emu_setkeys", ((uint)buttons).ToString());
		}

		public void Cmd_Emu_TapKeys(ConsoleButtons buttons)
		{
			// Not great, but this hack will do for now
			Cmd_Emu_SetKeys(buttons);
			Thread.Sleep(30);
			Cmd_Emu_SetKeys(ConsoleButtons.None);
		}
	}
}
