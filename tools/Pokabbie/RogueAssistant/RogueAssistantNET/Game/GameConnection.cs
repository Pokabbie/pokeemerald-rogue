using RogueAssistantNET.Game.Commands;
using RogueAssistantNET.Game.Commands.Connection;
using RogueAssistantNET.Game.Commands.Game;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RogueAssistantNET.Game
{
	public class GameConnectionResponse
	{
		public byte[] Buffer = new byte[2048];
		public int Count = 0;

		public override string ToString()
		{
			StringBuilder str = new StringBuilder();
			
			foreach(char c in Encoding.ASCII.GetChars(Buffer, 0, Count))
			{
				if (c == '\0')
					break;

				str.Append(c);
			}

			return str.ToString();
		}
	}

	public class GameConnection
	{
		private GameConnectionHeader m_Header = default;
		private TcpClient m_Client = null;

		private Queue<CommandRequest> m_CommandQueue = new Queue<CommandRequest>();
		private GameState m_State = null;
		private Random m_RNG = new Random();

		public GameConnection(TcpClient client)
		{
			m_Client = client;
			EnqueueCommand(new VerifyGameCommand()).Then((VerifyGameCommand result) => 
			{
				if(result.HasConnected)
				{
					Console.WriteLine($"Game verified.");

					m_State = new GameState(this);

					uint a = m_State.GetConstantValue(GameStateConstant.NetPlayerCapacity);
					uint b = m_State.GetConstantValue(GameStateConstant.NetPlayerAddress);

					EnqueueCommand(new EchoGameCommand());
				}
				else
				{
					Console.WriteLine($"Failed to verify game.");
					// TODO - Disconnect???
				}
			});
		}

		public GameConnectionHeader Header
		{
			get => m_Header;
			set => m_Header = value;
		}

		public uint GameCommandArgAddress
		{
			get => m_Header.RogueInputBufferAddress + 4;
		}

		public GameState State
		{
			get => m_State;
		}

		public Random RNG
		{
			get => m_RNG;
		}

		public void Update()
		{
			if (m_CommandQueue.Count != 0)
			{
				var currCmd = m_CommandQueue.Peek();

				currCmd.Execute(this);

				m_CommandQueue.Dequeue();
			}

			m_State.Update();
		}

		public CommandRequest EnqueueCommand(IConnectionCommandBehaviour cmd)
		{
			var req = new CommandRequest(cmd);
			m_CommandQueue.Enqueue(req);
			return req;
		}

		public GameConnectionResponse SendInternal(string command, params object[] args)
		{
			string commandStr = CreateToCommandString(command, args);
			byte[] commandBuffer = Encoding.ASCII.GetBytes(commandStr);

			m_Client.Client.Send(commandBuffer, 0, commandBuffer.Length, SocketFlags.None);

			GameConnectionResponse res = new GameConnectionResponse();
			res.Count = m_Client.Client.Receive(res.Buffer);
			return res;
		}

		public static string CreateToCommandString(string command, params object[] args)
		{
			foreach (var arg in args)
			{
				if (arg is byte[])
				{
					foreach (var b in (byte[])arg)
						command += ";" + b;
				}
				else
					command += ";" + arg;
			}

			return command;
		}


		public uint SendReliable(GameCommandCode code)
		{
			ushort token = m_State.NextCommandToken();
			WriteU16(m_Header.RogueInputBufferAddress + 2, (ushort)code);
			WriteU16(m_Header.RogueInputBufferAddress + 0, token); // Write token last, after the params have been setup (Now game will know it's ready to execute)

			ushort lastGameToken = 0;

			do
			{
				Thread.Sleep(TimeSpan.FromMilliseconds(5));
				lastGameToken = ReadU16(m_Header.RogueOutputBufferAddress);
			}
			while (token != lastGameToken);

			return m_Header.RogueOutputBufferAddress + 2;
		}

		public void SendUnreliable(GameCommandCode code)
		{
			ushort token = m_State.NextCommandToken();
			WriteU16(m_Header.RogueInputBufferAddress + 2, (ushort)code);
			WriteU16(m_Header.RogueInputBufferAddress + 0, token); // Write token last, after the params have been setup (Now game will know it's ready to execute)

			// Ignore here, can be overwritten
		}

		public byte ReadU8(uint address)
		{
			GameConnectionResponse res = SendInternal("readByte", address);
			return byte.Parse(res.ToString());
		}

		public sbyte ReadS8(uint address)
		{
			GameConnectionResponse res = SendInternal("readByte", address);
			return sbyte.Parse(res.ToString());
		}

		public ushort ReadU16(uint address)
		{
			GameConnectionResponse res = SendInternal("readBytes", address, sizeof(ushort));
			return BitConverter.ToUInt16(res.Buffer, 0);
		}

		public short ReadS16(uint address)
		{
			GameConnectionResponse res = SendInternal("readBytes", address, sizeof(short));
			return BitConverter.ToInt16(res.Buffer, 0);
		}

		public uint ReadU32(uint address)
		{
			GameConnectionResponse res = SendInternal("readBytes", address, sizeof(uint));
			return BitConverter.ToUInt32(res.Buffer, 0);
		}

		public int ReadS32(uint address)
		{
			GameConnectionResponse res = SendInternal("readBytes", address, sizeof(int));
			return BitConverter.ToInt32(res.Buffer, 0);
		}

		public void WriteU8(uint address, byte value)
		{
			SendInternal("writeByte", address, value);
		}

		public void WriteS8(uint address, sbyte value)
		{
			SendInternal("writeByte", address, value);
		}

		public void WriteU16(uint address, ushort value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			SendInternal("writeBytes", address, bytes);
		}

		public void WriteS16(uint address, short value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			SendInternal("writeBytes", address, bytes);
		}

		public void WriteU32(uint address, uint value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			SendInternal("writeBytes", address, bytes);
		}

		public void WriteS32(uint address, int value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			SendInternal("writeBytes", address, bytes);
		}

		public void WriteRange(uint address, byte[] data)
		{
			SendInternal("writeBytes", address, data);
		}

		public string ReadAsciiStringFixed(uint address, uint length)
		{
			GameConnectionResponse res = SendInternal("readBytes", address, length);
			return res.ToString();
		}

		public string ReadGameStringFixed(uint address, uint length)
		{
			GameConnectionResponse res = SendInternal("readBytes", address, length);
			return GameString.ConvertBytes(res.Buffer, length);
		}
	}
}
