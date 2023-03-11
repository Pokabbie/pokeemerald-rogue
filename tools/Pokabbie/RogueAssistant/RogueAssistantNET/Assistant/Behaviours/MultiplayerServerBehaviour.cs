using RogueAssistantNET.Assistant.Utilities;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace RogueAssistantNET.Assistant.Behaviours
{
	public class MultiplayerServerBehaviour : IRogueAssistantBehaviour
	{
		public const string c_Handshake1 = "8Tr3tolLRgywSHHKmI0gGWbdNhuplYhB";
		public const string c_Handshake2 = "lyCOm4XQIj8UEaoManH7wB4lKEm6zBAF"; 

		private NetPlayerSyncBehaviour m_PlayerSync;
		private TcpListenerServer m_Server;

		private ConcurrentQueue<TcpClient> m_NewConnections = new ConcurrentQueue<TcpClient>();
		private List<TcpClient> m_ActiveConnections = new List<TcpClient>();
		private Dictionary<Socket, NetPlayerData> m_ConnectionPlayerData = new Dictionary<Socket, NetPlayerData>();

		public MultiplayerServerBehaviour(int port)
		{
			m_Server = new TcpListenerServer(IPAddress.Any, port);
		}

		public bool ReachedMaxPlayerCount
		{
			get => m_ActiveConnections.Count >= m_PlayerSync.MaxPlayerCount - 1;
		}

		public void OnAttach(RogueAssistant assistant)
		{
			Console.WriteLine($"== Openning Server ==");
			m_PlayerSync = assistant.FindOrCreateBehaviour<NetPlayerSyncBehaviour>();
			m_PlayerSync.RefreshLocalPlayer(assistant.Connection);

			m_Server.Open(1);
		}

		public void OnDetach(RogueAssistant assistant)
		{
		}

		public void OnUpdate(RogueAssistant assistant)
		{
			if(m_Server.TryAccept(out TcpClient newClient))
			{
				Task.Run(() =>
				{
					if (TryAccept(assistant, newClient))
						m_NewConnections.Enqueue(newClient);
					else
						newClient.Close();
				});
			}

			if(m_NewConnections.Count != 0 && m_NewConnections.TryDequeue(out newClient))
			{
				m_ActiveConnections.Add(newClient);

				var asyncArgs = new SocketAsyncEventArgs();
				asyncArgs.SetBuffer(new byte[2048], 0, 2048);
				asyncArgs.Completed += OnTcpClientRecieve;

				newClient.Client.ReceiveAsync(asyncArgs);
			}

			foreach(var client in m_ActiveConnections)
			{
				SendPlayerData(client.Client, m_PlayerSync.LocalPlayer);

				foreach(var kvp in m_ConnectionPlayerData)
				{
					if(kvp.Key != client.Client)
					{
						SendPlayerData(client.Client, kvp.Value);
					}
				}
			}
		}

		private void SendPlayerData(Socket socket, NetPlayerData player)
		{
			socket.Send(new NetworkPacket(NetworkChannel.NetPlayerProfile, player.ProfileData).ToBinaryBlob());
			socket.Send(new NetworkPacket(NetworkChannel.NetPlayerState, player.StateData).ToBinaryBlob());
		}

		private void OnTcpClientRecieve(object sender, SocketAsyncEventArgs asyncArgs)
		{
			Socket socket = (Socket)sender;

			foreach(var packet in NetworkPacket.ParsePackets(asyncArgs.Buffer, asyncArgs.Offset, asyncArgs.BytesTransferred))
			{
				switch(packet.Channel)
				{
					case NetworkChannel.NetPlayerProfile:
						{
							NetPlayerData playerData = GetPlayerData(socket);
							playerData.ProfileData = packet.Content;
							break;
						}

					case NetworkChannel.NetPlayerState:
						{
							NetPlayerData playerData = GetPlayerData(socket);
							playerData.StateData = packet.Content;
							break;
						}
				}
			}

			socket.ReceiveAsync(asyncArgs);
		}

		public bool TryAccept(RogueAssistant assistant, TcpClient client)
		{
			Console.WriteLine($"Player connecting...");
			client.ReceiveTimeout = 15 * 1000;
			client.SendTimeout = 15 * 1000;

			if (ReachedMaxPlayerCount)
			{
				Console.WriteLine($"Rejecting (Reached Max Players)");
				return false;
			}

			byte[] buffer = new byte[4096];

			bool VerifyString(string match)
			{
				int readCount = client.Client.Receive(buffer);
				string incoming = Encoding.ASCII.GetString(buffer, 0, readCount);

				if (incoming != match)
					return false;

				// Send 2nd continue signal
				int count = Encoding.ASCII.GetBytes("OK", 0, 2, buffer, 0);
				client.Client.Send(buffer, 0, count, SocketFlags.None);
				return true;
			}

			// Check handshakes
			if(!VerifyString(c_Handshake1) || !VerifyString(c_Handshake2))
			{
				Console.WriteLine($"Rejecting (Failed to verify)");
				return false;
			}

			// Check game version
			if (!VerifyString(assistant.Connection.Header.GameEdition.ToString()))
			{
				Console.WriteLine($"Rejecting (Incompatible GameEdition)");
				return false;
			}

			return true;
		}

		public NetPlayerData GetPlayerData(Socket socket)
		{
			NetPlayerData player;
			if (m_ConnectionPlayerData.TryGetValue(socket, out player))
				return player;

			player = new NetPlayerData();
			m_ConnectionPlayerData.Add(socket, player);
			m_PlayerSync.AddOnlinePlayer(player);

			return player;
		}
	}
}
